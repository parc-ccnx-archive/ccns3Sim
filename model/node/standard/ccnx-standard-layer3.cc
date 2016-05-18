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

#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/uinteger.h"
#include "ns3/object-vector.h"
#include "ns3/boolean.h"
#include "ns3/integer.h"

#include "ns3/ccnx-standard-layer3.h"
#include "ns3/ccnx-routing-error.h"
#include "ns3/trace-source-accessor.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxStandardLayer3");
NS_OBJECT_ENSURE_REGISTERED (CCNxStandardLayer3);

const uint16_t CCNxStandardLayer3::m_protocolNumber = 0x0801;

TypeId
CCNxStandardLayer3::GetTypeId (void)
{
  static TypeId tid =
    TypeId ("ns3::ccnx::CCNxStandardLayer3")
    .SetParent<CCNxL3Protocol> ()
    .AddConstructor<CCNxStandardLayer3> ()
    .SetGroupName ("CCNx")
    .AddTraceSource ("Tx",
                     "Send CCNx packet to outgoing interface.",
                     MakeTraceSourceAccessor (&CCNxStandardLayer3::m_txTrace),
                     "ns3::ccnx::CCNxStandardLayer3::TxRxTracedCallback")
    .AddTraceSource ("Rx",
                     "Receive CCNx packet from incoming interface.",
                     MakeTraceSourceAccessor (&CCNxStandardLayer3::m_rxTrace),
                     "ns3::ccnx::CCNxStandardLayer3::TxRxTracedCallback")
    .AddTraceSource ("Drop",
                     "Drop CCNx packet",
                     MakeTraceSourceAccessor (&CCNxStandardLayer3::m_dropTrace),
                     "ns3::ccnx::CCNxStandardLayer3::DropTracedCallback")
  ;
  return tid;
}

uint16_t
CCNxStandardLayer3::GetProtocolNumber (void)
{
  return m_protocolNumber;
}

CCNxStandardLayer3::CCNxStandardLayer3 ()
{
  NS_LOG_FUNCTION (this);
  m_ccnxForward = true;
  m_defaultTtl = 255;
}

CCNxStandardLayer3::~CCNxStandardLayer3 ()
{
  NS_LOG_FUNCTION (this);
  m_protocols.clear ();
}

void
CCNxStandardLayer3::AddProtocol (const CCNxL4Protocol &protocol,
                                 CCNxL4Protocol::ReceiveCallback callback)
{
  CCNxL4Protocol::CCNxL4Id id = protocol.GetInstanceId ();
  Ptr<CCNxConnectionL4> conn = Create<CCNxConnectionL4> ();
  conn->SetReceiveCallback (callback);

  m_protocols[id] = conn;
  m_connections[conn->GetConnectionId ()] = conn;

  NS_LOG_DEBUG ("AddProtocol protocol " << &protocol << " id " << id);
}

void
CCNxStandardLayer3::UnregisterProtocol (const CCNxL4Protocol &protocol)
{
  CCNxL4Protocol::CCNxL4Id id = protocol.GetInstanceId ();

  L4MapType::iterator i = m_protocols.find (id);
  if (i != m_protocols.end ())
    {
      Ptr<CCNxConnectionL4> conn = i->second;

      NS_LOG_INFO ("Deleting ConnID " << conn->GetConnectionId ());

      // mark it as closed
      conn->Close ();

      // TODO: Remove FIB entries that use this nexthop
      m_connections.erase (conn->GetConnectionId ());
      m_protocols.erase (i);
    }
  else
    {
      NS_ASSERT_MSG (i != m_protocols.end (),
                     "Could not find protocol id " << id);
    }
}

void
CCNxStandardLayer3::RegisterPrefix (const CCNxL4Protocol &protocol,
                                    Ptr<const CCNxName> prefix)
{
  CCNxL4Protocol::CCNxL4Id id = protocol.GetInstanceId ();
  NS_LOG_FUNCTION (this << prefix << id);

  L4MapType::iterator i = m_protocols.find (id);
  if (i != m_protocols.end ())
    {
      Ptr<CCNxConnectionL4> conn = i->second;

      NS_LOG_INFO (
        "RegisterPrefix   L4Id " << id << " ConnId " << conn->GetConnectionId () << " for " << *prefix);

      m_forwarder->AddRoute (conn, prefix);
    }
  else
    {
      NS_ASSERT_MSG (i != m_protocols.end (),
                     "Could not find protocol id " << id);
    }
}

