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

#ifndef CCNS3SIM_NAMEFLOODINGPROTOCOL_H
#define CCNS3SIM_NAMEFLOODINGPROTOCOL_H

#include <list>
#include <map>

#include "ns3/ccnx-l3-protocol.h"
#include "ns3/ccnx-routing-protocol.h"
#include "ns3/nstime.h"
#include "ns3/ccnx-portal.h"
#include "ns3/nfp-payload.h"
#include "ns3/nfp-neighbor.h"
#include "ns3/nfp-neighbor-key.h"

#include "ns3/nfp-prefix.h"
#include "ns3/nfp-workqueue.h"

#include "ns3/timer.h"
#include "ns3/random-variable-stream.h"
#include "ns3/nfp-prefix-timer-heap.h"

#include "ns3/nfp-computation-cost.h"
#include "ns3/nfp-stats.h"

// For unit tests
class NfpRoutingProtocolFriend;

namespace ns3 {
namespace ccnx {

/**
 * \defgroup nfp-routing Name Flooding Protocol Routing for CCNx
 * \ingroup ccnx-routing
 *
 * The Name Flooding Protocol (NFP) is a distance vector routing protocol for CCNx names.
 * It is implemented in the class NfpRoutingProtocol that inherits from CCNxRoutingProtocol.
 *
 * NFP advertises (prefix, anchorName, anchorPrefixSeqnum, distance).  Each (prefix, anchorName) pair is considered independently from
 * other advertisements.  This means that NFP maintains routes to all anchors that advertise a specific name prefix.
 * Each anchor is responsible for incrementing its own anchorPrefixSeqnum with each new advertisement.
 * As the advertisement travels over the network, its distance is always increasing with each hop.  We do not
 * require "1" link costs, it could be any positive distance per hop.
 *
 * An advertisement for (prefix, anchorName) is considered feasible if the anchorPrefixSeqnum is greater than
 * any previously seen sequence number, or anchorPrefixSeqnum being equal to a previous advertisement the
 * distance is no greater than any previous advertisement seen at the node.
 *
 * - Each node is an anchor for one or more name prefixes.  These are set via the configuration file.
 *
 * - Periodically (e.g. every 5 seconds) the node advertises those prefixes along with a monotonically increasing
 *   sequence number.
 *
 * - A route advertisement is feasible at a node if (a) the sequence number is larger than stored, or (b) the sequence
 *   number is equal to store and the hop count is not less than stored.  This allows equal cost multipath.
 *
 * - Each link (neighbor adjacency) has a positive cost.  We use "1" for all link costs.
 *
 * - Each node has a unique name, for example a cryptographic hash of a public key.  It is set in the configuration file.
 *   If it is not set, it will default to ccnx:/name=nfp/name=rtr/name=<nodeid>.
 *
 * - If a node loses reachability for a route, it may immediately send a RouteWithdraw message to its neighbors.  It
 *   should continue to send a RouteWithdraw in each subsequent advertisement for a few times.
 *
 * - Each route will timeout at 3x advertisement period.
 *
 * - Each message has a 1-hop MessageSeqnum specific to that 1-hop message.  A message is only accepted if
 *   (MyName, MessageSeqnum) is greater than the previously heard (MyName, MessageSeqnum).
 *
 * - A RouteAdvertisement message is also an implicit Hello from that neighbor.  It may be sent with no advertisements
 *   simply as a Hello.
 *
 * - All messages are sent as an Interest with payload.  The advertisement name will be either
 * @code
 *         ccnx:/name=nfp/name=adv
 *         ccnx:/name=nfp/name=withdraw
 * @endcode
 *
 * - The router will also register its name with the local forwarder.
 * @code
 *         ccnx:/name=nfp/name=rtr/name=<nodeid>
 * @endcode
 *
 */

/**
 * \ingroup nfp-routing
 *
 * A simple distance vector routing protocol based on flooding name advertisements.
 * Should be free of permanent loops even with topology changes.  But, its convergence time
 * is to too fast because it is based on timers.
 *
 * See nfp/README.md for a protocol description.
 *
 * @section tables Tables
 *
 * @subsection neighbor-table Neighbor Table
 *      The neighbor table tracks Hello status for each neighbor.  It has a callback to NeighborStateChanged() whenever
 *      the state of a neighbor changes (UP, DOWN, DEAD).  We use a 3-state model for neighbors so we do not erase their
 *      messageSeqnum right away when they go DOWN.  They have to timeout a second time to DEAD state before we erase the record.
 *      If a neighbor goes to DOWN state, we erase all next hops in m_prefixes that use that neighbor.  That may cause PrefixStateChanged
 *      to be called if a (prefix, anchorName) becomes unreachable.  On a DEAD change, we simply erase the record from m_neighbors.
 *
 *      m_neighbors   : CCNxName -> NfpNeighbor  (routerName -> neighbor entry)
 *
 * @subsection prefix-table Prefix Table
 *      The prefix table stores the individual best advertisements we have seen for each (prefix, anchorName) pair.
 *      It has a callback to PrefixStateChanged() when a (prefix, anchorName) becomes reachable or unreachable.  There currently is
 *      not a method to prune DEAD records like there is for neighbors.  Whenever we get a PrefixStateChanged, we will
 *      create an NfpWorkQueue entry for the (prefix, anchorName).  The work queue is serviced by timers or some specific events
 *      (see WorkQueue section below).
 *
 *      m_prefixes    : CCNxName -> NfpPrefix    (prefixName -> prefix entry)
 *
 * @subsection model-fib Model FIB
 *      The model FIB keeps track of all the state we have injected into the FIB, organized by prefix.  When we need
 *      to make an update, we can get the current route out of the RIB and compare it to the model FIB.  We then
 *      add the updates from the RIB to the FIB and remove any stale routes from the FIB not in the RIB.  We do it
 *      in that order so there's always a route in the FIB, if possible.
 *
 *      m_modelFib    : CCNxName -> CCNxRoute    (prefixName -> CCNxRoute)
 *
 * @subsection anchor-name-table Anchor Names Table
 *      This anchor table is our node's anchor names, i.e. what we advertise.
 *      We put entries in m_prefixes for m_anchorNames
 *
 *      m_anchorNames : CCNxName -> uint32_t     (local anchor name -> refcount)
 *
 * @subsection link-cost-table Link Cost Table
 *      The link cost table should be interface # (GetIfIndex()), but we only store
 *      connection IDs.  We'd need to lookup the connection, then query for its interface number...
 *      This needs some re-thinking.
 *
 *      This table is currently not used and we only use a cost of "1"
 *
 *      m_linkcosts   : uint32_t -> NfpLinkCost  (??? -> link cost)
 *
 * @subsection prefix-timer-heap Prefix Timer Heap
 *      The heap tracks the expiry time of each (prefix, anchorName) pair.  We insert a record into the heap
 *      every time we advertise a (prefix, anchorName) with the new expiry time of the record.  We can then
 *      Peek() and Pop() from the top of the heap, which is the next timer that will expire.
 *
 *      m_prefixTimerHeap : NfpPrefixTimerHeap
 *
 * @section work-queue Work Queue
 *     The work queue tracks (anchorname, prefix) pairs.  When something is inserted into the work queue,
 *     it is a book mark for the routing protocol that it has to do something with that pair.
 *
 *     The work queue stores entries in FIFO order and a given (anchorName, prefix) pair can only
 *     be in the queue once.  If it is added multiple times, only the first one stays (and stays at its original place
 *     in the queue).
 *
 *     After each advertisement timer or anchor timer, we process the work queue, which will generate actual
 *     packets and broadcast them.  The work queue is also processed on some trigger events, like learning about
 *     a new route or receiving a route withdraw so we propagae those events immediately.
 *
 * @section Timers
 *	All timers are set via the `SetTimer()` function, which uses `CalculateJitteredTime()`.  Jittering will fire a timer
 *	in the period [interval - jitter, interval), so all timers at at most their prescribed interval.
 *
 * @subsection hello-timer Hello Timer
 *      All NFP messages count has 'hello' messages.  They contain our router name and our message sequence number.
 *      Only if we have not sent any other NFP messages will we send an empty 'hello' message when this timer expires.
 *
 * @subsection anchor-route-timer Anchor Route Timer
 *      The anchor route timer fires every m_routeTimeout.
 *
 * @subsection adv-timer Advertise Timer
 *
 *
 * Notes:
 * - If a neighbor goes down, we need to scan m_prefixes to find any entries that use the
 *   neighbor as a nexthop and remove them.  There's no quick way to do this, as we don't maintain
 *   a separate list in m_neighbors.
 *
 * @see NfpPrefixTimerHeap, NfpPrefix, NfpNeighbor, NfpWorkQueue
 */
class NfpRoutingProtocol : public CCNxRoutingProtocol
{
  friend class ::NfpRoutingProtocolFriend;

public:
  static TypeId GetTypeId ();

