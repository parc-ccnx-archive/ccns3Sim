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

#ifndef CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_CONTENT_STORE_LRU_LIST_H_
#define CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_CONTENT_STORE_LRU_LIST_H_

#include <unordered_map>
#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-standard-content-store-entry.h"

namespace ns3 {
namespace ccnx {

class CCNxStandardContentStoreLruList : public ns3::SimpleRefCount<CCNxStandardContentStoreLruList>
{
public:


  CCNxStandardContentStoreLruList ();

  virtual ~CCNxStandardContentStoreLruList ();

  /**
   * The Least Recently Used (lru) list has the least recently used packet at the end of the list,
   * and the most recently used at the beginning. The list allows the LRU algorithm to be used when
   * to evict packets when the content store has reached maximum size.
   *
   * The data structure need to enable the scalable quick operations listed below in priority order:
   *
   * 1. find an entry if present.
   * 2. Add an entry if not present.
   * 3. Delete the least recently used entry.  occurs every time an object is added (once the CS has filled).
   * 4. Refresh an entry if present.
   *
   * This approach uses an unordered map < Ptr<entry>, listIterator> and a list < entry >. The map delivers quick
   * scalable find(1) while the list provides add(2),refresh(4),delete(3).
   *
   * @param entry
   * @return
   **/

  /*
   * Add/Refresh an Entry to/in the LRU.
   */
   bool AddEntry(Ptr<CCNxStandardContentStoreEntry> entry);

  /*
   * Delete an Entry from the LRU. Not a common operation.
   */
   bool DeleteEntry(Ptr<CCNxStandardContentStoreEntry> entry);

   /*
    * Return entry ptr so it can be deleted from Lru and  other structures.
    */

   Ptr<CCNxStandardContentStoreEntry> GetBackEntry() ;

   Ptr<CCNxStandardContentStoreEntry> GetFrontEntry() ;



   uint64_t GetSize() const;


protected:


private:

  typedef std::list<Ptr<CCNxStandardContentStoreEntry> > LruListType;
  LruListType m_lruList;

  typedef std::map <Ptr<CCNxStandardContentStoreEntry>,  LruListType::iterator > LruMapType;
  LruMapType m_lruMap;
};

}   /* namespace ccnx */
} /* namespace ns3 */






#endif /* CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_CONTENT_STORE_LRU_LIST_H_ */
