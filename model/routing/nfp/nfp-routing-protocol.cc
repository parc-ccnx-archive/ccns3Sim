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

//#include <numeric_limits>

#include <iostream>
#include <iomanip>

#include "ns3/assert.h"
#include "ns3/log.h"
#include "nfp-routing-protocol.h"
#include "ns3/nstime.h"

#include "ns3/nfp-advertise.h"
#include "ns3/nfp-withdraw.h"
#include "ns3/nfp-payload.h"
#include "ns3/buffer.h"
#include "ns3/ccnx-name-builder.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("NfpRoutingProtocol");
NS_OBJECT_ENSURE_REGISTERED (NfpRoutingProtocol);

TypeId
NfpRoutingProtocol::GetTypeId ()
{
  // TODO: Put all the default values in defined constants
  static TypeId tid = TypeId ("ns3::ccnx::NfpRoutingProtocol")
    .SetParent<CCNxRoutingProtocol> ()
    .SetGroupName ("CCNx")
    .AddConstructor<NfpRoutingProtocol> ()
    .AddAttribute ("HelloInterval", "HELLO messages emission interval.",
                   TimeValue (Seconds (2)),
                   MakeTimeAccessor (&NfpRoutingProtocol::m_helloInterval),
                   MakeTimeChecker ())
    .AddAttribute ("AdvertiseInterval", "Advertisement messages emission interval.",
                   TimeValue (Seconds (5)),
                   MakeTimeAccessor (&NfpRoutingProtocol::m_advertiseInterval),
                   MakeTimeChecker ())
    .AddAttribute ("Jitter", "Subtractive jitter to messages from interval.",
                   TimeValue (MilliSeconds (100)),
                   MakeTimeAccessor (&NfpRoutingProtocol::m_jitter),
                   MakeTimeChecker ())
    .AddAttribute ("RouteTimeout", "Timeout a route if not heard in this period.",
                   TimeValue (Seconds (15)),
                   MakeTimeAccessor (&NfpRoutingProtocol::m_routeTimeout),
                   MakeTimeChecker ())
    .AddAttribute ("NeighborTimeout", "Timeout a neighbor if not heard in this period.",
                   TimeValue (Seconds (6)),
                   MakeTimeAccessor (&NfpRoutingProtocol::m_neighborTimeout),
                   MakeTimeChecker ());
  return tid;
}

// ========================================
// NameFloodingProtocol specific calls

NfpRoutingProtocol::NfpRoutingProtocol ()
{
  m_anchorSeqnum = 0;
  m_messageSeqnum = 1;
  m_helloTimer = Timer (Timer::REMOVE_ON_DESTROY);
  m_advertiseTimer = Timer (Timer::REMOVE_ON_DESTROY);
  m_anchorRouteTimer = Timer (Timer::REMOVE_ON_DESTROY);
  m_processWorkQueueTimer  = Timer (Timer::REMOVE_ON_DESTROY);

  m_uniformRandomVariable = CreateObject<UniformRandomVariable> ();
  m_nfpPrefix = Create<CCNxName> ("ccnx:/name=nfp");
  m_minimumMtu = GetMinimumMtu ();

  // set this way in the past
  m_lastBroadcast = Time (Seconds (-1000));

  // the specific name will be added in DoInitialize();
  m_routerName = 0;
}

NfpRoutingProtocol::~NfpRoutingProtocol ()
{
  m_uniformRandomVariable = 0;
}

void
NfpRoutingProtocol::SetRouterName (void)
{
  Buffer buffer (0);
  buffer.AddAtStart (32);

  Buffer::Iterator i = buffer.Begin ();
  i.WriteHtonU64 (m_node->GetId ());
  i.WriteU8 (0, 24);
  std::string s ((const char *) buffer.PeekData (), buffer.GetSize ());

  Ptr<CCNxNameSegment> segment = Create<CCNxNameSegment> (CCNxNameSegment_Name, s);
  CCNxNameBuilder builder;
  builder.Append (segment);
  m_routerName = builder.CreateName ();
  NS_LOG_INFO ("Node " << m_node->GetId () << " nfp router name: " << *m_routerName);
}

