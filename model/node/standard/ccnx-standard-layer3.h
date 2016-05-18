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

#ifndef CCNS3_CCNXSTANDARDL3_H
#define CCNS3_CCNXSTANDARDL3_H

#include "ns3/ccnx-l3-protocol.h"

#include <map>

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/net-device.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"

#include "ns3/ccnx-fixedheader.h"
#include "ns3/ccnx-l3-interface.h"
#include "ns3/ccnx-packet.h"
#include "ns3/ccnx-routing-protocol.h"
#include "ns3/ccnx-l4-protocol.h"
#include "ns3/ccnx-connection-device.h"
#include "ns3/ccnx-connection-l4.h"
#include "ns3/ccnx-forwarder.h"
#include "ns3/ccnx-routing-error.h"

namespace ns3 {

class Packet;
class NetDevice;
class Node;

namespace ccnx {

/**
 * @defgroup ccnx-standard-l3 CCNx Standard L3
 * @ingroup ccnx-l3
 */

/**
 * @ingroup ccnx-standard-l3
*
* Implements the NS3 interface for CCNx Forwarder implementation (CCNxForwarder)
*
* Like Metis, we use a ConnectionId to identify each peer.
* There are two kinds of peers: local L4 protocols and NetDevices.
*
* A L4 Protocol is identified by its GetInstanceId() which is guaranteed
* to be unique (over the whole simulator even).
*
* A NetDevice is identified by its GetIfIndex (a uint32_t). We could probably
* have used its GetAddress too.
*
* A CCNxConnection can be either a CCNxConnectionLocal or a CCNxConnectionDevice,
* representing the two types of connections.  Each one implements a Send(Ptr<CCNxPacket>) function
* specific to how we send data over that device.
*
* Each CCNxConnection has a unique ConnId (unique over whole simulator).
*
* TODO: CCnxConnectionDevice should be renamed CCNxConnectionNeighbor
*
* Tables:
*    m_protocols: CCnxL4Protocol::CCNxL4Id -> Ptr<CCNxConnectionL4>
*    m_devices: GetIfIndex -> Ptr<CCNxL3Interface>
*
*    m_neighbors: Address -> Ptr<CCNxConnectionDevice>
*
*    m_connections: ConnId -> Ptr<CCNxConnection>
*
* To manage the tables from the configuration file:
*
*  # Address = L2 Address of Peer
*  AddNeighbor(Address, Ptr<NetDevice>)
*  AddRoute(Address, Ptr<const CCNxName>)
*
*  # To add a L4 route, use RegisterPrefix via CCNxPortal.
*/
class CCNxStandardLayer3 : public CCNxL3Protocol
{
public:
  static TypeId GetTypeId (void);
  static uint16_t GetProtocolNumber (void);

  /**
   * Constructor to create CCNxStandardLayer3
   */
  CCNxStandardLayer3 ();

  /**
   * Destructor for CCNxStandardLayer3
   */
  virtual ~CCNxStandardLayer3 ();

  /**
   * Set node associated with this stack.
   *
   * @param [in] node node to set
   */
  void SetNode (Ptr <Node> node);

  /**
   * Returns the node this CCNx Layer 3 is bound to.
   *
   * @return Pointer to the node object
   */
  Ptr<Node> GetNode (void) const;

  /**
   * Register a new routing protocol to be used by this CCNx stack
   *
   * This call will replace any routing protocol that has been previously
   * registered.  If you want to add multiple routing protocols, you must
   * add them to a CCNxListRoutingProtocol directly.
   *
   * @param [in] routingProtocol smart pointer to CCNxRoutingProtocol object
   */
  void SetRoutingProtocol (Ptr<CCNxRoutingProtocol> routingProtocol);

  /**
   * Set the instantiation of the CCNxForwarder to use on the node.
   *
   * @param [in] forwarder Pointer to the CCNxForwarder object
   */
  void SetForwarder (Ptr<CCNxForwarder> forwarder);

  /**
   * Returns the forwarder set on the node
   *
   * @return Pointer to the forwarder object
   */
  Ptr<CCNxForwarder> GetForwarder (void) const;

  /**
   * Get the routing protocol to be used by the stack
   *
   * @return Pointer to CCNxRoutingProtocol object, or null pointer if none
   */
  Ptr<CCNxRoutingProtocol> GetRoutingProtocol (void) const;

  /**
   * Interface related methods
   */

