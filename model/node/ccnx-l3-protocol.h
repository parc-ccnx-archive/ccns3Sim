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


#ifndef CCNS3_CCNXL3PROTOCOL_H
#define CCNS3_CCNXL3PROTOCOL_H

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

namespace ns3 {

class Packet;
class NetDevice;
class Node;

namespace ccnx {

/**
 * @defgroup ccnx-l3 CCNx Layer 3 Protocol
 * @ingroup ccnx
 */

/**
 * @ingroup ccnx-l3
 *
 * Defines the CCNx Layer 3 protocol
 *
 * `CCNxL3Protocol` is the abstract base class for a CCNx Layer 3 protocol.  It is implemented by
 * `standard/ccnx-standard-layer3.{h,cc}`.  If one wishes to use a different layer 3 protocol implementation,
 * `CCNxStackHelper` provides a function `SetLayer3ProtocolTypeId()`, which defaults to "ns3::ccnx::CCNxStandardLayer3".
 *
 */
class CCNxL3Protocol : public Object
{
public:
  static TypeId GetTypeId (void);

  /**
   * @return Returns the EtherType protocol number
   */
  static uint16_t GetProtocolNumber (void);

  /**
   * Constructor to create L3 protocol
   */
  CCNxL3Protocol ();

  /**
   * Destructor for L3 protocol
   */
  virtual ~CCNxL3Protocol ();

  /**
   * Set node associated with this stack.
   *
   * @param [in] node node to set
   */
  virtual void SetNode (Ptr <Node> node) = 0;

  /**
   * Returns the node this CCNx Layer 3 is bound to.
   *
   * @return Pointer to the node object
   */
  virtual Ptr<Node> GetNode (void) const = 0;

  /**
   * Register a new routing protocol to be used by this CCNx stack
   *
   * This call will replace any routing protocol that has been previously
   * registered.  If you want to add multiple routing protocols, you must
   * add them to a CCNxListRoutingProtocol directly.
   *
   * @param [in] routingProtocol smart pointer to CCNxRoutingProtocol object
   */
  virtual void SetRoutingProtocol (Ptr<CCNxRoutingProtocol> routingProtocol) = 0;

  /**
   * Set the instantiation of the CCNxForwarder to use on the node.
   *
   * @param [in] forwarder Pointer to the CCNxForwarder object
   */
  virtual void SetForwarder (Ptr<CCNxForwarder> forwarder) = 0;

  /**
   * Returns the forwarder set on the node
   *
   * @return Pointer to the forwarder object
   */
  virtual Ptr<CCNxForwarder> GetForwarder (void) const = 0;

  /**
   * Get the routing protocol to be used by the stack
   *
   * @return Pointer to CCNxRoutingProtocol object, or null pointer if none
   */
  virtual Ptr<CCNxRoutingProtocol> GetRoutingProtocol (void) const = 0;

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
  virtual uint32_t AddInterface (Ptr<NetDevice> device) = 0;

  /**
   * @return The number of interfaces added by the user.
   */
  virtual uint32_t GetNInterfaces (void) const = 0;

  /**
   * @param [in] interface Interface index
   * @return Pointer to the interface object.
   */
  virtual Ptr<CCNxL3Interface> GetInterface (uint32_t interface) const = 0;

  /**
   * To be forwarding, an interface must be both L3 up and L3 forwarding
   */
  virtual bool GetInterfaceForwarding (uint32_t interface) const = 0;

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
  virtual bool SendFromLayer4 (const CCNxL4Protocol &protocol, Ptr<CCNxPacket> ccnxPacket) = 0;

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
  virtual bool SendToFromLayer4 (const CCNxL4Protocol &protocol, Ptr<CCNxPacket> ccnxPacket, uint32_t egressConnectionId) = 0;

  /**
   * L2 calls this based on CCNxL3Protocol registering a Protocol Number
   * callback with NS3.
   */
  virtual void ReceiveFromLayer2 (Ptr <NetDevice> device, Ptr<const Packet> p, uint16_t protocol, const Address &from,
                                  const Address &to, NetDevice::PacketType packetType) = 0;

