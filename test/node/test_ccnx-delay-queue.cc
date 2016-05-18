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

#include "ns3/test.h"
#include "../TestMacros.h"
#include "ns3/ccnx-delay-queue.h"
#include "ns3/nstime.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxDelayQueue {

class MockQueueItem : public SimpleRefCount<MockQueueItem>
{
public:
  MockQueueItem (unsigned id, Time finishTime) : m_id (id), m_finishTime (finishTime)
  {
  }
  virtual ~MockQueueItem ()
  {
  }
  unsigned GetId () const
  {
    return m_id;
  }
  Time GetFinishTime () const
  {
    return m_finishTime;
  }
private:
  unsigned m_id;
  Time m_finishTime;
};

typedef CCNxDelayQueue<MockQueueItem> QueueType;

BeginTest (Constructor)
{
  unsigned servers = 1;
  QueueType::GetServiceTimeCallback getTime = MakeCallback (&Constructor::GetServiceTime, this);
  QueueType::DequeueCallback service = MakeCallback (&Constructor::Service, this);

  Ptr<QueueType> queue = Create< QueueType > (servers, getTime, service);

  bool exists = (queue);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
}

Time GetServiceTime (Ptr<MockQueueItem> item)
{
  return Seconds (1);
}

void Service (Ptr<MockQueueItem> item)
{

}
EndTest ()

/**
 * This test will create a 2-server queue.  It will use a service time of 3 seconds and insert
 * queue entries every 0.5 second for 5 items beginnig at time 1
 *
 * Time  Id    Action
 *  1.0   1    enqueue
 *  1.0   1    begin service
 *  1.5   2    enqueue
 *  1.5   2    begin service
 *  2.0   3    enqueue
 *  2.5   4    enqueue
 *  3.0   5    enqueue
 *  4.0   1    end service
 *  4.0   3    begin service
 *  4.5   2    end service
 *  4.5   4    begin service
 *  7.0   3    end service
 *  7.0   5    begin service
 *  7.5   4    end service
 *  10.0  5    end service
 */
BeginTest (Timing)
{

  unsigned servers = 2;
  QueueType::GetServiceTimeCallback getTime = MakeCallback (&Timing::GetServiceTime, this);
  QueueType::DequeueCallback service = MakeCallback (&Timing::Service, this);

  m_queue = Create< QueueType > (servers, getTime, service);

  struct
  {
    unsigned id;
    Time arrival;
    Time begin;
    Time end;
  } testVectors[] = {
    { 1, Seconds (1.0f), Seconds (1.0f), Seconds (4.0f) },
    { 2, Seconds (1.5f), Seconds (1.5f), Seconds (4.5f) },
    { 3, Seconds (2.0f), Seconds (4.0f), Seconds (7.0f) },
    { 4, Seconds (2.5f), Seconds (4.5f), Seconds (7.5f) },
    { 5, Seconds (3.0f), Seconds (7.0f), Seconds (10.0f) },
    { 0, Seconds (0), Seconds (0), Seconds (0) }
  };

  for (int i = 0; testVectors[i].id > 0; i++)
    {
      // The id starts at 1. It's just for reference, no actual meaning to it
      Ptr<MockQueueItem> item = Create<MockQueueItem> (i + 1, testVectors[i].end);
      Simulator::Schedule (testVectors[i].arrival, &Timing::GenerateTraffic, this, item);
    }

  m_completedItems = 0;
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_EXPECT_MSG_EQ (m_completedItems, 5, "Incorrect number of completed items");
}


Ptr<QueueType> m_queue;
unsigned m_completedItems;

void GenerateTraffic (Ptr<MockQueueItem> item)
{
  printf ("%6.3f push_back %u\n", Simulator::Now ().GetSeconds (), item->GetId ());
  m_queue->push_back (item);
}

Time GetServiceTime (Ptr<MockQueueItem> item)
{
  return Seconds (3);
}

void Service (Ptr<MockQueueItem> item)
{
  printf ("%6.3f Service  %u\n", Simulator::Now ().GetSeconds (), item->GetId ());
  NS_TEST_EXPECT_MSG_EQ (Simulator::Now (), item->GetFinishTime (), "Incorrect finish time for " << item->GetId ());
  m_completedItems++;
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxTime
 */
static class TestSuiteCCNxDelayQueue : public TestSuite
{
public:
  TestSuiteCCNxDelayQueue () : TestSuite ("ccnx-delay-queue", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new Timing (), TestCase::QUICK);
  }
} g_TestSuiteCCNxDelayQueue;

} // namespace TestSuiteCCNxDelayQueue
