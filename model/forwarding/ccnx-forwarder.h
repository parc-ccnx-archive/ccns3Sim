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

#ifndef CCNS3_FORWARDER_H
#define CCNS3_FORWARDER_H

#include "ns3/object.h"
#include "ns3/net-device.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/ccnx-routing-error.h"
#include "ns3/ccnx-connection.h"
#include "ns3/ccnx-route.h"
#include "ns3/callback.h"
#include "ns3/ccnx-connection-list.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {
namespace ccnx {

class CCNxL3Protocol;

/**
 * @defgroup ccnx-forwarder CCNx Forwarder model
 * @ingroup ccnx
 */

/**
 * @ingroup ccnx-forwarder
 *
 * This is the abstract base class of a CCNx Forwarder.  The forwarder is responsible for
 * handling requests to `RouteIn()` and `RouteOut()` to resolve a CCNxConnection for forwarding
 * a packet.
 *
 * To accurately model delays, the CCNxForwarder API is an asynchronous message passing API.
 * This allows the forwarder and its components (PIT, FIB, CS) to introduce arbitrary processing delays
 * on all messages.
 *
 * The user must set the RouteCallback function via `SetRouteCallback()`.  Otherwise the user will never
 * get a result out of the forwarder.
 *
 * It also offers an API for managing the entries in the Forwarding Information Base (FIB).
 *
 * This API is used by CCNxL3Protocol and by routing protocols.  A routing protocol can
 * resolve the forwarder running on a node using:
 *
 * @code
 * {
 *    Ptr<CCNxForwarder> forwarder = node->GetObject<CCNxForwarder>()
 * }
 * @endcode
 *
 * CCNxStandardForwarder is the RFC-compliant implementation, based on work in the IRTF ICNRG work group.
 */
class CCNxForwarder : public Object
{
public:
  static TypeId GetTypeId ();

  CCNxForwarder ();
  virtual ~CCNxForwarder ();

  /**
   * The `RouteCallback` is an asynchronous callback after the forwarder has decided how to route
   * a packet.  It is called as the return from `RouteIn()` and `RouteOut()`.
   *
   * The forwarder should ensure there is always one call to the callback for each call to `RouteIn()` and `RouteOut()`
   *
   * @param Ptr<CCNxPacket> The packet being routed
   * @param Ptr<CCNxConnection> The ingress connection
   * @param RoutingErrno The error encounted (or NoError)
   * @param ConnectionsList The egress connections to forward on (may be empty)
   */
  typedef Callback<void, Ptr<CCNxPacket>, Ptr<CCNxConnection>, enum CCNxRoutingError::RoutingErrno, Ptr<CCNxConnectionList> > RouteCallback;

  /**
   * Query for list of next hops   - outbound packet (from L4)
   *
   * This is an asynchronous call.  It will return immediately.  At some later time (after processing delay), the
   * forwarder will return the packet via the RouteCallback.
   *
   * @param packet [in] The CCNxPacket to route for output
   * @param ingressConnection [in] The source CCNxConnection
   * @param egressConnection [in] The CCNxConnection to send it out on (may be Ptr<CCNxConnection>(0))
   */
  virtual void RouteOutput (Ptr<CCNxPacket> packet,
                            Ptr<CCNxConnection> ingressConnection,
                            Ptr<CCNxConnection> egressConnection) = 0;

  /**
   *  Query for list of next hops  - inbound packet (from L2)
   *
   * This is an asynchronous call.  It will return immediately.  At some later time (after processing delay), the
   * forwarder will return the packet via the RouteCallback.
   *
   * @param packet [in] The CCNxPacket to route for output
   * @param ingressConnection [in] The source CCNxConnection
   */
  virtual void RouteInput (Ptr<CCNxPacket> packet,
                           Ptr<CCNxConnection> ingressConnection) = 0;

  /**
   * Configures the forwarder with the callback to use for the result of
   * `RouteIn()` and `RouteOut()`.
   *
   * @param callback [in] The callback for the return of a RouteIn or RouteOut operation.
   */
  virtual void SetRouteCallback (RouteCallback callback);

  /**
   * @deprecated Use AddRoute(Ptr<const CCNxRoute>)
   *  Add Route to FIB using connection and name.
   *
   * \param connection Pointer to connection object.
   * \param name Pointer to name object.
   * \return true if route added, false otherwise
   */
  virtual bool AddRoute (Ptr<CCNxConnection> conn, Ptr<const CCNxName> name) = 0;

  /**
   * @deprecated Use RemoveRoute(Ptr<const CCNxRoute>)
   *  Remove Route to FIB using connection and name.
   *
   * \param connection Pointer to connection object.
   * \param name Pointer to name object.
   * \return true if route removed, false otherwise
   */
  virtual bool RemoveRoute (Ptr<CCNxConnection> conn, Ptr<const CCNxName> name) = 0;

  /**
   * For each of the CCNxRouteEntry in the CCNxRoute, add it to the FIB.  If a similar
   * FIB entry (prefix, connectionId) already exists, it is replaced (the cost updated).
   *
   * @param [in] route The routes to add to the FIB
   * @return true of one or more CCNxRouteEntrys were added to the FIB.
   * @return false If none of the CCNxRouteEntrys were added to the FIB.
   */
  virtual bool AddRoute (Ptr<const CCNxRoute> route) = 0;

  /**
   * For each of the CCNxRouteEntry in the CCNxRoute, remove it from the FIB.
   * A matching FIB entry as the same (prefix, connectionId).  The cost is ignored.
   *
   * @param [in] route The routes to remove from the FIB
   * @return true of one or more CCNxRouteEntrys were removed to the FIB.
   * @return false If none of the CCNxRouteEntrys existed in the FIB.
   */
  virtual bool RemoveRoute (Ptr<const CCNxRoute> route) = 0;

  /**
   * Dump the node's forwarding table (FIB) to the given output stream.  The format of the FIB is
   * particular to each instance of a CCNxForwarder.
   *
   * @param streamWrapper [in] The stream to write the FIB to
   */
  virtual void PrintForwardingTable (Ptr<OutputStreamWrapper> streamWrapper) const = 0;

  /**
   * \brief Print the forwarding statistics
   *
   * The exact output will vary from implementation to implementation.  In general, this
   * should print calls to `RouteIn()` and `RouteOut()` and the outcome, such as routes matches or
   * no route.
   *
   * The forwarder should also includes statistics from its principle tables, such as
   * the PIT, FIB, and CS, as appropriate.
   *
   * @param streamWrapper [in] the ostream the forwarding statistics is printed to
   */
  virtual void PrintForwardingStatistics (Ptr<OutputStreamWrapper> streamWrapper) const = 0;


  // =======

  /**
   * Sets the node on which this forwarder sits.
   *
   * Will also set m_ccnx by looking up type TypeId "ns3::ccnx::CCNxL3Protocol".
   */
  void SetNode (Ptr<Node> node);

protected:
  Ptr<CCNxL3Protocol> m_ccnx;
  Ptr<Node> m_node;
  RouteCallback m_routeCallback;
};

}     // namespace ccnx
} // namespace ns3


#endif //CCNS3_FORWARDER_H