void
NfpRoutingProtocol::DoInitialize (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_portal = CCNxPortal::CreatePortal (m_node, TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory"));
  NS_ASSERT_MSG (m_portal, "Failed to create CCNxMessagePortal on node " << m_node);
  m_portal->SetRecvCallback (MakeCallback (&NfpRoutingProtocol::ReceiveNotify, this));
  m_portal->RegisterPrefix (m_nfpPrefix);

  m_helloTimer.SetFunction (&NfpRoutingProtocol::HelloTimerExpired, this);
  m_advertiseTimer.SetFunction (&NfpRoutingProtocol::AdvertiseTimerExpired, this);
  m_anchorRouteTimer.SetFunction (&NfpRoutingProtocol::AnchorRouteTimerExpired, this);
  m_processWorkQueueTimer.SetFunction (&NfpRoutingProtocol::ProcessWorkQueueTimerExpired, this);

  // Set the hello timer to start in 1 jitter interval with 25% jitter
  SetTimer (m_helloTimer, m_jitter, m_jitter / 4);

  // Set the advertise timer to start in 1 jitter interval with 25% jitter
  SetTimer (m_advertiseTimer, m_jitter, m_jitter / 4);

  // Set the anchor route timer to start in 1 jitter interval with 25% jitter
  SetTimer (m_anchorRouteTimer, m_jitter, m_jitter / 4);
}

/**
 * Called when the protocol should stop running
 */
void
NfpRoutingProtocol::DoDispose (void)
{
  // Kill hello timer
  // kill advertisement timer
}

// ========================================

/**
 * This node is an anchor for the given prefix.
 *
 * Called from the configuration script.
 */
void
NfpRoutingProtocol::AddAnchorPrefix (Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << prefix);

  m_computationCost.IncrementEvents();
  AnchorNameMapType::iterator i = m_anchorNames.find (prefix);
  if (i != m_anchorNames.end ())
    {
      NS_ASSERT_MSG (i->second > 0, "Found a map entry with a 0 ref count");
      i->second++;
    }
  else
    {
      NS_LOG_WARN("Add anchor prefix " << *prefix);
      m_anchorNames[prefix] = 1;
      InjectAnchorRoute (prefix, m_anchorSeqnum++);
    }
}

void
NfpRoutingProtocol::RemoveAnchorPrefix (Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << prefix);

  m_computationCost.IncrementEvents();
  AnchorNameMapType::iterator i = m_anchorNames.find (prefix);
  if (i != m_anchorNames.end ())
    {
      NS_ASSERT_MSG (i->second > 0, "Found a map entry with a 0 ref count");
      i->second--;
      if (i->second == 0)
        {
          NS_LOG_INFO ("Removing Anchor name " << *(i->first));
          InjectAnchorWithdraw (prefix);
          m_anchorNames.erase (i);
        }
      else
        {
          NS_LOG_INFO ("Decrement count Anchor name " << *(i->first) << " to " << i->second);
        }
    }
  else
    {
      NS_LOG_WARN ("Count not find Anchor name " << *(i->first));
    }
}

NfpRoutingProtocol::AnchorNameMapType::const_iterator
NfpRoutingProtocol::GetAnchorPrefixIterator (void) const
{
  return m_anchorNames.cbegin ();
}


Time
NfpRoutingProtocol::GetHelloInterval (void) const
{
  return m_helloInterval;
}

Time
NfpRoutingProtocol::GetAdvertiseInterval (void) const
{
  return m_advertiseInterval;
}

Time
NfpRoutingProtocol::GetJitter (void) const
{
  return m_jitter;
}

Time
NfpRoutingProtocol::GetRouteTimeout (void) const
{
  return m_routeTimeout;
}

Time
NfpRoutingProtocol::GetNeighborTimeout (void) const
{
  return m_neighborTimeout;
}

Time
NfpRoutingProtocol::CalculateJitteredTime (Time interval, Time jitter)
{
  NS_ASSERT_MSG(interval > 0, "Interval must be positive");
  NS_ASSERT_MSG(jitter >= 0, "Jitter must be non-negative");

  Time offset (0);

  if (jitter.IsPositive ())
    {
      uint32_t r = m_uniformRandomVariable->GetInteger (0, jitter.GetNanoSeconds ());
      offset = Time (NanoSeconds (r));
    }

  Time t = interval - offset;
  NS_ASSERT_MSG(t > 0, "Negative time!  interval = " << interval << " jitter = " << jitter << " offset = " << offset);
  return t;
}

void
NfpRoutingProtocol::ProcessWorkQueueTimerExpired()
{
  m_computationCost.IncrementEvents();
  ProcessWorkQueue();
}

void
NfpRoutingProtocol::SetProcessWorkQueueTimer()
{
  static Time interval = MilliSeconds(10);
  static Time jitter = MilliSeconds(5);

  if (!m_processWorkQueueTimer.IsRunning()) {
      SetTimer(m_processWorkQueueTimer, interval, jitter);
  }
}

void
NfpRoutingProtocol::SetTimer (Timer &timer, Time interval, Time jitter)
{
  Time final = CalculateJitteredTime (interval, jitter);
  timer.SetDelay (final);
  timer.Schedule ();

  NS_LOG_DEBUG ("Set timer " << &timer << " delay " << final << " interval " << interval << " jitter " << jitter);
}