  /**
   * Add an interface to a device
   * Once added, it can never be removed: if you want
   * to disable it, you can invoke Ipv4::SetDown which will
   * make sure that it is never used during packet forwarding.
   *
   * @param [in] device device to add the list of Ipv4 interfaces to,
   *             which can be used as output interfaces during packet forwarding.
   * @return The index of the Ipv4 interface added.
   */
  uint32_t AddInterface (Ptr<NetDevice> device);

  /**
   * @return The number of interfaces added by the user.
   */
  uint32_t GetNInterfaces (void) const;

  /**
   * @param [in] interface Interface index
   * @return Pointer to the interface object.
   */
  Ptr<CCNxL3Interface> GetInterface (uint32_t interface) const;

  /**
   * To be forwarding, an interface must be both L3 up and L3 forwarding
   */
  bool GetInterfaceForwarding (uint32_t interface) const;

  /**
     * CCNxL4Protocol Send Prototype
     */

  /**
   * Called from a Layer 4 protocol to send a packet.
   * The packet will be sent through RouteOut() of the CCNxForwarder.
   *
   * @param [in] protocol L4 protocol to be used for sending the packet
   * @param [in] ccnxPacket Packet to be sent
   * @return true if forwarded, false otherwise
   */
  bool SendFromLayer4 (const CCNxL4Protocol &protocol, Ptr<CCNxPacket> ccnxPacket);

  /**
   * Call from upper-level layer to send a packet, with specified egress connection id.
   *
   * Normal forwarding does not happen.  While the packet is still processed through RouteOut() so the
   * forwarder sees the packet, it will return the same egress connection as specified in the call.
   * The FIB is not used.  The forwarder must see the packet so it can make PIT and CS updates as needed.
   *
   * @param [in] protocol The L4 protocol sending the packet.  Used to register a reverse route.
   * @param [in] ccnxPacket The packet to send.
   * @param [in] egressConnectionId The connection ID of the egress connection.
   * @return true if forwarded, false otherwise.
   */
  bool SendToFromLayer4 (const CCNxL4Protocol &protocol, Ptr<CCNxPacket> ccnxPacket, uint32_t egressConnectionId);

  /**
   * L2 calls this based on CCNxL3Protocol registering a Protocol Number
   * callback with NS3.
   */
  void ReceiveFromLayer2 (Ptr <NetDevice> device, Ptr<const Packet> p, uint16_t protocol, const Address &from,
                          const Address &to, NetDevice::PacketType packetType);

  /**
   * Add a Layer 4 protocol to the system.  This is how a CCNxPortal registers with Layer 3.
   *
   * @param [in] protocol The CCNx layer 4 protocol
   * @param [in] callback The callback inside the layer 4 protocol to use for sending a packet up the stack.
   */
  void AddProtocol (const CCNxL4Protocol &protocol, CCNxL4Protocol::ReceiveCallback callback);

  /**
   * Removes a Layer 4 protocol from the system.  This is how a CCNxPortal removes itself from layer 3
   * when it is closed.
   *
   * @param [in] protocol The CCNx layer 4 protocol
   */
  void UnregisterProtocol (const CCNxL4Protocol &protocol);

  /**
   * Register a prefix with the local CCNxForwarder.  This lets a Layer 4 protocol ask for a name
   * to be routed up the stack to a specific CCNxPortal.  There is no interaction with the
   * CCNxRoutingProtocol.
   *
   * @param [in] protocol The Layer 4 protocol responsible for the prefix.
   * @param [in] prefix The CCNxName prefix
   *
   * @see RegisterAnchor, UnregisterPrefix
   */
  void RegisterPrefix (const CCNxL4Protocol &protocol, Ptr<const CCNxName> prefix);

  /**
   * Removes the association of a prefix with a Layer 4 protocol.  It is only removed from
   * the local CCNxForwarder, not with the CCNxRoutingProtocol.  This undoes the action
   * of RegisterPrefix().
   *
   * @param [in] protocol The Layer 4 protocol responsible for the prefix.
   * @param [in] prefix The CCNxName prefix
   */
  void UnregisterPrefix (const CCNxL4Protocol &protocol, Ptr<const CCNxName> prefix);

  /**
   * Does the same thing as CCNxRegisterPrefix, but in addition tells the CCNxRoutingProtocol to
   * advertise the prefix as being locally serviced on this node.  You do not need to call both
   * RegisterPrefix() and RegisterAnchor().
   *
   * @param [in] protocol The Layer 4 protocol responsible for the prefix.
   * @param [in] prefix The CCNxName prefix
   *
   * @see RegisterPrefix, UnregisterAnchor
   */
  void RegisterAnchor (const CCNxL4Protocol &protocol, Ptr<const CCNxName> prefix);