void
CCNxStandardLayer3::UnregisterPrefix (const CCNxL4Protocol &protocol,
                                      Ptr<const CCNxName> prefix)
{
  CCNxL4Protocol::CCNxL4Id id = protocol.GetInstanceId ();
  NS_LOG_FUNCTION (this << prefix << id);

  L4MapType::iterator i = m_protocols.find (id);
  if (i != m_protocols.end ())
    {
      Ptr<CCNxConnectionL4> conn = i->second;

      NS_LOG_INFO (
        "UnregisterPrefix L4Id " << id << " ConnId " << conn->GetConnectionId () << " for " << *prefix);

      m_forwarder->RemoveRoute (conn, prefix);
    }
  else
    {
      NS_ASSERT_MSG (i != m_protocols.end (),
                     "Could not find protocol id " << id);
    }
}

void
CCNxStandardLayer3::RegisterAnchor (const CCNxL4Protocol &protocol,
                                    Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << protocol.GetInstanceId () << prefix);
  RegisterPrefix (protocol, prefix);
  if (m_routingProtocol)
    {
      m_routingProtocol->AddAnchorPrefix (prefix);
    }
}

void
CCNxStandardLayer3::UnregisterAnchor (const CCNxL4Protocol &protocol,
                                      Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << protocol.GetInstanceId () << prefix);
  UnregisterPrefix (protocol, prefix);
  if (m_routingProtocol)
    {
      m_routingProtocol->RemoveAnchorPrefix (prefix);
    }
}

void
CCNxStandardLayer3::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

Ptr<Node>
CCNxStandardLayer3::GetNode (void) const
{
  return m_node;
}

void
CCNxStandardLayer3::SetRoutingProtocol (Ptr<CCNxRoutingProtocol> routingProtocol)
{
  NS_LOG_FUNCTION (this << routingProtocol);
  m_routingProtocol = routingProtocol;
}

Ptr<CCNxRoutingProtocol>
CCNxStandardLayer3::GetRoutingProtocol (void) const
{
  NS_LOG_FUNCTION (this);
  return m_routingProtocol;
}

void
CCNxStandardLayer3::SetForwarder (Ptr<CCNxForwarder> forwarder)
{
  m_forwarder = forwarder;
  m_forwarder->SetRouteCallback (MakeCallback (&CCNxStandardLayer3::RouteCallback, this));
}

Ptr<CCNxForwarder>
CCNxStandardLayer3::GetForwarder (void) const
{
  return m_forwarder;
}

void
CCNxStandardLayer3::SetDefaultTtl (uint8_t ttl)
{
  NS_LOG_FUNCTION (this << static_cast<uint32_t> (ttl));
  m_defaultTtl = ttl;
}

// ==========================
// Interface related methods

uint32_t
CCNxStandardLayer3::AddInterface (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device);

  m_node->RegisterProtocolHandler (
    MakeCallback (&CCNxStandardLayer3::ReceiveFromLayer2, this),
    CCNxStandardLayer3::GetProtocolNumber (), device);

  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  uint32_t id = device->GetIfIndex ();
  m_devices[id] = l3if;
  NS_LOG_INFO ("AddInterface " << device << " id " << id);

  Ptr<CCNxConnectionDevice> bcast = AddNeighbor (device->GetBroadcast (), device);
  l3if->SetBroadcastConnection (bcast);

  l3if->SetForwarding (m_ccnxForward);
  l3if->SetUp ();

  return id;
}

bool
CCNxStandardLayer3::GetInterfaceForwarding (uint32_t interface) const
{
  bool result = false;
  Ptr<CCNxL3Interface> l3if = GetInterface (interface);
  if (l3if)
    {
      if (l3if->IsUp () && l3if->IsForwarding ())
        {
          result = true;
        }
    }
  return result;
}

uint32_t
CCNxStandardLayer3::GetNInterfaces (void) const
{
  return m_devices.size ();
}

Ptr<CCNxL3Interface>
CCNxStandardLayer3::GetInterface (uint32_t interface) const
{
  L2MapType::const_iterator i = m_devices.find (interface);
  if (i != m_devices.end ())
    {
      return i->second;
    }
  else
    {
      return 0;
    }
}

// ===========================