void
NfpRoutingProtocol::HelloTimerExpired ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_computationCost.IncrementEvents();

  /*
   * If we are within m_jitter of the hello interval from the last time we broadcast, then send a Hello packet.
   *
   * Explaining the math example:
   *    m_helloInterval = 5000 msec
   *    m_jitter = 10 msec
   *    m_lastBroadcast = 10000 msec
   *
   *    This means our next hello message should be sent anywhere in the interval [14990, 15000]
   *
   *    m_lastBroadcast + m_helloInterval - m_jitter = 14990
   *    So if GetCurrentTime() is at least 14990 we will send the hello.
   */
  if (m_lastBroadcast + m_helloInterval - m_jitter <= GetCurrentTime ())
    {
      m_stats.IncrementHellosSent();

      Ptr<NfpPayload> payload = CreatePayload ();
      Ptr<CCNxPacket> packet = CreatePacket (payload);

      NS_LOG_DEBUG ("HelloTimerExpired sending payload " << *payload << ", serialized = " << payload->GetSerializedSize ());
      // that's all we need to do, as a Hello message doesn't advertise anything

      // this will update m_lastBroadcast
      Broadcast (packet);

      NS_LOG_DEBUG ("HelloTimerExpired resetting timer delta = " << m_helloInterval);
      SetTimer (m_helloTimer, m_helloInterval, m_jitter);
    }
  else
    {
      /*
       * set timer for the remaining wait time.
       * this is guaranteed to be positive due to if() condition
       * we know m_lastBroadcast + m_helloInterval - m_jitter > GetCurrentTime(), so
       * m_lastBroadcast + m_helloInterval must by itself also be greater than GetCurrentTime().
       */
      Time delta = m_lastBroadcast + m_helloInterval - GetCurrentTime ();
      NS_LOG_DEBUG ("HelloTimerExpired resetting timer delta = " << delta);

      SetTimer (m_helloTimer, delta, m_jitter);
    }
}

void
NfpRoutingProtocol::InjectAnchorRoute (Ptr<const CCNxName> prefix, uint32_t anchorSeqnum)
{
  m_stats.IncrementAdvertiseOriginated();

  // because we are the anchor, the distance is always 0.
  uint16_t distance = 0;
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (m_routerName, prefix, anchorSeqnum, distance);

  NS_LOG_INFO("Inject anchor route " << *advertise);

  /*
   * We set the connection id to LocalHost.  This means that when it comes time to manage the FIB
   * table in the CCNxForwarder, we will skip these as they should already be in the FIB based on
   * RegisterPrefix().
   */
  //TODO - ask Marc if standard forwarder should skip these routes too?

  Ptr<CCNxConnectionL4> connection = Create<CCNxConnectionL4> ();
  connection->SetConnectionLocalHost ();
  ReceiveAdvertise (advertise, connection);
}

void
NfpRoutingProtocol::InjectAnchorWithdraw (Ptr<const CCNxName> prefix)
{
  m_stats.IncrementWithdrawOriginated();
  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> (m_routerName, prefix);
  Ptr<CCNxConnectionL4> connection = Create<CCNxConnectionL4> ();
  connection->SetConnectionLocalHost ();
  ReceiveWithdraw (withdraw, connection);
}

void
NfpRoutingProtocol::AnchorRouteTimerExpired ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_computationCost.IncrementEvents();

  if (!m_anchorNames.empty ())
    {
      NS_LOG_DEBUG ("AnchorRouteTimerExpired Node " << m_node->GetId() << " injecting routes for our anchor names (timeout = " << m_routeTimeout << ")");

      uint32_t anchorSeqnum = m_anchorSeqnum++;
      // Create an advertise for each anchor name and inject it into our RIB.
      for (AnchorNameMapType::const_iterator i = m_anchorNames.cbegin (); i != m_anchorNames.cend (); ++i)
        {
          InjectAnchorRoute (i->first, anchorSeqnum);
        }
     }

  SetTimer (m_anchorRouteTimer, m_routeTimeout, m_jitter);
}

void
NfpRoutingProtocol::AdvertiseTimerExpired ()
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_LOG_DEBUG ("AdvertiseTimerExpired, processing work queue");

  m_computationCost.IncrementEvents();

  /*
   * Pop stuff off the timer heap to advertise anything whose timer has expired.
   * We add the expired entries to the work queue, then process the work queue again.
   */

  Ptr<const NfpPrefixTimerEntry> entry;
  while ( (entry = m_prefixTimerHeap.Peek ()))
    {
      m_computationCost.IncrementLoopIterations();

      // Include anything off the heap out to now + jitter time  This avoids
      // setting very short timers
      if (entry->GetTime () <= GetCurrentTime () + m_jitter)
        {
          // process entry
          AddWorkQueueEntry (entry->GetAnchorName (), entry->GetPrefix ());
          m_prefixTimerHeap.Pop ();
        }
      else
        {
          break;
        }
    }

  // Now look at the top element in the heap to set our next expiry timer
  if ( (entry = m_prefixTimerHeap.Peek ()) )
    {
      // If delta is greater the m_routeTimeout, use m_routeTimeout.
      Time delta = entry->GetTime () - GetCurrentTime ();
      NS_ASSERT_MSG(delta > 0, "The top of heap is already expired");

      delta = std::min (delta, m_routeTimeout);

      // If delta is less than m_jitter, use delta as the maximum jitter.
      Time jitter = std::min (delta, m_jitter);
      SetTimer (m_advertiseTimer, delta, jitter);
    }
  else
    {
      // nothing in the heap, set to the maximum interval
      SetTimer (m_advertiseTimer, m_routeTimeout, m_jitter);
    }
}

uint16_t
NfpRoutingProtocol::GetNextMessageSeqnum (void)
{
  return m_messageSeqnum++;
}

