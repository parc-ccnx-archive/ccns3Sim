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
#include "ns3/nfp-seqnum.h"

#include "nfp-neighbor.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("NfpNeighbor");

NfpNeighbor::NeighborIdType NfpNeighbor::m_nextNeighborId = 1;

NfpNeighbor::NeighborIdType
NfpNeighbor::GetUniqueNeighborId (void)
{
  return m_nextNeighborId++;
}

NfpNeighbor::NfpNeighbor (Ptr<const CCNxName> name, Ptr<CCNxConnection> connection, uint16_t messageSeqnum, Time neighborTimeout,
                          Callback< void, Ptr<NfpNeighborKey> > stateChangeCallback)
  : m_neighborId (GetUniqueNeighborId ()), m_name (name), m_connection (connection)
{
  NS_LOG_FUNCTION (this << name << messageSeqnum << connection->GetConnectionId ());

  m_neighborKey = Create<NfpNeighborKey> (name, connection);
  m_messageSeqnum = messageSeqnum;
  m_neighborTimeout = neighborTimeout;
  m_stateChangeCallback = stateChangeCallback;
  m_expiry = ComputeExpiry ();
  m_state = StateUp;
  m_timer = Timer (Timer::REMOVE_ON_DESTROY);
  m_timer.SetFunction (&NfpNeighbor::TimeoutExpired, this);
  SetTimer ();

  LogState ();
}

NfpNeighbor::~NfpNeighbor ()
{
  // empty
}

const NfpNeighbor::NeighborIdType
NfpNeighbor::GetNeighborId (void) const
{
  return m_neighborId;
}

bool
NfpNeighbor::IsHelloInOrder (uint16_t messageSeqnum) const
{
  int result = NfpSeqnum::Compare (m_messageSeqnum, messageSeqnum);
  return result < 0;
}

bool
NfpNeighbor::ReceiveHello (uint16_t messageSeqnum)
{
  NS_LOG_FUNCTION (this << messageSeqnum);

  bool result = false;
  NeighborState oldState = m_state;
  if (IsHelloInOrder (messageSeqnum))
    {
      m_messageSeqnum = messageSeqnum;
      m_expiry = ComputeExpiry ();
      m_state = StateUp;
      LogState ();

      result = true;
    }
  else
    {
      NS_LOG_DEBUG ("Received out-of-order messageSeqnum " << messageSeqnum << " from connId " << m_connection->GetConnectionId ());
    }

  if (oldState != m_state)
    {
      m_stateChangeCallback (m_neighborKey);
    }
  return result;
}

bool
NfpNeighbor::IsExpired (void) const
{
  return m_expiry <= Simulator::Now ();
}

Ptr<const CCNxName>
NfpNeighbor::GetName (void) const
{
  return m_name;
}

Time
NfpNeighbor::GetExpiryTime (void) const
{
  return m_expiry;
}

Ptr<CCNxConnection>
NfpNeighbor::GetConnection (void) const
{
  return m_connection;
}

uint16_t
NfpNeighbor::GetMessageSeqnum (void) const
{
  return m_messageSeqnum;
}

Ptr<NfpNeighborKey>
NfpNeighbor::GetNeighborKey (void) const
{
  return m_neighborKey;
}

Time
NfpNeighbor::ComputeExpiry (void) const
{
  return Simulator::Now () + m_neighborTimeout;
}

bool
NfpNeighbor::IsStateUp (void) const
{
  return m_state == StateUp;
}

bool
NfpNeighbor::IsStateDown (void) const
{
  return m_state == StateDown;
}

bool
NfpNeighbor::IsStateDead (void) const
{
  return m_state == StateDead;
}

void
NfpNeighbor::TimeoutExpired (void)
{
  m_computationCost.IncrementEvents();
  switch (m_state)
    {
    case StateUp:
      UpStateTimeout ();
      break;
    case StateDown:
      DownStateTimeout ();
      break;
    case StateDead:
    // fallthrough.  We shold not receive timeouts in the Dead state
    default:
      NS_ASSERT_MSG (false,"Received a timeout in unknown state: " << m_state);
      break;
    }
}

void
NfpNeighbor::UpStateTimeout (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (IsExpired ())
    {
      NS_LOG_INFO ("UP neighbor expired: " << *m_name);
      m_state = StateDown;

      // Update the expiry time as we are now counting down until Dead
      // Do this before calling the callback so we appear as IsStateDown() and not IsExpired().
      m_expiry = ComputeExpiry ();
      LogState ();
      m_stateChangeCallback (m_neighborKey);
    }
  else
    {
      // Reset the timer.  We are still in IsStateUp() and not IsExpired().
    }

  // Set the next timer (either our Down timer or the next Up timer)
  SetTimer ();
}

void
NfpNeighbor::DownStateTimeout (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (IsExpired ())
    {
      NS_LOG_INFO ("DOWN neighbor expired: " << *m_name);
      m_state = StateDead;
      LogState ();

      // Do not update the expiry time and do not start a new timer.  The callback
      // should delete this object from the neighbor map.
      // We are now in IsStateDead() and IsExpired()
      m_stateChangeCallback (m_neighborKey);
    }
  else
    {
      // reset the timer.  We are still in IsStateDown() and not IsExpired().
      SetTimer ();
    }
}

void
NfpNeighbor::SetTimer (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT_MSG (m_expiry >= Simulator::Now (), "Trying to schedule a neighbor timer in the past");

  Time delta = m_expiry - Simulator::Now ();

  m_timer.SetDelay (delta);
  m_timer.Schedule ();

  NS_LOG_DEBUG ("Set timer " << &m_timer << " delay " << delta);
}

void
NfpNeighbor::LogState (void) const
{
  NS_LOG_DEBUG (*this);
}

std::ostream &
ns3::ccnx::operator << (std::ostream &os, const NfpNeighbor &neighbor)
{
  static const std::string state[] = { "Up", "Down", "Dead" };

  os << "{ Neighbor : " << *neighbor.GetName () << ", " << neighbor.GetMessageSeqnum () << ", "
     << neighbor.GetConnection ()->GetConnectionId () << ", " << state[neighbor.m_state] << ", " << neighbor.GetExpiryTime () << " }";
  return os;
}

NfpComputationCost
NfpNeighbor::GetComputationCost() const
{
  return m_computationCost;
}