  // ========================================
  // NameFloodingProtocol specific calls

  NfpRoutingProtocol ();
  virtual ~NfpRoutingProtocol ();

  /**
  * Set the name by which the router is known on the network.
  */
  void SetRouterName (Ptr<const CCNxName> name);

  // ========================================
  // === Inherited from CCNxRoutingProtocol

  virtual void NotifyInterfaceUp (uint32_t interface);

  virtual void NotifyInterfaceDown (uint32_t interface);

  virtual void SetNode (Ptr<Node> node);

  virtual void PrintNeighborTable (Ptr<OutputStreamWrapper> streamWrapper) const;

  virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> streamWrapper) const;

  virtual void PrintAnchorPrefixes (Ptr<OutputStreamWrapper> streamWrapper) const;

  virtual void AddAnchorPrefix (Ptr<const CCNxName> prefix);

  virtual void RemoveAnchorPrefix (Ptr<const CCNxName> prefix);

  virtual int64_t AssignStreams (int64_t stream);

  void PrintRoutingStats (Ptr<OutputStreamWrapper> streamWrapper) const;

  // ========================================
  // === Getters for parameters

  /**
   * Return the HelloInterval
   */
  Time GetHelloInterval (void) const;

  /**
   * Return the AdvertiseInterval
   */
  Time GetAdvertiseInterval (void) const;

