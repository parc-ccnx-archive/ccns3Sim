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

#include <iostream>

#include "ns3/assert.h"
#include "ns3/log.h"
#include "nfp-prefix.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("NfpPrefix");

NfpPrefix::NfpPrefix (Ptr<const CCNxName> prefix, Time advertisementTimeout, PrefixStateChangeCallback callback)
  : m_prefix (prefix), m_advertisementTimeout (advertisementTimeout), m_stateChangeCallback (callback)
{
  NS_LOG_FUNCTION (this << prefix << advertisementTimeout);
}

NfpPrefix::~NfpPrefix ()
{
  // empty
}

Ptr<const CCNxName>
NfpPrefix::GetPrefix (void) const
{
  return m_prefix;
}

NfpPrefix::AdvertisementResult
NfpPrefix::ReceiveAdvertisement (Ptr<NfpAdvertise> advertisement, Ptr<CCNxConnection> ingressConnection, Time now)
{
  NS_LOG_FUNCTION (this << advertisement << ingressConnection->GetConnectionId ());
  NS_ASSERT_MSG (m_prefix->Equals (*advertisement->GetPrefix ()), "Advertisement prefix does not match");

  NfpPrefix::AdvertisementResult result = Advertisement_NotFeasible;

  bool notify = false;
  Ptr<const CCNxName> anchorName = advertisement->GetAnchorName ();

  // table event
  m_computationCost.IncrementEvents();
  AnchorMapType::iterator i = m_anchors.find (anchorName);
  if (i == m_anchors.end ())
    {
      // create a new row
      Time expiryTime = now + m_advertisementTimeout;
      Ptr<NfpAnchorAdvertisement> aa = Create<NfpAnchorAdvertisement> (advertisement, ingressConnection, expiryTime);

      m_computationCost.IncrementEvents();
      m_anchors[anchorName] = aa;
      result = Advertisement_ImprovedRoute;
      notify = true;
    }
  else
    {
      Ptr<NfpAnchorAdvertisement> aa = i->second;
      if (aa->IsFeasibleAdvertisement (advertisement))
        {
	  m_computationCost.IncrementEvents();
          Time expiryTime = now + m_advertisementTimeout;
          bool wasReachable = aa->GetNexthopCount () > 0;
          NfpAnchorAdvertisement::CompareResult compareResult = aa->UpdateAdvertisement (advertisement, ingressConnection, expiryTime);
          bool isReachable = aa->GetNexthopCount () > 0;

          switch (compareResult) {
            case NfpAnchorAdvertisement::REPLACE:
              result = Advertisement_ImprovedRoute;
              break;
            case NfpAnchorAdvertisement::EQUAL_COST:
              result = Advertisement_EqualRoute;
              break;
            default:
              break;
          }
          notify = wasReachable != isReachable;
        }
    }

  if (notify)
    {
      NS_LOG_DEBUG ("Prefix " << *advertisement->GetPrefix () << " anchor " << *anchorName << " now reachable");
      m_stateChangeCallback (m_prefix, anchorName);
    }

  NS_LOG_DEBUG ("ReceiveAdvertisement(" << *advertisement << ", " << ingressConnection->GetConnectionId () << ") result = " << result);
  return result;
}

bool
NfpPrefix::ReceiveWithdraw (Ptr<NfpWithdraw> withdraw, Ptr<CCNxConnection> ingressConnection, Time now)
{
  NS_LOG_FUNCTION (this << withdraw << ingressConnection->GetConnectionId ());
  NS_ASSERT_MSG (m_prefix->Equals (*withdraw->GetPrefix ()), "Withdraw prefix does not match");

  bool result = false;
  Ptr<const CCNxName> anchorName = withdraw->GetAnchorName ();
  m_computationCost.IncrementEvents();
  AnchorMapType::iterator i = m_anchors.find (anchorName);
  if (i != m_anchors.end ())
    {
      Ptr<NfpAnchorAdvertisement> aa = i->second;
      bool wasReachable = aa->GetNexthopCount () > 0;
      result = aa->RemoveNexthop (ingressConnection);
      if (wasReachable && aa->GetNexthopCount () == 0)
        {
          NS_LOG_DEBUG ("Prefix " << *m_prefix << " anchor " << *anchorName << " no longer reachable");
          m_stateChangeCallback (m_prefix, anchorName);
        }
    }

  NS_LOG_DEBUG ("ReceiveWithdraw(" << *withdraw << ", " << ingressConnection->GetConnectionId () << ") result = " << result);
  return result;
}

void
NfpPrefix::RemoveConnection (Ptr<CCNxConnection> connection)
{
  NS_LOG_FUNCTION (this << connection->GetConnectionId ());

  for (AnchorMapType::iterator i = m_anchors.begin (); i != m_anchors.end (); ++i)
    {
      m_computationCost.IncrementLoopIterations();
      Ptr<NfpAnchorAdvertisement> aa = i->second;
      bool wasReachable = aa->GetNexthopCount () > 0;
      aa->RemoveNexthop (connection);
      bool isReachable = aa->GetNexthopCount () > 0;
      if (wasReachable != isReachable)
        {
          m_stateChangeCallback (m_prefix, aa->GetAnchorName ());
        }
    }
}


bool
NfpPrefix::IsReachable (void)
{
  bool result = false;
  for (AnchorMapType::const_iterator i = m_anchors.cbegin (); i != m_anchors.cend (); ++i)
    {
      m_computationCost.IncrementLoopIterations();
      if (i->second->GetNexthopCount () > 0)
        {
          result = true;
          break;
        }
    }
  return result;
}

bool
NfpPrefix::IsReachable (Ptr<const CCNxName> anchorName)
{
  bool result = false;
  m_computationCost.IncrementEvents();
  AnchorMapType::const_iterator i = m_anchors.find (anchorName);
  if (i != m_anchors.cend ())
    {
      Ptr<NfpAnchorAdvertisement> aa = i->second;
      if (aa->GetNexthopCount () > 0)
        {
          result = true;
        }
    }
  return result;
}

Ptr<CCNxRoute>
NfpPrefix::GetRoute (void)
{
  // TODO: cache this
  Ptr<CCNxRoute> route = Create<CCNxRoute> (m_prefix);
  for (AnchorMapType::const_iterator i = m_anchors.begin (); i != m_anchors.end (); ++i)
    {
      m_computationCost.IncrementLoopIterations();
      route->AddRoute (i->second->GetRouteNoLinkCosts ());
    }
  return route;
}

Ptr<const NfpAdvertise>
NfpPrefix::GetAdvertisement (Ptr<const CCNxName> anchorName) const
{
  AnchorMapType::const_iterator i = m_anchors.find (anchorName);
  NS_ASSERT_MSG ((i != m_anchors.cend ()), "Could not find anchor " << *anchorName);

  return i->second->GetBestAdvertisement ();
}

std::ostream &
ns3::ccnx::operator << (std::ostream &os, const NfpPrefix &prefix)
{
  os << "{ NfpPrefix " << *prefix.m_prefix << " entries = {" << std::endl;
  for (NfpPrefix::AnchorMapType::const_iterator i = prefix.m_anchors.cbegin (); i != prefix.m_anchors.cend (); ++i)
    {
      os << *(i->second) << std::endl;
    }
  os << "} }";
  return os;
}

NfpComputationCost
NfpPrefix::GetComputationCost() const
{
  NfpComputationCost total(m_computationCost);
  for (NfpPrefix::AnchorMapType::const_iterator i = m_anchors.cbegin (); i != m_anchors.cend (); ++i) {
      total += i->second->GetComputationCost();
  }
  return total;
}
