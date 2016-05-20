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

#ifndef CCNX_MODEL_ROUTING_NFP_NFP_WORKQUEUE_ENTRY_H_
#define CCNX_MODEL_ROUTING_NFP_NFP_WORKQUEUE_ENTRY_H_

#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-name.h"

namespace ns3 {
namespace ccnx {

class NfpWorkQueueEntry : public SimpleRefCount<NfpWorkQueueEntry>
{
public:
  NfpWorkQueueEntry (Ptr<const CCNxName> anchorName, Ptr<const CCNxName> prefix);
  virtual ~NfpWorkQueueEntry ();

  Ptr<const CCNxName> GetAnchorName (void) const;
  Ptr<const CCNxName> GetPrefix (void) const;

  /**
   * Determine if this object sorts before `other`.
   *
   * The lexicographic sort order is first by prefix, then by anchorName.
   *
   * @param other The other entry to compare against
   * @return true if this object sorts before the other object.
   */
  bool operator < (const NfpWorkQueueEntry &other) const;

  /**
   * Display this work queue entry to the provided output stream
   *
   * @param os
   * @param entry
   * @return
   */
  friend std::ostream & operator << (std::ostream &os, const NfpWorkQueueEntry &entry);

  /**
   * Determine the ordering of two work queue entries.
   *
   * The lexicographic sort order is first by prefix, then by anchorName.
   *
   * Example:
   * @code
   * {
   *   typedef std::set< Ptr<NfpWorkQueueEntry> >, NfpWorkQueueEntry::islessPtrWorkQueueEntry > WorkQueueType;
   * }
   * @endcode
   */
  struct islessPtrWorkQueueEntry
  {
    bool
    operator() (const Ptr<NfpWorkQueueEntry> a,
                const Ptr<NfpWorkQueueEntry> b) const
    {
      return *a < *b;
    }
  };

protected:
  Ptr<const CCNxName> m_anchorName;
  Ptr<const CCNxName> m_prefix;
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNX_MODEL_ROUTING_NFP_NFP_WORKQUEUE_ENTRY_H_ */