  /**
   * Return the Jitter subtracted from each Interval
   */
  Time GetJitter (void) const;

  /**
   * Return the timeout of a route entry (when it is deleted)
   */
  Time GetRouteTimeout (void) const;

  /**
   * Return the timeout of a neighbor (when all its routes are removed)
   */
  Time GetNeighborTimeout (void) const;

  NfpComputationCost GetComputationCost() const;

  void PrintComputationCost (Ptr<OutputStreamWrapper> streamWrapper) const;

  NfpStats GetStats() const;

protected:
  /**
   * The storage type for Anchor Prefixes.
   */
  typedef std::map< Ptr<const CCNxName>, uint32_t > AnchorNameMapType;

  /**
   * To inspect anchor names, you can work with their Iterator
   */
  AnchorNameMapType::const_iterator GetAnchorPrefixIterator (void) const;

  /**
   * Called when the protocol should start running
   */
  virtual void DoInitialize (void);

  /**
   * Called when the protocol should stop running
   */
  virtual void DoDispose (void);

  /**
   * The node associated with this routing protocol.
   */
  Ptr<Node> m_node;

  /**
   * Reference to the CCNxL3Protocol on this node.
   *
   * Used mostly for getting information about interfaces and their
   * broadcast connections.
   */
  Ptr<CCNxL3Protocol> m_ccnx;

  /**
   * The portal on which we do all our I/O
   */
  Ptr<CCNxPortal> m_portal;

  /**
   * The portal callback when the portal has a packet for us.
   *
   * @param portal The portal to read.
   */
  void ReceiveNotify (Ptr<CCNxPortal> portal);

  /**
    * Receive an NFP Payload (which may contain multiple NfpMessages).
    *
    * Called from ReceiveNotify() as it processes in-bound packet.
    *
    * The ingressConnecitonId is used as the nexthop address for the neighbor.
    *
    * @param payload The NfpPayload from a neighbor.
    * @param ingressConnection The L3 connection ID that points to this neighbor.
    */
  void ReceivePayload (Ptr<NfpPayload> payload, Ptr<CCNxConnection> ingressConnection);

  /**
   * Called by ReceiveAdvertise to add a new advertisement.
   *
   * This will start an advertisement timer in m_prefixTimerHeap and make a workqueue item for it.
   *
   * @param advertise
   * @param ingressConnection
   */
  void AddAdvertise (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection);

  /**
   * Receive an NFP Advertisement.
   *
   * Called from ReceivePayload().  ReceivePayload will fiter out advertisements with our router name, just to save work.
   * It is ok if we ReceiveAdvertise() with our anchor name because they will go through the same feasibilty test as anything
   * else and should fail becasue we have a 0 distnace to things we anchor.
   *
   * The ingressConnecitonId is used as the nexthop address for the neighbor.
   *
   * @param advertise The advertisement from a neighbor.
   * @param ingressConnection The L3 connection ID that points to this neighbor.
   */
  void ReceiveAdvertise (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection);