Ptr<NfpPayload>
NfpRoutingProtocol::CreatePayload (void)
{
  // FIXME: need to subtract the FixedHeader and Interest overhead.  The "50" is just a hack.
  size_t packetOverhead = 50;
  Ptr<NfpPayload> payload = Create<NfpPayload> (m_minimumMtu - packetOverhead, m_routerName, GetNextMessageSeqnum ());

  NS_LOG_DEBUG("Create payload " << *payload);

  return payload;
}

Ptr<CCNxPacket>
NfpRoutingProtocol::CreatePacket (Ptr<NfpPayload> payload)
{
  Ptr<CCNxBuffer> buffer = Create<CCNxBuffer> (0);
  buffer->AddAtStart (payload->GetSerializedSize ());
  payload->Serialize (buffer->Begin ());

  CCNxNameBuilder b(*m_nfpPrefix);
  b.Append(m_routerName->GetSegment(0));

  Ptr<CCNxInterest> interest = Create<CCNxInterest> (b.CreateName(), buffer);
  Ptr<CCNxPacket> packet = CCNxPacket::CreateFromMessage (interest);
  packet->GetFixedHeader ()->SetHopLimit (1);

  return packet;
}

void
NfpRoutingProtocol::ReceiveNotify (Ptr<CCNxPortal> portal)
{
  NS_LOG_FUNCTION (this << portal);

  Ptr<CCNxPacket> packet;
  Ptr<CCNxConnection> ingressConnection;
  while ( (packet = portal->RecvFrom (ingressConnection)) )
    {
      // don't count this loop, it's counted as receiving a message

      NS_ASSERT_MSG (ingressConnection, "Received portal message with NULL ingress connection");
      NS_LOG_DEBUG ("ReceiveNotify packet " << packet << " ingress connId " << ingressConnection->GetConnectionId ());

      Ptr<CCNxMessage> ccnxMessage = packet->GetMessage ();
      NS_ASSERT_MSG (ccnxMessage, "Got null message from packet");

      Ptr<CCNxBuffer> interestPayload = ccnxMessage->GetPayload ();
      NS_ASSERT_MSG (interestPayload, "Got null payload from ccnx Message");

      Ptr<NfpPayload> nfpPayload = Create<NfpPayload> ();
      NS_ASSERT_MSG (nfpPayload, "Failed to create empty nfpPayload");

      nfpPayload->Deserialize (interestPayload->Begin ());
      ReceivePayload (nfpPayload, ingressConnection);
    }
}

void
NfpRoutingProtocol::ReceivePayload (Ptr<NfpPayload> payload, Ptr<CCNxConnection> ingressConnection)
{
  NS_LOG_FUNCTION (this << payload << ingressConnection->GetConnectionId ());

  NS_LOG_DEBUG ("ReceivePayload from " << *payload->GetRouterName () << ", msgSeqnum " << payload->GetMessageSeqnum ());

  if (!m_routerName->Equals (*payload->GetRouterName ()))
    {
      m_stats.IncrementPayloadsReceived();
      m_stats.IncrementBytesReceived (payload->GetSerializedSize ());

      // Neighbor management
      bool accept =  ReceiveHello(payload->GetRouterName (), payload->GetMessageSeqnum (), ingressConnection);

      // If the message is out-of-sequence from prior messages, ignore it
      if (accept)
        {
          // The message seqnum was in-order, so we can accept the payload
          for (size_t i = 0; i < payload->GetMessageCount (); i++)
            {
              // don't incremnet counter for this loop, its counted as receiving a message
              Ptr<NfpMessage> message = payload->GetMessage (i);
              if (message->IsAdvertise ())
                {
                  Ptr<NfpAdvertise> advertise = DynamicCast<NfpAdvertise, NfpMessage> (message);
                  if ( !m_routerName->Equals (*advertise->GetAnchorName ()))
                    {
                      ReceiveAdvertise (advertise, ingressConnection);
                    }
                  else
                    {
                      NS_LOG_DEBUG ("ReceivePayload anchor name is us, ignore advertisement");
                    }
                }
              else if (message->IsWithdraw ())
                {
                  Ptr<NfpWithdraw> withdraw = DynamicCast<NfpWithdraw, NfpMessage> (message);
        	  if ( !m_routerName->Equals (*withdraw->GetAnchorName ()))
        	    {
        	      ReceiveWithdraw (withdraw, ingressConnection);
        	    } else {
                        NS_LOG_DEBUG ("ReceivePayload anchor name is us, ignore withdraw");
        	    }
                }
              else
                {
                  NS_LOG_ERROR ("Unknown message type [" << i << "] : "  << message);
                }
            }
        }
      else
        {
          NS_LOG_INFO ("ReceivePayload from " << *payload->GetRouterName () << ", msgSeqnum " << payload->GetMessageSeqnum () << " out-of-order, ignoring");
        }
    }
  else
    {
      NS_LOG_WARN ("Received hello message from neighbor with our name.  Ignorning.");
    }
}

