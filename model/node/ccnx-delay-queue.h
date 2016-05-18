/*
 * Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 *
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */

#ifndef CCNS3SIM_MODEL_NODE_CCNX_DELAY_QUEUE_H_
#define CCNS3SIM_MODEL_NODE_CCNX_DELAY_QUEUE_H_

#include <deque>
#include <vector>
#include "ns3/simple-ref-count.h"
#include "ns3/nstime.h"
#include "ns3/callback.h"
#include "ns3/timer.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-l3
 *
 * The CCNxDelayQueue is a general queuing class to delay events.  Several of
 * the major CCNx forwarding components have delay queues to model computation time.
 *
 * N.B.: Because this is a class template, all implementation must be in the header
 * file so the compiler can generate the template specializations from user code.
 *
 * @subsection Overview
 * @image html delay-queue-1.png
 *
 * A `CCNxDelayQueue<T>` is constructed with the number of parallel servers of
 * the queue (which enforce the delay) and two functions.  The first function
 * `GetServiceTime(T item)` is called whenever a queue item is moved to the head-of-line
 * and there is a free server for it.  The second function `DequeueCallback(T item)`
 * is called after the service time is over.  It dequeues the item back to the user's code.
 *
 * @subsection Input Delay Model
 * @image html delay-queue-2.png
 *
 * When used to model input delay, the `GetServiceTime(T item)` function usually
 * performs only a simple calculation for the service time or returns some pre-determined
 * value.  Once the service time is over, the `DequeueCallback(T item)` function carries
 * on processing the queue item.
 *
 * @subsection Processing Delay Model
 * @image html delay-queue-3.png
 *
 * When used to model processing delay, the typical usage pattern is for
 * `GetServiceTime(T item)` to perform a calculation on `item` and put the result
 * back in item (e.g. `item->SetResult(...)`) and then return the amount of
 * simulation delay to wait.  Once the delay is over and the item is passed
 * to `DequeueCallback(T item)`, no further calculation is done and `item` is
 * passed to the next step in the service pipeline.
 */

template <class T>
class CCNxDelayQueue : public SimpleRefCount< CCNxDelayQueue<T> >
{
public:
  typedef Callback<Time, Ptr<T> > GetServiceTimeCallback;
  typedef Callback<void, Ptr<T> > DequeueCallback;

  CCNxDelayQueue (unsigned servers, GetServiceTimeCallback getServiceTime, DequeueCallback dequeue)
    : m_serverCount (servers), m_GetServiceTime (getServiceTime), m_DequeueCallback (dequeue), m_backlogCount (0)
  {
    NS_ASSERT_MSG (servers > 0, "Cannot specify 0 servers, must be positive.");

    m_servers = ServerArrayType (m_serverCount);
    for (int i = 0; i < m_serverCount; i++)
      {
        m_servers[i] = new Timer (Timer::REMOVE_ON_DESTROY);
        m_servers[i]->SetFunction (&CCNxDelayQueue::TimerExpired, this);
      }
  }

  virtual ~CCNxDelayQueue ()
  {
    for (int i = 0; i < m_serverCount; i++)
      {
        delete m_servers[i];
      }
  }

  /**
   * Enqueues an item.  If there's an available server timer, it will be put
   * at the head-of-line immediately and wait its delay.  Otherwise, it will stay
   * in the backlog queue until it reaches the head-of-line.
   *
   * @param item
   */
  void push_back (Ptr<T> item)
  {
    m_inputQueue.push_back (item);
    m_backlogCount++;

    if (m_backlogCount <= m_serverCount)
      {
        // find a free server
        bool foundServer = false;
        for (int i = 0; i < m_serverCount; i++)
          {
            if (!m_servers[i]->IsRunning ())
              {
                BeginService (m_servers[i]);
                foundServer = true;
                break;
              }
          }
        NS_ASSERT_MSG (foundServer, "backlog <= m_serverCount, but did not find free server");
      }
  }


  /**
   * The number of items in the delay queue, including those
   * being serviced (backlog + serviced)
   *
   * @return
   */
  size_t size () const
  {
    return m_backlogCount;
  }

private:
  /**
   * Begin servicing the top of the input queue in the server `server`.  Will get the
   * delay time by calling m_GetServiceTime().
   *
   * @param server [in] The server to use for the scheduling
   */
  void BeginService (Timer *server)
  {
    if (!m_inputQueue.empty ())
      {
        Ptr<T> item = m_inputQueue.front ();
        m_inputQueue.pop_front ();

        Time serviceTime = m_GetServiceTime (item);
        server->SetDelay (serviceTime);
        server->SetArguments (item, server);
        server->Schedule ();
      }
  }

  /**
   * Function called when a timer expires.  It will pass the `item` to
   * the dequeue function, then check the backlog queue to
   * see if there's a new item to put in the timer.
   *
   * @param item
   * @param server The timer that just expired, which is not free and can be loaded with a new item
   */
  void TimerExpired (Ptr<T> item, Timer *server)
  {
    NS_ASSERT_MSG (m_backlogCount > 0, "Timer expired, but backlog count is 0");

    // It's possible that calling m_DequeueCallback will immediately queue another packet,
    // so do not decrement the backlog until after.  Otherwise, we might get
    // overlapping calls to BeginService()
    m_DequeueCallback (item);

    m_backlogCount--;
    BeginService (server);
  }

private:
  /**
   * The number of parallel heads-of-line (i.e. the parallelism of queue processing)
   */
  unsigned m_serverCount;

  /**
   * Callback to get the service time for a new head-of-line item
   */
  GetServiceTimeCallback m_GetServiceTime;

  /**
   * Callback to actually service an item after it has waited the service time.
   * An item is dequeued and passed to this service function.
   */
  DequeueCallback m_DequeueCallback;

  typedef std::deque< Ptr<T> > QueueType;

  /**
   * This is the delay queue.  FIFO with multiple parallel servers.
   */
  QueueType m_inputQueue;

  /**
   * The array of servers.
   */
  typedef std::vector< Timer * > ServerArrayType;

  /**
   * The number of items in the queue plus servers
   */
  size_t m_backlogCount;

  /**
   * Each head-of-line item is delayed using a Timer.  When the timer expires
   * it calls TimerExpired().  TimerExpired passes the head-of-line item to
   * m_service then checks the backlog to see if there's a new item that
   * can be put in the timer.
   */
  ServerArrayType m_servers;
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_NODE_CCNX_DELAY_QUEUE_H_ */
