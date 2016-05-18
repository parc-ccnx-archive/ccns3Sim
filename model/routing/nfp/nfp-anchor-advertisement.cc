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
#include "ns3/simulator.h"
#include "nfp-anchor-advertisement.h"
#include "ns3/nfp-seqnum.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("NfpAnchorAdvertisement");

NfpAnchorAdvertisement::NfpAnchorAdvertisement (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection, Time expiryTime)
{
  NS_LOG_FUNCTION (this << advertise << ingressConnection->GetConnectionId ());

  m_anchorName = advertise->GetAnchorName ();
  m_prefix = advertise->GetPrefix ();
  m_equalCostPaths = Create<CCNxRoute> (advertise->GetPrefix ());
  Replace (advertise, ingressConnection, expiryTime);
  UpdateLastChangeTime ();
  LogState ();
}

NfpAnchorAdvertisement::~NfpAnchorAdvertisement ()
{
  // empty
}


NfpAnchorAdvertisement::CompareResult
NfpAnchorAdvertisement::CompareSeqnumAndDistance (Ptr<NfpAdvertise> advertise) const
{
  CompareResult result = IGNORE;

  int compare = NfpSeqnum::Compare (m_bestAdvertisement->GetAnchorSeqnum (), advertise->GetAnchorSeqnum ());
  if (compare < 0)
    {
      // our seqnum is less than advertisements, unconditionally acceptable
      result = REPLACE;
    }
  else if (compare == 0)
    {
      if (m_bestAdvertisement->GetDistance () > advertise->GetDistance ())
        {
          result = REPLACE;
        }
      else if (m_bestAdvertisement->GetDistance () == advertise->GetDistance ())
        {
          result = EQUAL_COST;
        }
    }

  return result;
}

bool
NfpAnchorAdvertisement::IsFeasibleAdvertisement (Ptr<NfpAdvertise> other) const
{
  NS_LOG_FUNCTION (this << other);

  bool isFeasible = false;

  if (m_anchorName->Equals (*other->GetAnchorName ()))
    {
      if (m_prefix->Equals (*other->GetPrefix ()))
        {
          CompareResult comparison = CompareSeqnumAndDistance (other);
          if (comparison != IGNORE)
            {
              isFeasible = true;
            }
        }
    }

  return isFeasible;
}


NfpAnchorAdvertisement::CompareResult
NfpAnchorAdvertisement::UpdateAdvertisement (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection, Time expiryTime)
{
  NS_LOG_FUNCTION (this << advertise << ingressConnection->GetConnectionId ());

  NfpAnchorAdvertisement::CompareResult result = IGNORE;
  if (IsFeasibleAdvertisement (advertise))
    {
      result = CompareSeqnumAndDistance (advertise);
      switch (result)
        {
        case REPLACE:
          Replace (advertise, ingressConnection, expiryTime);
          break;

        case EQUAL_COST:
          AddEqualCostPath (ingressConnection, expiryTime);
          break;

        default:
          NS_ASSERT_MSG (false, "comparison resulted in unexpected value: " << result);
        }
    }
  else
    {
      NS_LOG_ERROR ("Advertisement " << *advertise << " is not feasible.");
    }
  return result;
}

void
NfpAnchorAdvertisement::Replace (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection, Time expiryTime)
{
  NS_LOG_FUNCTION (this << advertise << ingressConnection->GetConnectionId ());
  m_bestAdvertisement = advertise;
  ClearEqualCostPaths ();

  m_equalCostPaths->AddNexthop (Create<CCNxRouteEntry> (advertise->GetPrefix (), advertise->GetDistance (), ingressConnection));

  m_expiryTimes[ingressConnection] = expiryTime;
  UpdateLastChangeTime ();
  LogState ();
}

void
NfpAnchorAdvertisement::ClearEqualCostPaths (void)
{
  m_equalCostPaths->clear ();
  m_expiryTimes.clear ();
}

void
NfpAnchorAdvertisement::AddEqualCostPath (Ptr<CCNxConnection> ingressConnection, Time expiryTime)
{
  NS_LOG_FUNCTION (this << ingressConnection->GetConnectionId ());
  m_equalCostPaths->AddNexthop (Create<CCNxRouteEntry> (m_bestAdvertisement->GetPrefix (), m_bestAdvertisement->GetDistance (), ingressConnection));
  m_expiryTimes[ingressConnection] = expiryTime;
  UpdateLastChangeTime ();
  LogState ();
}

Ptr<const CCNxRoute>
NfpAnchorAdvertisement::GetRouteNoLinkCosts (void)
{
  PruneEqualCostPaths (Simulator::Now ());
  return m_equalCostPaths;
}

/**
 * Return the name of the anchor whose advertisements we're tracking
 * @return
 */
Ptr<const CCNxName>
NfpAnchorAdvertisement::GetAnchorName (void) const
{
  return m_anchorName;
}

/**
 * Returns a const version of the best advertisement.  It is const just to make sure
 * that the caller does not change any state
 */
Ptr<const NfpAdvertise>
NfpAnchorAdvertisement::GetBestAdvertisement (void) const
{
  return m_bestAdvertisement;
}

void
NfpAnchorAdvertisement::LogState (void) const
{
  NS_LOG_DEBUG ("" << *this);
}


bool
NfpAnchorAdvertisement::RemoveNexthop (Ptr<CCNxConnection> connection)
{
  NS_LOG_FUNCTION (this << connection->GetConnectionId ());
  size_t removeCount = m_equalCostPaths->RemoveNexthop (connection);
  if (removeCount > 0)
    {
      m_expiryTimes.erase (connection);
      UpdateLastChangeTime ();
      LogState ();
    }
  return removeCount > 0;
}

void
NfpAnchorAdvertisement::PruneEqualCostPaths (Time now)
{
  NS_LOG_FUNCTION (this << now);
  ExpiryTimeMap::iterator i = m_expiryTimes.begin ();
  while (i != m_expiryTimes.end ())
    {
      m_computationCost.IncrementLoopIterations();
      if (i->second <= now)
        {
          m_equalCostPaths->RemoveNexthop (i->first);
          i = m_expiryTimes.erase (i);
          UpdateLastChangeTime ();
        }
      else
        {
          ++i;
        }
    }
  if (m_lastChangeTime == now)
    {
      LogState ();
    }
}

void
NfpAnchorAdvertisement::UpdateLastChangeTime (void)
{
  m_lastChangeTime = Simulator::Now ();
}

Time
NfpAnchorAdvertisement::GetLastChangeTime (void) const
{
  return m_lastChangeTime;
}


size_t
NfpAnchorAdvertisement::GetNexthopCount (void)
{
  PruneEqualCostPaths (Simulator::Now ());
  return m_equalCostPaths->size ();
}

std::ostream &
ns3::ccnx::operator << (std::ostream &os, const NfpAnchorAdvertisement &anchorAdvertise)
{
  os << "{ AnchorAdv : " << *anchorAdvertise.GetBestAdvertisement () << ", connIds: { ";

  for (CCNxRoute::const_iterator i = anchorAdvertise.m_equalCostPaths->cbegin (); i != anchorAdvertise.m_equalCostPaths->cend (); ++i)
    {
      os << i->GetConnection ()->GetConnectionId () << ", ";
    }
  os << " }, changeTime " << anchorAdvertise.GetLastChangeTime () << " }";
  return os;
}

NfpComputationCost
NfpAnchorAdvertisement::GetComputationCost() const
{
  return m_computationCost;
}