void
NfpRoutingProtocol::AddAdvertise (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection)
{
  Ptr<const CCNxName> prefixName = advertise->GetPrefix ();
  Ptr<NfpPrefix> prefixEntry = Create<NfpPrefix> (prefixName, m_routeTimeout,
                                                  MakeCallback (&NfpRoutingProtocol::PrefixStateChanged, this));

  // must do this before calling ReceiveAdvertisement because if that triggers the PrefixStateChagned callback,
  // we need to have it in our map first.
  m_computationCost.IncrementEvents();
  m_prefixes[prefixName] = prefixEntry;
  m_prefixTimerHeap.Insert (prefixName, advertise->GetAnchorName (),
                            GetCurrentTime () + CalculateJitteredTime (m_routeTimeout, m_jitter));

  AddWorkQueueEntry (advertise->GetAnchorName (), prefixName);

  prefixEntry->ReceiveAdvertisement (advertise, ingressConnection, GetCurrentTime ());
}

void
NfpRoutingProtocol::ReceiveAdvertise (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection)
{
  NS_LOG_FUNCTION (this << advertise << ingressConnection->GetConnectionId ());

  m_stats.IncrementAdvertiseReceived();
  m_computationCost.IncrementEvents();
  Ptr<const CCNxName> prefixName = advertise->GetPrefix ();
  PrefixMapType::iterator i = m_prefixes.find (prefixName);
  if (i == m_prefixes.end ())
    {
      NS_LOG_INFO ("Adding advertisement " << *advertise << " ingress " << ingressConnection->GetConnectionId());
      m_stats.IncrementAdvertiseReceivedFeasible();
      AddAdvertise (advertise, ingressConnection);
    }
  else
    {
      Ptr<NfpPrefix> prefixEntry = i->second;
      NfpPrefix::AdvertisementResult result = prefixEntry->ReceiveAdvertisement (advertise, ingressConnection, GetCurrentTime ());

      // only re-advertise it if it improved our route
      if (result == NfpPrefix::Advertisement_ImprovedRoute)
        {
	  NS_LOG_INFO("Update advertisement " << *advertise << " ingress " << ingressConnection->GetConnectionId());

	  m_stats.IncrementAdvertiseReceivedFeasible();
          AddWorkQueueEntry (advertise->GetAnchorName (), prefixName);
        }
    }
}

void
NfpRoutingProtocol::ReceiveWithdraw (Ptr<NfpWithdraw> withdraw, Ptr<CCNxConnection> ingressConnection)
{
  NS_LOG_FUNCTION (this << withdraw << ingressConnection->GetConnectionId ());

  m_stats.IncrementWithdrawReceived();

  m_computationCost.IncrementEvents();
  Ptr<const CCNxName> prefixName = withdraw->GetPrefix ();
  PrefixMapType::iterator i = m_prefixes.find (prefixName);
  if (i != m_prefixes.end ())
    {
      Ptr<NfpPrefix> prefixEntry = i->second;
      bool wasReachable = prefixEntry->IsReachable ();
      prefixEntry->ReceiveWithdraw (withdraw, ingressConnection, GetCurrentTime ());
      bool isReachable = prefixEntry->IsReachable ();

      // if we transitioned the edge from reachable -> not reachable, add it to work queue
      if (wasReachable && !isReachable)
	{

	  NS_LOG_INFO("Process withdraaw " << *withdraw << " ingress " << ingressConnection->GetConnectionId());

	  AddWorkQueueEntry (withdraw->GetAnchorName (), prefixName);
	}
    }
}

bool
NfpRoutingProtocol::ReceiveHello (Ptr<const CCNxName> neighborName, uint16_t msgSeqnum, Ptr<CCNxConnection> ingressConnection)
{
  NS_LOG_FUNCTION (this << neighborName << msgSeqnum << ingressConnection->GetConnectionId ());

  bool result = false;
  // table event
  m_computationCost.IncrementEvents();
  Ptr<NfpNeighborKey> key = Create<NfpNeighborKey> (neighborName, ingressConnection);
  NeighborMapType::iterator i = m_neighbors.find (key);
  if (i != m_neighbors.end ())
    {
      Ptr<NfpNeighbor> neighbor = i->second;
      if (neighbor->IsHelloInOrder (msgSeqnum))
        {
          // in map, and message is new
          neighbor->ReceiveHello (msgSeqnum);
          NS_LOG_DEBUG ("ReceiveHello from " << *neighborName << " ConnId " << ingressConnection->GetConnectionId () << " msgSeqnum " << msgSeqnum << " update neighbor");
          result = true;
        }
      else
        {
          // in map, but message is old
	  NS_LOG_DEBUG ("ReceiveHello from " << *neighborName << " ConnId " << ingressConnection->GetConnectionId () << " msgSeqnum " << msgSeqnum << " out of order, rejected");
        }
    }
  else
    {
      // not in map
      m_computationCost.IncrementEvents();
      Ptr<NfpNeighbor> neighbor = Create<NfpNeighbor> (neighborName, ingressConnection, msgSeqnum, m_neighborTimeout,
                                                       MakeCallback (&NfpRoutingProtocol::NeighborStateChanged, this));
      m_neighbors[key] = neighbor;
      NS_LOG_DEBUG ("ReceiveHello from " << *neighborName << " ConnId " << ingressConnection->GetConnectionId () << " msgSeqnum " << msgSeqnum << " add new neighbor");
      result = true;
    }

  return result;
}