  /**
    * Receive an NFP Withdraw.
    *
    * Called from ReceivePayload().
    *
    * The ingressConnecitonId is used as the nexthop address for the neighbor.
    *
    * @param withdraw The withdraw from a neighbor.
    * @param ingressConnection The L3 connection ID that points to this neighbor.
    */
  void ReceiveWithdraw (Ptr<NfpWithdraw> withdraw, Ptr<CCNxConnection> ingressConnection);

  /**
   * Receive a Hello from a neighbor.
   *
   * Add or update the neighbor table.  If an entry already exists, make sure the msgSeqnum
   * is in-order (greater than previous), otherwise drop.
   *
   * Then, update the Expiry time (now + m_neighborTimeout).  Update the Connection number
   * if they changed interfaces.  Update the msgSeqnum to this number (which we know was larger
   * than the prior).
   *
   * @param neighbor The name of the neighbor router
   * @param msgSeqnum The seqnuence number of the message
   * @param ingressConnection The reverse path connection ID (L3 id) of the neighbor.
   *
   * @return true if the Hello was valid (in-order msgSeqnum), false if rejected
   */
  bool ReceiveHello (Ptr<const CCNxName> neighbor, uint16_t msgSeqnum, Ptr<CCNxConnection> ingressConnection);

  /**
   * The name prefix to send and receive Interets on
   *
   * Defaults to "ccnx:/name=nfp".
   *
   * TODO: Make this settable
   */
  Ptr<const CCNxName> m_nfpPrefix;

  /**
   * Used to compute jitters
   */
  Ptr<UniformRandomVariable> m_uniformRandomVariable;

  /**
   * How often do we send a Hello (or a RouteAdvertise).
   *
   * Because all messages counts as a Hello, a Hello will not be sent if we've recently sent
   * some other message.  Basically, the Hello Interval starts counting from the most recently sent
   * message of any type.
   */
  Time m_helloInterval;

  /**
   * How often do we send a RouteAdvertise.
   *
   * Advertise all our reachable routes every m_advertiseInterval.  If all routes do not fit in a single packet
   * individual packets may be paced over the interval to avoid bursts.
   */
  Time m_advertiseInterval;

  /**
   * The amount of time to randomly subtract from the Hello and Advertisement intervals.  For example,
   * if m_helloInterval is 2 seconds and m_jitter is 50 msec, the Hello will be sent betwen 1.95 and 2.00
   * seconds, uniformly distributed each sample.
   */
  Time m_jitter;

  /**
   * If we do not hear from the advertising gateway for this period, any route
   * from that gateway will be removed.
   */
  Time m_routeTimeout;

  /**
   * If we do not hear any message from a neighbor in this period, mark the
   * neighbor as dead and remove all of its routes.
   */
  Time m_neighborTimeout;

  /**
   * My sequence number to advertise my anchor names with.
   */
  uint32_t m_anchorSeqnum;

  /**
   * The name used by this router in the NFP message header.
   *
   * Defaults to "ccnx:/name=<hash>" where hash is a 32-byte string.  We set it
   * as CCNxHashValue(node.GetNodeId());
   */
  Ptr<const CCNxName> m_routerName;

  /**
   * My sequence number to number my 1-hop packets with.
   */
  uint16_t m_messageSeqnum;

  /**
   * The anchor name map is a refcount of the number of times something has called
   * RegisterAnchor().  We only delete it when the refcount is reduced to zero.  This allows
   * multiple CCNxPortals to register the same name.
   */
  AnchorNameMapType m_anchorNames;

  /**
   * The anchor timer is used to inject anchor updates in to the RIB. Bascially, we advertise them
   * to ourself and thus they go in to the RIB and are treated just like any other router update.  This
   * means the process for advertising them is the same as all other routes.
   *
   * With each timer expiry, we increment m_anchorSeqnum and then send all the prefixes with that seqnum.
   * @see m_anchorSeqnum, AnchorRouteTimerExpired()
   */
  Timer m_anchorRouteTimer;

  /**
   * Called when m_anchorRouteTimer expires.
   */
  void AnchorRouteTimerExpired ();