void
CCNxStandardLayer3::RouteCallback (Ptr<CCNxPacket> ccnxPacket, Ptr<CCNxConnection> ingress,
                                   enum CCNxRoutingError::RoutingErrno routingErrno, Ptr<CCNxConnectionList> egress)
{

  if (egress->size () == 0)
    {
      NS_LOG_WARN ("forwarder return an empty result " << *ccnxPacket->GetMessage ()->GetName ());
      m_dropTrace (this, ccnxPacket->CreateNs3Packet (), ccnxPacket, ingress->GetConnectionId (), DROP_NO_ROUTE);
    }

  Ptr<CCNxConnection> egressFromForwarder;

  for (CCNxConnectionList::iterator it = egress->begin (); it != egress->end (); it++)
    {
      egressFromForwarder = *it;      //TODO send to all connections not just the first

      if (routingErrno == CCNxRoutingError::CCNxRoutingError_NoError)
        {
          NS_LOG_DEBUG ("SendCCNxPacket using egress connid " << egressFromForwarder->GetConnectionId () << " packet " << *ccnxPacket->CreateNs3Packet ());
          m_txTrace (this, ccnxPacket->CreateNs3Packet (), ccnxPacket, egressFromForwarder->GetConnectionId ());
          bool result = egressFromForwarder->Send (ccnxPacket, ingress);
          if (!result)     //Send failed
            {
              NS_LOG_WARN ("Send failed!");
              m_dropTrace (this, ccnxPacket->CreateNs3Packet (), ccnxPacket, -1, DROP_INTERFACE_DOWN);
            }
        }
      else
        {
          NS_LOG_WARN ("Routing Error " << *ccnxPacket->GetMessage ()->GetName ());
          m_dropTrace (this, ccnxPacket->CreateNs3Packet (), ccnxPacket, ingress->GetConnectionId (), DROP_NO_ROUTE);
        }
    }
}

bool
CCNxStandardLayer3::Layer4RouteOut (Ptr<CCNxPacket> ccnxPacket, Ptr<CCNxConnectionL4> ingress, Ptr<CCNxConnection> egress)
{
  bool result = true;

  if (egress)
    {
      NS_LOG_DEBUG ("Layer4RouteOut " << *ccnxPacket << " ingress connid " << ingress->GetConnectionId () << " egress " << egress->GetConnectionId ());
    }
  else
    {
      NS_LOG_DEBUG ("Layer4RouteOut " << *ccnxPacket << " ingress connid " << ingress->GetConnectionId () << " egress none");
    }

  m_forwarder->RouteOutput (ccnxPacket, ingress, egress);

  return result;
}

bool
CCNxStandardLayer3::SendFromLayer4 (const CCNxL4Protocol &protocol, Ptr<CCNxPacket> ccnxPacket)
{
  CCNxL4Protocol::CCNxL4Id id = protocol.GetInstanceId ();
  NS_LOG_FUNCTION (this << &protocol << ccnxPacket);

  // serialize it
  Ptr<Packet> ns3Packet = ccnxPacket->CreateNs3Packet ();

  m_rxTrace (this, ns3Packet, ccnxPacket, id);

  // look up the incoming protocols Connection and receive it from there
  bool result = false;
  L4MapType::iterator i = m_protocols.find (id);
  if (i != m_protocols.end ())
    {
      Ptr<CCNxConnectionL4> ingress = i->second;
      result = Layer4RouteOut (ccnxPacket, ingress, Ptr<CCNxConnection> (0));
    }
  else
    {
      NS_LOG_ERROR ("SendCCNxPacket " << *ccnxPacket << " l4id " << id << " : Could not find protocol id");
      m_dropTrace (this, ns3Packet, ccnxPacket, id, DROP_ROUTE_ERROR);
      NS_ASSERT_MSG (i != m_protocols.end (), "Could not find protocol id " << id);
    }
  return result;
}