void
NfpRoutingProtocol::NeighborStateChanged (Ptr<NfpNeighborKey> neighborKey)
{
  NS_LOG_FUNCTION (this << *neighborKey);
  NeighborMapType::iterator i = m_neighbors.find (neighborKey);
  NS_ASSERT_MSG (i != m_neighbors.end (), "Got a state change callback but cannot find neighbor in map: " << *neighborKey);

  Ptr<NfpNeighbor> neighbor = i->second;

  // nothing to do if the neighbor is up
  if (neighbor->IsStateDown ())
    {
      NS_LOG_WARN ("Removing routes for neighbor " << *neighborKey);
      RemoveNeighborRoutes (neighborKey->GetName (), neighborKey->GetConnection ());
    }
  else if (neighbor->IsStateDead ())
    {
      NS_LOG_WARN ("Removing dead neighbor " << *neighborKey);
      m_neighbors.erase (i);
    }
}

Ptr<CCNxRoute>
NfpRoutingProtocol::GetModelFibRoute (Ptr<const CCNxName> prefix)
{
  m_computationCost.IncrementEvents();
  Ptr<CCNxRoute> route;
  ModelFibType::iterator i = m_modelFib.find (prefix);
  if (i == m_modelFib.end ())
    {
      route = Create<CCNxRoute> (prefix);
      m_modelFib[prefix] = route;
    }
  else
    {
      route = i->second;
    }
  return route;
}

void
NfpRoutingProtocol::AddAnchorToFib (Ptr<NfpPrefix> prefixEntry, Ptr<const CCNxName> anchorName)
{
  NS_LOG_FUNCTION (this << *prefixEntry << *anchorName);
  NS_LOG_DEBUG ("AddAnchorToFib: prefix " << *prefixEntry->GetPrefix ());

  Ptr<CCNxL3Protocol> ccnx = m_node->GetObject<CCNxL3Protocol> ();
  Ptr<CCNxForwarder> forwarder = m_node->GetObject<CCNxForwarder> ();

  Ptr<CCNxRoute> currentRoute = prefixEntry->GetRoute ();
  Ptr<CCNxRoute> modelRoute   = GetModelFibRoute (prefixEntry->GetPrefix ());

  // For now, we simply remove everything from modelRoute then add everything from currentRoute.
  forwarder->RemoveRoute (modelRoute);
  forwarder->AddRoute (currentRoute);

  m_computationCost.IncrementEvents();
  m_modelFib[prefixEntry->GetPrefix ()] = currentRoute;
}

void
NfpRoutingProtocol::RemoveAnchorFromFib (Ptr<NfpPrefix> prefixEntry, Ptr<const CCNxName> anchorName)
{
  NS_LOG_FUNCTION (this << *prefixEntry << *anchorName);
  NS_LOG_DEBUG ("RemoveAnchorFromFib: prefix " << *prefixEntry->GetPrefix ());

  Ptr<CCNxL3Protocol> ccnx = m_node->GetObject<CCNxL3Protocol> ();
  Ptr<CCNxForwarder> forwarder = m_node->GetObject<CCNxForwarder> ();

  Ptr<CCNxRoute> currentRoute = prefixEntry->GetRoute ();
  Ptr<CCNxRoute> modelRoute   = GetModelFibRoute (prefixEntry->GetPrefix ());

  // For now, we simply remove everything from modelRoute then add everything from currentRoute.
  forwarder->RemoveRoute (modelRoute);
  forwarder->AddRoute (currentRoute);

  m_computationCost.IncrementEvents();
  m_modelFib[prefixEntry->GetPrefix ()] = currentRoute;
}

void
NfpRoutingProtocol::PrefixStateChanged (Ptr<const CCNxName> prefix, Ptr<const CCNxName> anchorName)
{
  NS_LOG_FUNCTION (this << *prefix << *anchorName);

  // Add a work queue item for processing.  We always process the work queue after ReceivePayload
  // and when any of the timers expire.
  AddWorkQueueEntry (anchorName, prefix);

  // Add or remove entries from the FIB

  m_computationCost.IncrementEvents();
  Ptr<NfpPrefix> prefixEntry = m_prefixes[prefix];
  if (prefixEntry)
    {
      if (prefixEntry->IsReachable (anchorName))
        {
          AddAnchorToFib (prefixEntry, anchorName);
        }
      else
        {
          RemoveAnchorFromFib (prefixEntry, anchorName);
        }

      if (prefixEntry->IsReachable()) {
	  NS_LOG_WARN ("PrefixStateChanged:   reachable prefix " << *prefix);
      } else {
	  NS_LOG_WARN ("PrefixStateChanged: unreachable prefix " << *prefix);
      }
    }
  else
    {
      NS_ASSERT_MSG (false, "Got PrefixStateChanged for a prefix we cannot find in m_prefix " << *prefix);
    }
}