  /**
   * Create a route advertisement for the name with the given sequence number and pass the advertisement
   * to ReceiveAdvertise().
   *
   * PRECONDITION: This node is an anchor for the prefix.  It will be advertised with our m_routerName as
   * the anchor for the prefix.
   *
   * @param anchorName
   * @param anchorSeqnum
   */
  void InjectAnchorRoute (Ptr<const CCNxName> prefix, uint32_t anchorSeqnum);

  /**
   * Generate a Withdraw message for an anchor name and send it to ReceiveWithdraw() to remove
   * prefix we were previously an anchor for.
   * @param prefix
   */
  void InjectAnchorWithdraw (Ptr<const CCNxName> prefix);

  /**
   * The timer to count down until the next Hello packet.
   *
   * @see HelloTimerExpired()
   */
  Timer m_helloTimer;

  /**
   * The time we sent our last broadcast packet.  This is used to determine if we need to send
   * a Hello message when the hello timer expires.  If m_lastBroadcast + m_helloInterval <= GetCurrentTime(),
   * then we need to send a hello packet.
   */
  Time m_lastBroadcast;

  /**
   * Data type used to store the L3 interfaces that have been added to the routing protocol
   * map: InterfaceId -> CCNxL3Interface
   */
  typedef std::map< uint32_t, Ptr<CCNxL3Interface> > InterfaceMapType;

  /**
   * A list of the interfaces that have been added to the routing protocol
   */
  InterfaceMapType m_interfaces;

  /**
   * Neighbors are indexed by the router name
   */
  typedef std::map< Ptr<NfpNeighborKey>, Ptr<NfpNeighbor>, NfpNeighborKey::isLessPtrNfpNeighborKey > NeighborMapType;

  /**
   * Our 1-hop neighbors
   */
  NeighborMapType m_neighbors;

  /**
   * Passed as the callback to NfpNeighbor for when its state changes.
   * UP: We can use the neighbor as a successor
   * DOWN: We should remove all routes through the neighbor
   * DEAD: We should delete the neighbor
   *
   * @param neighborKey the key (name, connid) of the neighbor that expired
   */
  void NeighborStateChanged (Ptr<NfpNeighborKey> neighborKey);

  /**
   * Remove all routes that point through this neighbor
   * @param neighborName The neighbor to remove from the RIB
   */
  void RemoveNeighborRoutes (Ptr<const CCNxName> neighborName, Ptr<CCNxConnection> connection);

  /**
   * Prefix routes are indexed by the prefix name, which then holds state for each anchor
   */
  typedef std::map< Ptr<const CCNxName>, Ptr<NfpPrefix>, CCNxName::isLessPtrCCNxName > PrefixMapType;

  /**
   * Our routing state.  This is also known as the Route Information Base (RIB).
   */
  PrefixMapType m_prefixes;

  /**
   * Timers associated with the PrefixMapType
   */
  NfpPrefixTimerHeap m_prefixTimerHeap;

  /**
   * The type we use to store the model FIB.  The model FIB stores our image of what is in
   * the actual FIB so we know what our diffs look like.
   */
  typedef std::map< Ptr<const CCNxName>, Ptr<CCNxRoute> > ModelFibType;

  /**
   * Stores the model FIB.
   */
  ModelFibType m_modelFib;

  /**
   * Looks up an entry from the Model FIB.  If no entry is found, an empty CCNxRoute is created
   * and inserted in to the Model FIB.
   *
   * @param [in] prefix The prefix to lookup.
   * @return The route for the given prefix.
   */
  Ptr<CCNxRoute> GetModelFibRoute (Ptr<const CCNxName> prefix);

  /**
   * We get a callback from NfpPrefix whenever the state of a prefix changes.
   *
   * @param [in] prefix The prefix name
   * @param [in] anchorName The anchor advertising the prefix
   */
  void PrefixStateChanged (Ptr<const CCNxName> prefix, Ptr<const CCNxName> anchorName);

  /**
   * Called by PrefixStateChanged, will add all the next hops for an anchor to the FIB.
   *
   * @param [in] prefixEntry The prefix record
   * @param [in] anchorName The specific anchor for the prefix
   */
  void AddAnchorToFib (Ptr<NfpPrefix> prefixEntry, Ptr<const CCNxName> anchorName);