  /**
   * Removes the association of a prefix with a Layer 4 protocol.  This undoes the effects
   * of RegisterAnchor().  It removes the association from the CCNxForwarder and tells the
   * CCNxRoutingProtocol to stop advertising it.  Note that CCNxRoutingProtocol keeps a
   * use count of RegisterAnchor() calls for the same prefix, so if multiple Layer 4 protocols
   * asked for the same prefix, calling this function will only remove one use count.
   *
   * @param [in] protocol The Layer 4 protocol responsible for the prefix.
   * @param [in] prefix The CCNxName prefix
   */
  void UnregisterAnchor (const CCNxL4Protocol &protocol, Ptr<const CCNxName> prefix);


  /**
   * Set the default TTL value
   * @param [in] ttl default ttl to use
   */
  void SetDefaultTtl (uint8_t ttl);

  /**
   * Table Management methods
   */

  /**
   * Create the neighbor adjacency on a node
   *
   * @param [in] nexthop Mac address of the neighbor's interface
   * @param [in] device Device object through which neighbor is reachable
   * @return Pointer to the ConnectionDevice object
   */
  Ptr<CCNxConnectionDevice> AddNeighbor (Address nexthop, Ptr<NetDevice>);

  /**
   * @return The connection object associated with the connection Id
   */
  Ptr<CCNxConnection> GetConnection (CCNxConnection::ConnIdType id) const;

protected:
  /**
   * Route a packet received from Layer 4 to output device(s).  This function is called from Layer4Send() and
   * Layer4SendTo().
   *
   * Will call the forwarder's RouteOut() method and if egress connections are returned, will forward
   * the packet out those connections.
   *
   * If the egress connection is specified from Layer4, that connection will be used instead of a FIB/PIT lookup.
   * See the documentation for CCNxForwarder::Routeout().
   *
   * @return true if forwarded, false otherwise
   */
  bool Layer4RouteOut (Ptr<CCNxPacket> ccnxPacket, Ptr<CCNxConnectionL4> ingress, Ptr<CCNxConnection> egress);

  /**
   * Asynchronous callback from `CCNxForwarder::RouteIn()` and `CCNxForwarder::RouteOut()`.
   *
   * @param packet [in] The packet being routed
   * @param ingress [in] The ingress connection of the packet
   * @param routingErrno [in] The route error (if any)
   * @param egress [in] The list of egress connections (may be empty)
   */
  void RouteCallback (Ptr<CCNxPacket> packet, Ptr<CCNxConnection> ingress,
                      enum CCNxRoutingError::RoutingErrno routingErrno, Ptr<CCNxConnectionList> egress);

protected:
  static const uint16_t m_protocolNumber;

  bool m_ccnxForward;              /* Forwarding packets (i.e. router mode) state */
  uint8_t m_defaultTtl;            /* Default TTL */
  Ptr<Node> m_node;                /* Node attached to stack */

  Ptr<CCNxRoutingProtocol> m_routingProtocol;
  Ptr<CCNxForwarder> m_forwarder;

  /* Our L4 protocols */
  typedef std::map < CCNxL4Protocol::CCNxL4Id, Ptr<CCNxConnectionL4> > L4MapType;
  L4MapType m_protocols;

  /* Our L2 devices */
  typedef std::map < uint32_t, Ptr<CCNxL3Interface> > L2MapType;
  L2MapType m_devices;

  /* 1 hop neighbors on L2 devices */
  typedef std::map < Address, Ptr<CCNxConnectionDevice> > NbrMapType;
  NbrMapType m_neighbors;

  /**
   * This holds both Local and Device connections for lookup by ConnId, which
   * is what we use in the FIB and PIT.
   */
  typedef std::map < uint32_t, Ptr<CCNxConnection> > ConnMapType;
  ConnMapType m_connections;

private:
  /*
   * Packet Tracing
   * The implementation of these trace methods is in ccnx-stack-helper. The
   * prototypes are defined in ccnx-l3-protocol.h.
   */

  /* Trace sent packets */
  TracedCallback<const CCNxL3Protocol *, Ptr<const Packet>, Ptr<const CCNxPacket>,  uint32_t> m_txTrace;

  /* Trace received packets */
  TracedCallback<const CCNxL3Protocol *, Ptr<const Packet>, Ptr<const CCNxPacket>, uint32_t> m_rxTrace;

  /* Trace of dropped packets */
  TracedCallback<const CCNxL3Protocol *, Ptr<const Packet>, Ptr<const CCNxPacket>, uint32_t, DropReason> m_dropTrace;
};

} // namespace ccnx
} // namespace ns3


#endif //CCNS3_CCNXL3PROTOCOL_H