void
NfpRoutingProtocol::RemoveNeighborRoutes (Ptr<const CCNxName> neighborName, Ptr<CCNxConnection> connection)
{
  NS_LOG_FUNCTION (this << *neighborName << connection->GetConnectionId ());

  for (PrefixMapType::iterator i = m_prefixes.begin (); i != m_prefixes.end (); ++i)
    {
      m_computationCost.IncrementLoopIterations();
      Ptr<NfpPrefix> prefix = i->second;

      // this may induce a callback to PrefixStateChanged()
      prefix->RemoveConnection (connection);
    }
}

uint32_t
NfpRoutingProtocol::GetMinimumMtu (void) const
{
  uint32_t minimum = std::numeric_limits<uint32_t>::max ();
  InterfaceMapType::const_iterator i = m_interfaces.cbegin ();
  while (i != m_interfaces.cend ())
    {
      Ptr<CCNxL3Interface> interface = i->second;
      if (interface->IsUp () && interface->GetMtu () < minimum)
        {
          minimum = interface->GetMtu ();
        }
      i++;
    }

  NS_LOG_DEBUG ("Minimum MTU " << minimum);
  return minimum;
}

void
NfpRoutingProtocol::SendFromWorkQueue (Ptr<NfpPayload> payload)
{
  Ptr<CCNxPacket> packet = CreatePacket (payload);
  Broadcast (packet);
}

void
NfpRoutingProtocol::ProcessWorkQueue (void)
{
  if (!m_workQueue.empty()) {
      NS_LOG_DEBUG("Node " << m_node->GetId() << " Process work queue");

      Ptr<NfpPayload> payload = CreatePayload ();
      while (!m_workQueue.empty ())
        {
	  // don't increment the LoopIterations, we count this in the IncrementEvents() just below

	  Ptr<NfpWorkQueueEntry> workEntry = m_workQueue.pop_front ();
          NS_LOG_DEBUG ("ProcessWorkQueue workEntry " << *workEntry);

          m_computationCost.IncrementEvents();
          PrefixMapType::const_iterator i = m_prefixes.find (workEntry->GetPrefix ());
          if (i != m_prefixes.end ())
            {
              Ptr<NfpPrefix> prefixEntry = i->second;
              if (prefixEntry->IsReachable (workEntry->GetAnchorName ()))
                {
                  // send an advertisement

                  Ptr<const NfpAdvertise> bestInRib = prefixEntry->GetAdvertisement (workEntry->GetAnchorName ());
                  NS_ASSERT_MSG (bestInRib, "Failed to retrieve stored advertisement for someting in workqueue");

                  // TODO: Implement link cost table
                  uint16_t linkcost = 1;
                  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (*bestInRib, linkcost);

                  if (advertise->GetSerializedSize () > payload->GetRemaining ())
                    {
                      SendFromWorkQueue (payload);
                      payload = CreatePayload ();
                    }

                  NS_LOG_DEBUG("Append to payload " << *advertise);
                  payload->AppendMessage (advertise);

                  m_stats.IncrementAdvertiseSent();
                }
              else
                {
                  // send a withdraw

                  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> (workEntry->GetAnchorName (), prefixEntry->GetPrefix ());
                  if (withdraw->GetSerializedSize () > payload->GetRemaining ())
                    {
                      SendFromWorkQueue (payload);
                      payload = CreatePayload ();
                    }

                  NS_LOG_DEBUG("Append to payload " << *withdraw);
                  payload->AppendMessage (withdraw);
                  m_stats.IncrementWithdrawSent();
                }

              m_computationCost.IncrementEvents();
              // We need to keep the timerHeap entry alive for the (prefix, anchor) regardless of if we
              // sent an advertisement or a withdraw.  As long as there's an m_prefix entry for it, there
              // needs to be a timer for it.
              m_prefixTimerHeap.Insert (workEntry->GetPrefix (), workEntry->GetAnchorName (),
                                        GetCurrentTime () + CalculateJitteredTime (m_routeTimeout, m_jitter));

            }
        }

      // If we finish without filling the payload, send it now.
      if (payload->GetMessageCount () > 0)
        {
          SendFromWorkQueue (payload);
        }
  }
}

void
NfpRoutingProtocol::Broadcast (Ptr<CCNxPacket> packet)
{
  NS_ASSERT_MSG (m_portal, "CCNxPortal not created");

  for (InterfaceMapType::const_iterator i = m_interfaces.begin (); i != m_interfaces.end (); ++i)
    {
      m_computationCost.IncrementLoopIterations();
      m_stats.IncrementPayloadsSent();
      m_stats.IncrementBytesSent (packet->GetMessage ()->GetPayload ()->GetSize ());

      Ptr<CCNxL3Interface> l3interface = i->second;
      Ptr<CCNxConnection> broadcastConnection = l3interface->GetBroadcastConnection ();

      NS_LOG_DEBUG ("SendTo(" << *packet << ", " << broadcastConnection->GetConnectionId () << ")");
      m_portal->SendTo (packet, broadcastConnection->GetConnectionId ());
    }

  m_lastBroadcast = GetCurrentTime ();
}

// ========================================
// === Inherited from CCNxRoutingProtocol

