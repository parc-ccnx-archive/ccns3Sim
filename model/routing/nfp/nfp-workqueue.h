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

#ifndef CCNX_MODEL_ROUTING_NFP_NFP_WORKQUEUE_H_
#define CCNX_MODEL_ROUTING_NFP_NFP_WORKQUEUE_H_

#include <set>
#include <deque>

#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-name.h"
#include "ns3/nfp-workqueue-entry.h"

namespace ns3 {
namespace ccnx {

/**
 * \ingroup nfp-routing
 *
 * The work queue tracks (anchorName, prefix) pairs.  When something is inserted into the work queue,
 * it is a book mark for the routing protocol that it has to do something with that pair.
 *
 * The work queue stores entries in FIFO order and a given (anchorName, prefix) pair can only
 * be in the queue once.  If it is added multiple times, only the first one stays (and stays at its original place
 * in the queue).
 */
class NfpWorkQueue : public SimpleRefCount<NfpWorkQueue>
{
public:
  /**
   * Creates a work queue with an empty set of things to do.
   */
  NfpWorkQueue ();

  /**
   *
   */
  virtual ~NfpWorkQueue ();

  /**
   * Creates a new work queue entry for the given anchor and prefix.  This is
   * essentially just a book mark that something needs to be done for that pair.
   *
   * @param anchorName
   * @param prefix
   */
  void push_back (Ptr<NfpWorkQueueEntry> entry);

  /**
   * Pops the front entry.
   *
   * PRECONDITION: is not empty().
   */
  Ptr<NfpWorkQueueEntry> pop_front (void);

  /**
   * Determines if the queue is empty
   * @return true if empty, false otherwise
   */
  bool empty (void) const;

  // =================================================================

protected:
  /**
   * The work queue set maintains the uniqueness of entries
   */
  typedef std::set< Ptr<NfpWorkQueueEntry>, NfpWorkQueueEntry::islessPtrWorkQueueEntry > WorkQueueSetType;
  WorkQueueSetType m_queueSet;

  /**
   * The work queue dequeue maintains the FIFO order
   */
  typedef std::deque< Ptr<NfpWorkQueueEntry> > WorkQueueDequeType;
  WorkQueueDequeType m_queue;

  /**
   * Sanity check the data structures
   */
  void AssertInvariants (void) const;
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNX_MODEL_ROUTING_NFP_NFP_WORKQUEUE_H_ */
