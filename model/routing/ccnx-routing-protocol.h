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

#ifndef CCNS3_CCNXROUTINGPROTOCOL_H
#define CCNS3_CCNXROUTINGPROTOCOL_H

#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-routing-error.h"

namespace ns3 {
namespace ccnx {

class CCNxL3Protocol;

/**
 * @defgroup ccnx-routing CCNx Routing Models
 * @ingroup ccnx
 */

/**
 * @ingroup ccnx-routing
 *
 * Abstract base class for CCNx Routing protocol implementations
 *
 * Unlike the NS3 IP model, there is a separation of routing from forwarding.
 * A routing model generally only needs to open one or more CCNxPortals.  It
 * configures a CCNxForwarder by looking up the CCNxForwarder implementaiton bound
 * to the node and calling those base class methods.
 */
class CCNxRoutingProtocol : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();

  /**
   * \param interface the index of the interface we are being notified about
   *
   * Protocols are expected to implement this method to be notified of the state change of
   * an interface in a node.
   */
  virtual void NotifyInterfaceUp (uint32_t interface) = 0;

  /**
   * \param interface the index of the interface we are being notified about
   *
   * Protocols are expected to implement this method to be notified of the state change of
   * an interface in a node.
   */
  virtual void NotifyInterfaceDown (uint32_t interface) = 0;

  /**
   * \param node the Node object this routing protocol is being associated with
   *
   * Typically, invoked directly or indirectly from ns3::ccnx::SetRoutingProtocol
   */
  virtual void SetNode (Ptr<Node> node) = 0;

  /**
   * Inform the routing protocol to begin advertising a prefix.
   *
   * May be called multiple times by different producers on the node.  An Add count is kept.
   */
  virtual void AddAnchorPrefix (Ptr<const CCNxName> prefix) = 0;

  /**
   * Inform the routing protocol to stop advertising a prefix.
   *
   * May be called multiple times by different producers on the node.  An Add count is kept.
   * The prefix is removed from the anchor list after the same number of Removes as Adds are called.
   */
  virtual void RemoveAnchorPrefix (Ptr<const CCNxName> prefix) = 0;

  /**
   * Assign a number random number stream to the protocol.
   *
   * \return The number of streams assigned
   *
   * Example inside a RoutingProtocol called by this function:
   *
   * int64_t
   * RoutingProtocol::AssignStreams (int64_t stream)
   * {
   *    NS_LOG_FUNCTION (this << stream);
   *    m_uniformRandomVariable->SetStream (stream);
   *    return 1;
   * }
   */
  virtual int64_t AssignStreams (int64_t stream) = 0;

  /**
   * Display the current set of 1-hop neighbors
   *
   * @param stream The output stream to write to
   */
  virtual void PrintNeighborTable (Ptr<OutputStreamWrapper> stream) const = 0;

  /**
   * \brief Print the Routing Information Base
   *
   * The RIB is the routing database maintained by the routing protocol, which is
   * different than the simple Forwarding Information Base (FIB) maintained by
   * the forwarder.
   *
   * \param stream the ostream the Routing table is printed to
   */
  virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const = 0;

  /**
   * \brief Print the set of Anchor names being advertised by this protocol
   */
  virtual void PrintAnchorPrefixes (Ptr<OutputStreamWrapper> stream) const = 0;

};
}
}

#endif //CCNS3_CCNXROUTINGPROTOCOL_H
