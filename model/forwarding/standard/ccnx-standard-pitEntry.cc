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

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/object.h"
#include "ccnx-standard-pitEntry.h"
#include <ns3/ccnx-keyid.h>

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxStandardPitEntry");

CCNxStandardPitEntry::CCNxStandardPitEntry () : m_expiryTime (Seconds(-1))
{
}

CCNxStandardPitEntry::~CCNxStandardPitEntry ()
{
}

size_t
CCNxStandardPitEntry::size() const
{
  size_t mysize = 0;
  if (!isExpired()) {
      mysize = m_reverseRoutes.size();
  }
  return mysize;
}

bool
CCNxStandardPitEntry::isExpired() const
{
  bool isExpired = false;
  if (m_expiryTime <= Simulator::Now()) {
      isExpired = true;
  }
  return isExpired;
}

CCNxPit::Verdict
CCNxStandardPitEntry::ReceiveInterest (Ptr<CCNxInterest> interest, Ptr<CCNxConnection> ingress, Time expiryTime)
{
  if (isExpired()) {
      NS_LOG_DEBUG("Pit entry is expired, clearing reverse routes");

      // make sure the reverse routes are cleaned up
      m_reverseRoutes.clear();
      m_expiryTime = Seconds(-1);
  }

  /*
   * If the new expiry time is beyond our current expiry time, extend it.  Never shrink
   * the expiry time.
   */

  if (m_expiryTime < expiryTime) {
      m_expiryTime = expiryTime;
      NS_LOG_DEBUG("Extending PitEntry expiry time to " << expiryTime);
 }

  // Implement the Interest Aggregation strategy.  The default behavior is to forward.
  enum CCNxPit::Verdict verdict = CCNxPit::Forward;

  /*
   * If the reverse route set is empty, forward the interest.
   */
  if (m_reverseRoutes.size() == 0) {
      m_reverseRoutes.insert(ingress);
      NS_LOG_DEBUG("Forward first from reverse route ConnId " << ingress->GetConnectionId());
  } else {
      /*
       * If the reverse route is already in the set, then it is a re-transmission and
       * we forward it.
       */
      if (m_reverseRoutes.find(ingress) == m_reverseRoutes.end()) {
	  /*
	   * The reverse route is not in the set.  Add it and mark it as Aggregated.
	   */

	  m_reverseRoutes.insert(ingress);
	  verdict = CCNxPit::Aggregate;

	  NS_LOG_DEBUG("Aggregate from reverse route ConnId " << ingress->GetConnectionId());
      } else {
	  /*
	   * Retransmission, forward
	   */
	  NS_LOG_DEBUG("Forward retransmission from reverse route ConnId " << ingress->GetConnectionId());
      }
  }

  return verdict;
}


CCNxStandardPitEntry::ReverseRouteType
CCNxStandardPitEntry::SatisfyInterest (Ptr<CCNxConnection> ingress)
{
  ReverseRouteType reverseRoutes;

  if (!isExpired()) {

      /*
       * Copy all the entries from m_reverseRoutes to the return variable
       */
      reverseRoutes.insert(m_reverseRoutes.begin(), m_reverseRoutes.end());

      /*
       * Remove the ingress
       */
      reverseRoutes.erase(ingress);

      /*
       * Clear m_reverseRoutes
       */
      m_reverseRoutes.clear();


  } else {
      NS_LOG_DEBUG("SatisfyInterest failed, PitEntry is expired");
  }

  NS_LOG_DEBUG("SatisfyIntest returning " << reverseRoutes.size() << " connections, new size = " << size());
  return reverseRoutes;
}

std::ostream & CCNxStandardPitEntry::PrintPitEntry (std::ostream & os)

{
  os << "PitEntry { ExpiryTime : " << m_expiryTime;
  if (isExpired()) {
      os << " EXPIRED";
  }

  os << " ReverseRouteSize : " << size();

  if (!isExpired()) {
      os << " ReverseRoutes: ";
      for (ReverseRouteType::const_iterator i = m_reverseRoutes.begin(); i != m_reverseRoutes.end(); ++i) {
	  os << (*i)->GetConnectionId() << ", ";
      }
  }

  os << " }" << std::endl;

  return os;
}