bool
CCNxStandardLayer3::SendToFromLayer4 (const CCNxL4Protocol &protocol, Ptr<CCNxPacket> ccnxPacket, uint32_t egressConnectionId)
{
  bool result = false;

  CCNxL4Protocol::CCNxL4Id id = protocol.GetInstanceId ();
  NS_LOG_FUNCTION (this << &protocol << ccnxPacket);

  // serialize it
  Ptr<Packet> ns3Packet = ccnxPacket->CreateNs3Packet ();

  m_rxTrace (this, ns3Packet, ccnxPacket, id);

  Ptr<CCNxConnection> egress = GetConnection (egressConnectionId);
  if (egress)
    {
      // look up the incoming protocols Connection and receive it from there
      bool result = false;
      L4MapType::iterator i = m_protocols.find (id);
      if (i != m_protocols.end ())
        {
          Ptr<CCNxConnectionL4> ingress = i->second;
          result = Layer4RouteOut (ccnxPacket, ingress, egress);
        }
      else
        {
          NS_LOG_ERROR ("SendCCNxPacket " << *ccnxPacket << " l4id " << id << " : Could not find protocol id");
          m_dropTrace (this, ns3Packet, ccnxPacket, id, DROP_ROUTE_ERROR);
          NS_ASSERT_MSG (i != m_protocols.end (), "Could not find protocol id " << id);
        }
    }
  else
    {
      NS_LOG_WARN ("Could not resolve egress connId " << egressConnectionId);
      m_dropTrace (this, ns3Packet, ccnxPacket, id, DROP_ROUTE_ERROR);
    }

  return result;
}

void
CCNxStandardLayer3::ReceiveFromLayer2 (Ptr<NetDevice> device, Ptr<const Packet> p,
                                       uint16_t protocol, const Address &from,
                                       const Address &to, NetDevice::PacketType packetType)
{
  Ptr<Packet> packet = p->Copy ();

  NS_LOG_FUNCTION (this << device << packet << protocol << from << to << packetType);

  NS_LOG_DEBUG ("Node " << m_node->GetId () << " Packet from " << from << " size " << packet->GetSize () << " packet " << *p);


  // If the L3 interface is down, drop the packet
  if (GetInterfaceForwarding (device->GetIfIndex ()))
    {
      // Lookup the incoming NetDevice's Connection and receive it from there

      NbrMapType::const_iterator i = m_neighbors.find (from);
      Ptr<CCNxConnectionDevice> ingress;
      if (i == m_neighbors.end ())
        {
          // If we don't know about the neighor add him
          Ptr<CCNxConnectionDevice> deviceConnection = AddNeighbor (from, device);
          ingress = deviceConnection;
        }
      else
        {
          ingress = i->second;
        }

      // Deserialize the packet, then grab the output from the ccnxPacket, as that
      // will be properly trimed of any layer 3 bloat in the ns3 packet.
      Ptr<CCNxPacket> ccnxPacket = CCNxPacket::CreateFromNs3Packet (packet);
      packet = ccnxPacket->CreateNs3Packet ();

      NS_ASSERT_MSG (ccnxPacket->GetFixedHeader ()->GetPacketLength () == packet->GetSize (),
                     "Packet wrong size, FixedHeader = " << (uint32_t) ccnxPacket->GetFixedHeader ()->GetPacketLength () <<
                     ", packet->GetSize() = " << packet->GetSize () << ", packet = " << *packet);

      m_rxTrace (this, packet, ccnxPacket, ingress->GetConnectionId ());

      m_forwarder->RouteInput (ccnxPacket, ingress);
    } //GetInterfaceForwarding
}

Ptr<CCNxConnection>
CCNxStandardLayer3::GetConnection (CCNxConnection::ConnIdType id) const
{
  ConnMapType::const_iterator i = m_connections.find (id);
  if (i != m_connections.end ())
    {
      return i->second;
    }
  return (Ptr<CCNxConnection> ) 0;
}

Ptr<CCNxConnectionDevice>
CCNxStandardLayer3::AddNeighbor (Address nexthop, Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << nexthop << device);

  Ptr<CCNxL3Interface> iface = GetInterface (device->GetIfIndex ());
  NS_ASSERT_MSG (iface, "Could not resolve iface for device " << device->GetIfIndex ());

  Ptr<CCNxConnectionDevice> conn = Create<CCNxConnectionDevice> ();
  conn->SetNexthop (nexthop);
  conn->SetOutputDevice (device);
  conn->SetProtocolNumber (GetProtocolNumber ());

  m_neighbors[nexthop] = conn;
  m_connections[conn->GetConnectionId ()] = conn;

  NS_LOG_INFO ("AddNeighbor ConnId " << conn->GetConnectionId () << " address " << nexthop);
  return conn;
}