void
NfpRoutingProtocol::NotifyInterfaceUp (uint32_t interface)
{
  NS_LOG_FUNCTION (this << interface);
  Ptr<CCNxL3Interface> l3interface = m_ccnx->GetInterface (interface);

  m_computationCost.IncrementEvents();
  m_interfaces[interface] = l3interface;
  m_minimumMtu = GetMinimumMtu ();

  NS_LOG_INFO ("Add interface " << interface << " mtu " << l3interface->GetMtu ());
}

void
NfpRoutingProtocol::NotifyInterfaceDown (uint32_t interface)
{
  NS_LOG_FUNCTION (this << interface);

  m_computationCost.IncrementEvents();
  InterfaceMapType::iterator i = m_interfaces.find (interface);
  if (i != m_interfaces.end ())
    {
      NS_LOG_INFO ("Remove interface " << interface);
      m_interfaces.erase (i);
    }

  m_minimumMtu = GetMinimumMtu ();
}

void
NfpRoutingProtocol::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  NS_ASSERT_MSG ((!m_node), "Re-assigning the node pointer once it was already set.");
  m_node = node;

  m_ccnx = m_node->GetObject<CCNxL3Protocol> ();
  NS_ASSERT_MSG (m_ccnx, "Failed to lookup the CCNxL3Protocol on this node");

  m_stats.SetNodeId(node->GetId());

  SetRouterName ();
}

void
NfpRoutingProtocol::AddWorkQueueEntry (Ptr<const CCNxName> anchorName, Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << anchorName << prefix);
  m_workQueue.push_back ( Create<NfpWorkQueueEntry> (anchorName, prefix) );
  SetProcessWorkQueueTimer();
}

Time
NfpRoutingProtocol::GetCurrentTime (void) const
{
  return Simulator::Now ();
}

void
NfpRoutingProtocol::PrintNeighborTable (Ptr<OutputStreamWrapper> streamWrapper) const
{
  std::ostream *stream = streamWrapper->GetStream ();
  ns3::LogTimePrinter timePrinter = ns3::LogGetTimePrinter ();
  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId ();
  *stream << " Neighbor table for " << *m_routerName << std::endl;
  size_t count = 0;
  for (NeighborMapType::const_iterator i = m_neighbors.cbegin (); i != m_neighbors.cend (); i++)
    {
      Ptr<const NfpNeighbor> neighbor = i->second;
      *stream << "      " << *neighbor << std::endl;
      count++;
    }
}

void
NfpRoutingProtocol::PrintRoutingStats (Ptr<OutputStreamWrapper> streamWrapper) const
{
  std::ostream *stream = streamWrapper->GetStream ();
  *stream << m_stats;
}

void
NfpRoutingProtocol::PrintRoutingTable (Ptr<OutputStreamWrapper> streamWrapper) const
{
  std::ostream *stream = streamWrapper->GetStream ();
  ns3::LogTimePrinter timePrinter = ns3::LogGetTimePrinter ();

  PrintRoutingStats(streamWrapper);

  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId ();
  *stream << " Routing table for " << *m_routerName << std::endl;
  size_t count = 0;
  for (PrefixMapType::const_iterator i = m_prefixes.cbegin (); i != m_prefixes.cend (); i++)
    {
      Ptr<const NfpPrefix> prefix = i->second;
      *stream << "      " << *prefix << std::endl;
      count++;
    }

  PrintAnchorPrefixes (streamWrapper);

//  *stream << "--- end ---\n";
}

void
NfpRoutingProtocol::PrintAnchorPrefixes (Ptr<OutputStreamWrapper> streamWrapper) const
{
  std::ostream *stream = streamWrapper->GetStream ();

  ns3::LogTimePrinter timePrinter = ns3::LogGetTimePrinter ();
  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId ();
  *stream << " Anchor table for " << *m_routerName << std::endl;
  size_t count = 0;
  for (AnchorNameMapType::const_iterator i = m_anchorNames.cbegin (); i != m_anchorNames.cend (); i++)
    {
      Ptr<const CCNxName> prefix = i->first;
      *stream << "      " << *prefix << std::endl;
      count++;
    }
}

int64_t
NfpRoutingProtocol::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uniformRandomVariable->SetStream (stream);
  return 1;
}

NfpComputationCost
NfpRoutingProtocol::GetComputationCost() const
{
  NfpComputationCost total(m_computationCost);
  for (PrefixMapType::const_iterator i = m_prefixes.begin(); i != m_prefixes.end(); ++i) {
      total += i->second->GetComputationCost();
  }

  for (NeighborMapType::const_iterator i = m_neighbors.begin(); i != m_neighbors.end(); ++i) {
      total += i->second->GetComputationCost();
  }
  return total;
}

NfpStats
NfpRoutingProtocol::GetStats() const
{
  return m_stats;
}

void
NfpRoutingProtocol::PrintComputationCost (Ptr<OutputStreamWrapper> streamWrapper) const
{
  PrintRoutingStats(streamWrapper);

  std::ostream *stream = streamWrapper->GetStream ();

  ns3::LogTimePrinter timePrinter = ns3::LogGetTimePrinter ();
  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId ();
  *stream << " " << GetComputationCost() << std::endl;
}