  /**
   * Called by PrefixStateChanged, will remove all the next hops for an anchor to the FIB.
   *
   * @param [in] prefixEntry The prefix record
   * @param [in] anchorName The specific anchor for the prefix
   */
  void RemoveAnchorFromFib (Ptr<NfpPrefix> prefixEntry, Ptr<const CCNxName> anchorName);

  /**
   * The work queue keeps a list of the (anchor, prefix) pairs that have recently changed.
   *
   * Any time we do an operation that updates our routing state related to a prefix,
   * we put an entry in the work queue so we know what we need to advertise.  This lets us
   * send triggered updates when there are specific changes in addition to periodic updates.
   */
  NfpWorkQueue m_workQueue;

  /**
   * Triggered on calls to NotifyInterfaceUp() and NotifyInterfaceDown(), we calculate
   * the minimum MTU and cache it in this varaible.  If no interface has been added, the
   * minimumMtu is the maximum for uint32_t.
   */
  uint32_t m_minimumMtu;

  /**
   * Scans the list of interfaces and returns the minimum MTU.  The
   * minimum MTU is used to size all Nfp Payload, taking in to account the
   * overhead of an Interest, so they will not exceed the MTU.
   * @return The minimum MTU of all UP L3 interfaces added to the routing protocol.
   */
  uint32_t GetMinimumMtu (void) const;

  /**
   * Stats about behavior
   */
  NfpStats m_stats;

  /**
   * Callback of Timer when m_helloTimer expires
   */
  void HelloTimerExpired ();

  /**
   * The timer to count down until the next Advertisement.
   */
  Timer m_advertiseTimer;

  /**
   * Callback of Timer when m_advertiseTimer expires.
   */
  void AdvertiseTimerExpired ();

  /**
   * Set a timer to expire in a uniform random period [0, Interval - Jitter].
   */
  void SetTimer (Timer &timer, Time interval, Time jitter);

  /**
   * Jitters the interval by a random time.
   *
   * The output time will be interval - Random(0 to jitter):
   *
   * @param interval
   * @param jitter
   * @return
   */
  Time CalculateJitteredTime (Time interval, Time jitter);

  /**
   * Gets the next sequence number to use in a message header.
   *
   * @return A uint16_t sequence number
   */
  uint16_t GetNextMessageSeqnum (void);

  /**
   * Sets the router name in m_routerName
   */
  void SetRouterName (void);

  /**
   * Creates an empty payload that we can then append messages to.
   * @return An empty NfpPayload (only has the T_MSG header).
   */
  Ptr<NfpPayload> CreatePayload (void);

  /**
   * Create a CCNxPacket (an Interest) from the given NFP payload.  The
   * packet is ready to send.
   *
   * @param payload The NFP payload to send
   * @return An Interest that is ready to send.
   */
  Ptr<CCNxPacket> CreatePacket (Ptr<NfpPayload> payload);

  /**
   * Broad cast a packet out all our interfaces.
   *
   * @param packet The Interest to broadcast.
   */
  void Broadcast (Ptr<CCNxPacket> packet);

  /**
   * Send a payload generated from ProcessWorkQueue.  Creates a packet and calls Broadcast().
   */
  void SendFromWorkQueue (Ptr<NfpPayload> payload);

  /**
   * The given prefix has updated its routing state.  Add it to the work queue so
   * we can send an update quickly without waiting for a timer.
   *
   * @param anchorName The anchor advertising prefix
   * @param prefix The prefix
   */
  void AddWorkQueueEntry (Ptr<const CCNxName> anchorName, Ptr<const CCNxName> prefix);

  /**
   * Go through the work queue and for each prefix advertise it (if reachable)
   * or withdraw it (if not reachable).
   */
  void ProcessWorkQueue (void);

  /**
   * Returns the current time
   *
   * @return The current time
   */
  Time GetCurrentTime (void) const;

  NfpComputationCost m_computationCost;

  /**
   * Whenever there is a change that requires a message to be sent, we queue it and start
   * (if not running) this timer so we send the messages a short delay later.
   */
  Timer m_processWorkQueueTimer;

  /**
   * The callback from `m_processWorkQueueTimer`.
   */
  void ProcessWorkQueueTimerExpired();

  /**
   * Call to set the timer, if it is not already running.  If it is running,
   * this method has no effect.
   */
  void SetProcessWorkQueueTimer();

};
}
}

#endif //CCNS3SIM_NAMEFLOODINGPROTOCOL_H
