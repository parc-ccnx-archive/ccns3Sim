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

#ifndef CCNS3SIM_MODEL_ROUTING_NFP_NFP_PREFIX_TIMER_ENTRY_H_
#define CCNS3SIM_MODEL_ROUTING_NFP_NFP_PREFIX_TIMER_ENTRY_H_

#include "ns3/ccnx-name.h"
#include "ns3/simple-ref-count.h"
#include "ns3/nstime.h"

namespace ns3 {
namespace ccnx {

/**
 * Represents an entry in the NfpPrefixTimerHeap.
 */
class NfpPrefixTimerEntry : public SimpleRefCount<NfpPrefixTimerEntry>
{
public:
  NfpPrefixTimerEntry (Ptr<const CCNxName> prefix, Ptr<const CCNxName> anchorName, Time expiry);

  Ptr<const CCNxName> GetPrefix (void) const;
  Ptr<const CCNxName> GetAnchorName (void) const;
  Time GetTime (void) const;

  /**
   * Marks the entry as invalid.  The timer heap might invalidate an entry in the heap
   * so it does not need to do a search to remove it.  It will be removed when it gets to the top.
   */
  void Invalidate (void);

  /**
   * Determines if the entry is valid.
   * @return
   */
  bool IsValid (void) const;

  /**
   * Returns true if a->GetTime() < b->GetTime().  Using this in a std::map or std::priority_queue will sort
   * the entries by greatest first.
   *
   * @param a The first entry to compare
   * @param b The second entry to compare
   * @return True if a->GetTime() < b->GetTime()
   */
  struct IsTimeLess
  {
    bool operator() (const Ptr<NfpPrefixTimerEntry> a, const Ptr<NfpPrefixTimerEntry> b) const
    {
      return a->GetTime () < b->GetTime ();
    }
  };

  /**
   * Returns true if a->GetTime() > b->GetTime().  Using this in a std::map or std::priority_queue will sort
   * the entries by least first.  This is what we want to use for timers so the earliest is on top.
   *
   * @param a The first entry to compare
   * @param b The second entry to compare
   * @return True if a->GetTime() > b->GetTime()
   */
  struct IsTimeGreater
  {
    bool operator() (const Ptr<NfpPrefixTimerEntry> a, const Ptr<NfpPrefixTimerEntry> b) const
    {
      return a->GetTime () > b->GetTime ();
    }
  };

  /**
   * Return true if the pair a->(prefix, anchorName) < b->(prefix, anchorName).
   *
   * @param a The first entry to compare
   * @param b The second entry to compare
   * @return True if (a->prefix < b->prefix) or (a->prefix == b->prefix && a->anchorName < b->anchorName)
   */
  struct IsNameLess
  {
    bool operator() (const Ptr<NfpPrefixTimerEntry> a, const Ptr<NfpPrefixTimerEntry> b) const
    {
      bool result = false;
      if (*a->GetPrefix () < *b->GetPrefix ())
        {
          result = true;
        }
      else if (a->GetPrefix ()->Equals (*b->GetPrefix ()))
        {
          result = *a->GetAnchorName () < *b->GetAnchorName ();
        }
      return result;
    }
  };

private:
  Ptr<const CCNxName> m_prefix;
  Ptr<const CCNxName> m_anchorName;
  Time m_expiry;
  bool m_valid;
};


}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_ROUTING_NFP_NFP_PREFIX_TIMER_ENTRY_H_ */
