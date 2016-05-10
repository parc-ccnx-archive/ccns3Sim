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
 * # is up to the contributors to maintain their section in this file up to date
 * # and up to the user of the software to verify any claims herein.
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

#ifndef CCNS3SIM_MODEL_ROUTING_NFP_NFP_PREFIX_TIMER_HEAP_H_
#define CCNS3SIM_MODEL_ROUTING_NFP_NFP_PREFIX_TIMER_HEAP_H_

#include <queue>
#include <map>

#include "ns3/ccnx-name.h"
#include "ns3/nfp-prefix-timer-entry.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup nfp-routing
 *
 * The NfpPrefixTimerHeap keeps an ordered heap of times (Time) for routing advertisements (prefix, anchorName).
 * Whenever NfpRoutingProtocol advertises a (prefix, anchorName), it puts an entry in the heap with the expiry time
 * it wants for the next advertisement.
 *
 * NfpRoutingProtocol can then peak at the top entry in the heap, and if it is expired, pop it off.
 *
 * NfpPrefixTimerHeap will only keep one entry for (prefix, anchorName).  If NfpRoutingProtocol sets a new
 * timer for that pair and one is already in the heap, it will be updated as appropriate.
 *
 * In our actual implementation, we do not do live updates because that would require a linear search of the
 * priority queue to find the old update.  Instead, we have a map by (prefix, anchorName) that stores a reference
 * to the prioirty queue entry and we invalidate it then insert a new entry.  This leaves some crud in the tree
 * that we prune off when it pops to the top.  It is only O(logN), not O(N logN).
 */
class NfpPrefixTimerHeap
{
public:
  NfpPrefixTimerHeap ();
  virtual ~NfpPrefixTimerHeap ();

  /**
   * Inserts an element in to the heap.  If the key (prefix, anchorName) already
   * exists, it is updated to the new expiry time.
   *
   * @param prefix
   * @param anchorName
   * @param expiry
   */
  void Insert (Ptr<const CCNxName> prefix, Ptr<const CCNxName> anchorName, Time expiry);

  /**
   * Returns the top heap element, but keeps it on top.  If the heap is empty, it
   * will return a null Ptr.
   *
   * This method may modify the heap by discarding invalidated timer entries.
   *
   * @return The top element or null Ptr.
   *
   * Example:
   * @code
   * {
   *   Ptr<const NfpPrefixTimerEntry> entry;
   *   while ( (entry = m_prefixTimerHeap.Peek()) ) {
   *     if (entry->GetTime() <= Simulator::Now()) {
   *       // process entry
   *       m_prefixTimerHeap.Pop();
   *     } else {
   *       break;
   *     }
   *   }
   * }
   * @endcode
   */
  Ptr<const NfpPrefixTimerEntry> Peek (void);

  /**
   * Removes the top element from the heap
   *
   * Example:
   * @code
   * {
   *   Ptr<const NfpPrefixTimerEntry> entry;
   *   while ( (entry = m_prefixTimerHeap.Peek()) ) {
   *     if (entry->GetTime() <= Simulator::Now()) {
   *       // process entry
   *       m_prefixTimerHeap.Pop();
   *     } else {
   *       break;
   *     }
   *   }
   * }
   * @endcode
   */
  void Pop (void);

private:
  typedef std::priority_queue< Ptr<NfpPrefixTimerEntry>, std::vector< Ptr<NfpPrefixTimerEntry> >, NfpPrefixTimerEntry::IsTimeGreater > HeapType;
  HeapType m_heap;

  typedef std::map< Ptr<NfpPrefixTimerEntry>, Ptr<NfpPrefixTimerEntry>, NfpPrefixTimerEntry::IsNameLess > KeyMapType;
  KeyMapType m_keys;

};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_ROUTING_NFP_NFP_PREFIX_TIMER_HEAP_H_ */
