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

#ifndef CCNS3_CCNXSTACKHELPER_H
#define CCNS3_CCNXSTACKHELPER_H

#include <vector>
#include <list>

#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/object-factory.h"
#include "ns3/ccnx-routing-helper.h"
#include "ns3/ccnx-ascii-trace-helper.h"
#include "ns3/ccnx-forwarding-helper.h"
#include "ns3/ccnx-layer3-helper.h"
#include "ns3/ccnx-l3-protocol.h"

namespace ns3 {

class Node;

namespace ccnx {

/**
 * @ingroup ccnx
 *
 *
 * @subsection ccnxTracingModel Tracing in CCNx
 *
 * NOTE: Tracing is in the process of being implemented, it is not ready to go yet.
 *
 * In this helpers directory, there are several tracing helpers, such as CCNxAsciiTraceHelper.  Those helpers
 * will call appropriate derived methods in this class to set up tracing.
 *
 * The implementation of CCNxL3Protocol, for example `CCNxStandardLayer3`, defines three tracing
 * attributes in its GetTypeId: "Tx" (ns3::ccnx::CCNxStandardLayer3::m_txTrace),
 * "Rx" (ns3::ccnx::CCNxStandardLayer3::m_rxTrace), and "Drop" (ns3::ccnx::CCNxStandardLayer3::m_dropTrace).
 *
 * The "Tx" trace is fired whenever a packet is sent out a connection.
 * The "Rx" trace is fired whenever a packet is received in a connection.
 * The "Drop" trace is fired if Layer 3 cannot handle the packet, such as it has an expired TTL
 * or there's no FIB entry saying where to send it.
 *
 */
class CCNxStackHelper : public Object,
                        public CCNxAsciiTraceHelper
{
public:
  /**
   */
  CCNxStackHelper (void);

  /**
   * Destroy the CCNxStackHelper
   */
  virtual ~CCNxStackHelper (void);

  /**
   * @brief Copy constructor
   */
  CCNxStackHelper (const CCNxStackHelper &);

  /**
   * @brief Copy constructor
   */
  CCNxStackHelper &operator= (const CCNxStackHelper &o);

  /**
   * Return helper internal state to that of a newly constructed one
   */
  void Reset (void);

  /**
   * Aggregate implementations of the ns3::ccnx::CCnxL3Protocol
   * onto the provided node.  This method will assert if called on a node that
   * already has an CCNx object aggregated to it.
   *
   * @param nodeName The name of the node on which to install the stack.
   */
  void Install (std::string nodeName) const;

  /**
   * @param node The node on which to install the stack.
   */
  void Install (Ptr<Node> node) const;

  /**
   * For each node in the input container, aggregate implementations of the
   * CCNxL3Protocol.  The program will assert
   * if this method is called on a container with a node that already has
   * an CCNx object aggregated to it.
   *
   * \param c NodeContainer that holds the set of nodes on which to install the
   * new stacks.
   */
  void Install (NodeContainer c) const;

  /**
   * Aggregate CCNxL3Protocol stacks to all nodes in the simulation
   */
  void InstallAll (void) const;

  /**
   * \brief Enable/disable CCNx stack install.
   * \param enable enable state
   */
  void SetCCNxStackInstall (bool enable);

  /**
   * Assign all the devices in the device container to the CCNxL3Protocol in
   * on the respective nodes.
   */
  void AddInterfaces (const NetDeviceContainer &c);

  /**
   * Set a routing helper and this routing protocol will be installed on nodes
   * when Install is called.  You should set all attributes of the routing protocol
   * prior to calling Install.
   */
  void SetRoutingHelper (const CCNxRoutingHelper & routingHelper);

  /**
   * Specify a layer 3 helper to install on a node.
   *
   * If this is not set, it will default to `CCNxStandardLayer3Helper`.
   *
   * @param layer3Helper The layer 3 helper to use.
   */
  void SetLayer3Helper (const CCNxLayer3Helper & layer3Helper);

  /**
   * Specify a Forwarding helper to install on a node.
   *
   * If this is not set, it will default to `CCNxStandardForwarderHelper`.
   *
   * @param forwardingHelper The helper to install on a node.
   */
  void SetForwardingHelper (const CCNxForwardingHelper & forwardingHelper);

protected:
  // Derived from CCNxAsciiTraceHelper class
  virtual void EnableAsciiCCNxInternal (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> namePrefix, Ptr<Node> node);

private:
  /**
   * A data container for trace filters
   */
  class TraceFilter
  {
public:
    TraceFilter (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> namePrefix);

    bool Match (Ptr<const CCNxName> name) const;

    Ptr<OutputStreamWrapper> GetStream (void) const;

private:
    Ptr<OutputStreamWrapper> m_stream;
    Ptr<CCNxName> m_namePrefix;
  };

  /**
   * We index the trace filters by node id in an array.
   */
  typedef std::list< TraceFilter > TraceFilterListType;
  typedef std::vector< TraceFilterListType * > TraceFilterVectorType;
  TraceFilterVectorType m_traceFilters;

  /**
   * \brief Initialize the helper to its default values
   */
  void Initialize (void);

  /**
   * \brief create an object from its TypeId and aggregates it to the node
   * \param node the node
   * \param typeId the object TypeId
   */
  static void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId);

  /**
   * \brief CCNx install state (enabled/disabled) ?
   */
  bool m_ccnxEnabled;

  /**
   * The default forwarding helper to use if not set by `SetForwardingHelper()`.
   */
  CCNxForwardingHelper * m_defaultForwardingHelper;

  /**
   * The default layer 3 helper to use if not set by `SetLayer3Helper()`.
   */
  CCNxLayer3Helper * m_defaultLayer3Helper;

  const CCNxRoutingHelper * m_routingHelper;
  const CCNxForwardingHelper * m_forwardingHelper;
  const CCNxLayer3Helper * m_layer3Helper;

  /**
   * This function is set as the m_txTrace and m_rxTrace of CCNxL3Protocol.
   *
   * @param context A context string added by the call to Config::Connect()
   * @param ccnx
   * @param ns3Packet
   * @param ccnxPacket
   * @param interface
   */
  void AsciiTxTraceCallback (std::string context, const CCNxL3Protocol *ccnx, Ptr<const Packet> ns3Packet,
                             Ptr<const CCNxPacket> ccnxPacket, uint32_t interface);
  void AsciiRxTraceCallback (std::string context, const CCNxL3Protocol *ccnx, Ptr<const Packet> ns3Packet,
                             Ptr<const CCNxPacket> ccnxPacket, uint32_t interface);
  void AsciiDropTraceCallback (std::string context, const CCNxL3Protocol *ccnx, Ptr<const Packet> ns3Packet,
                               Ptr<const CCNxPacket> ccnxPacket, uint32_t interface, CCNxL3Protocol::DropReason reason);
};

}         // namespace ccnx
} // namespace ns3


#endif //CCNS3_CCNXSTACKHELPER_H