  /**
   * Add a Layer 4 protocol to the system.  This is how a CCNxPortal registers with Layer 3.
   *
   * @param [in] protocol The CCNx layer 4 protocol
   * @param [in] callback The callback inside the layer 4 protocol to use for sending a packet up the stack.
   */
  virtual void AddProtocol (const CCNxL4Protocol &protocol, CCNxL4Protocol::ReceiveCallback callback) = 0;

  /**
   * Removes a Layer 4 protocol from the system.  This is how a CCNxPortal removes itself from layer 3
   * when it is closed.
   *
   * @param [in] protocol The CCNx layer 4 protocol
   */
  virtual void UnregisterProtocol (const CCNxL4Protocol &protocol) = 0;

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
  virtual void RegisterPrefix (const CCNxL4Protocol &protocol, Ptr<const CCNxName> prefix) = 0;

  /**
   * Removes the association of a prefix with a Layer 4 protocol.  It is only removed from
   * the local CCNxForwarder, not with the CCNxRoutingProtocol.  This undoes the action
   * of RegisterPrefix().
   *
   * @param [in] protocol The Layer 4 protocol responsible for the prefix.
   * @param [in] prefix The CCNxName prefix
   */
  virtual void UnregisterPrefix (const CCNxL4Protocol &protocol, Ptr<const CCNxName> prefix) = 0;

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
  virtual void RegisterAnchor (const CCNxL4Protocol &protocol, Ptr<const CCNxName> prefix) = 0;

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
  virtual void UnregisterAnchor (const CCNxL4Protocol &protocol, Ptr<const CCNxName> prefix) = 0;

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
  virtual Ptr<CCNxConnectionDevice> AddNeighbor (Address nexthop, Ptr<NetDevice>) = 0;

  /**
   * @return The connection object associated with the connection Id
   */
  virtual Ptr<CCNxConnection> GetConnection (CCNxConnection::ConnIdType id) const = 0;

  /**
   * Packet tracing
   */

  /**
   * DropReason - Reason why a packet has been dropped.
   */
  enum DropReason
  {
    DROP_TTL_EXPIRED = 1,   /**< Packet TTL has expired */
    DROP_NO_ROUTE,          /**< No route to host */
    DROP_INTERFACE_DOWN,    /**< Interface is down so can not send packet */
    DROP_ROUTE_ERROR,       /**< Route error */
  };

  /**
   * TracedCallback signature for packet send.  Packet send means the packet was injected from a Layer 4
   * protocol at the node.
   *
   * @param [in] The CCNx L3 reporting the trace
   * @param [in] packet The packet.
   * @param [in] ccnxPacket The CCNxPacket representation
   * @param [in] interface
   */
  typedef void (* SentTracedCallback) (const CCNxL3Protocol *ccnx, Ptr<const Packet> packet, Ptr<const CCNxPacket> ccnxPacket, uint32_t interface);

  /**
   * TracedCallback signature for packet transmission or reception events.
   *
   * @param [in] The CCNx L3 reporting the trace
   * @param [in] packet The packet.
   * @param [in] ccnxPacket The CCNx Packet
   * @param [in] interface
   */
  typedef void (* TxRxTracedCallback) (const CCNxL3Protocol *ccnx, Ptr<const Packet> packet, Ptr<const CCNxPacket> ccnxPacket, uint32_t interface);

  /**
   * TracedCallback signature for packet drop events.
   *
   * @param [in] The CCNx L3 reporting the trace
   * @param [in] header The Ipv4Header.
   * @param [in] packet The packet.
   * @param [in] ccnxPacket The CCNx Packet
   * @param [in] interface
   * @param [in] reason The reason the packet was dropped.
   */
  typedef void (* DropTracedCallback) (const CCNxL3Protocol *ccnx, Ptr<const Packet> packet, Ptr<const CCNxPacket> ccnxPacket, uint32_t interface, DropReason reason);
};

} // namespace ccnx
} // namespace ns3


#endif //CCNS3_CCNXL3PROTOCOL_H
