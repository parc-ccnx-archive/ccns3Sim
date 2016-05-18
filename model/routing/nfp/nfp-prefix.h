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

#ifndef CCNX_MODEL_ROUTING_NFP_NFP_PREFIX_H_
#define CCNX_MODEL_ROUTING_NFP_NFP_PREFIX_H_

#include <map>
#include <queue>

#include "ns3/simple-ref-count.h"
#include "ns3/nstime.h"
#include "ns3/nfp-anchor-advertisement.h"

#include "ns3/nfp-advertise.h"
#include "ns3/nfp-withdraw.h"
#include "ns3/ccnx-connection.h"

#include "ns3/ccnx-route.h"
#include "ns3/nfp-computation-cost.h"

namespace ns3 {
namespace ccnx {

/**
 * Stores state related to a prefix.
 *
 * Multiple anchors can advertise the same prefix.  This allows us to multi-path for
 * any given prefix.  For each (anchor, prefix) name, we only admit route advertisements
 * that are in-order (higher seqnum, or for same seqnum no greater distance).  But between
 * anchors all routes are feasible.  However, we can only insert routes in the FIB that have
 * equal distance, otherwise two routes could bounce back and forth if they chose different
 * anchors to route on.
 *
 * The prefix table also tracks the last time an advertisement was sent for each anchor
 * for this prefix.  This allows the routing protocol to read off times from the heap
 * so it can send advertisements at the right interval.
 */
class NfpPrefix : public SimpleRefCount<NfpPrefix>
{
public:
  /**
   * The callback signature in NfpRoutingProtocol that gets notified whenever an
   * anchor's state changes for this prefix.  A state is Reachable and Unreachable.
   */
  typedef Callback< void, Ptr<const CCNxName>, Ptr<const CCNxName> > PrefixStateChangeCallback;

  /**
   * Creates a new NfpPrefix data structure for the given prefix.
   *
   * All advertisements share a common timeout.  If an advertisement has not been updated within the
   * `advertisementTimeout` it is considered stale and not a valid path.
   *
   * @param [in] prefix The prefix of this object
   * @param [in] advertisementTimeout The timeout for an advertisement if it has not been updated
   * @param [in] stateChangeCallback The callback to notify whenever an anchor changes state
   * @return A new NfpPrefix object
   */
  NfpPrefix (Ptr<const CCNxName> prefix, Time advertisementTimeout, PrefixStateChangeCallback stateChangeCallback);

  /**
   * Destroys an NfpPrefix, releasing all stored pointer references
   */
  virtual ~NfpPrefix ();

  /**
   * Returns the prefix name of this object
   *
   * @return The prefix name
   */
  Ptr<const CCNxName> GetPrefix (void) const;

  typedef enum {
    Advertisement_ImprovedRoute,	/*< The advertisement was strictly better than what we had before */
    Advertisement_EqualRoute,		/*< The advertisement is equal to what is currently in table (multipath) */
    Advertisement_NotFeasible,		/*< The advertisement is not feasible (worse than what we have) */
  } AdvertisementResult;

  /**
   * Receives an advertisement.
   *
   * PRECONDITION: The advertisement prefix must match this objects prefix, assert otherwise.
   *
   * Will create an new anchor advertisement entry for a new anchor.  Otherwise, will update
   * an existing entry if the new advertisement is feasible.
   *
   * @param advertisement
   * @param ingressConnection The nexthop connection id
   * @param now The current time
   * @return Advertisement_ImprovedRoute If the advertisement is strictly better than current route
   * @return Advertisement_EqualRoute If is feasible, but not strictly better
   * @return Advertisement_NotFeasible If not feasible (worse than current)
   */
  AdvertisementResult ReceiveAdvertisement (Ptr<NfpAdvertise> advertisement, Ptr<CCNxConnection> ingressConnection, Time now);

  /**
   * Removes the neighbor from the nexthop list of a particular {anchor, prefix} pair.
   *
   * PRECONDITION: The caller has ensured that the withdraw message is in-order from the neighbor
   * represented by ingressConnection.
   *
   * @param withdraw
   * @param ingressConnection
   * @param now The current time
   * @return
   */
  bool ReceiveWithdraw (Ptr<NfpWithdraw> withdraw, Ptr<CCNxConnection> ingressConnection, Time now);

  /**
   * Remove a connection id from all routes.
   *
   * @param [in] connection The connection id to remove from all routes.
   */
  void RemoveConnection (Ptr<CCNxConnection> connection);

  /**
   * Determines if we can reach the prefix.
   *
   * To be reachable, we must know of at least one anchor with at least one nexthop
   *
   * @return True if reachable, false if not reachable
   */
  bool IsReachable (void);

  /**
   * Determines if the prefix is reachable via the given anchor (i.e. the anchor
   * has at least one nexthop).
   *
   * @param anchorName
   * @return true if prefix is reachable at the given anchor
   */
  bool IsReachable (Ptr<const CCNxName> anchorName);

  /**
   * Returns the best advertisement we have heard from the named anchor
   *
   * PRECONDITION: The anchor IsReachable() for the prefix.
   *
   * @param anchorName
   * @return The best advertisement we have heard from the named anchor.
   */
  Ptr<const NfpAdvertise> GetAdvertisement (Ptr<const CCNxName> anchorName) const;

  typedef std::set< uint32_t > EgressListType;

  /**
   * Returns the set of next hops for the prefix.  This is the union of all the
   * possible next hops for all anchors.  This is the image of what the FIB entry should
   * look like for the prefix.
   *
   * @return The route for this prefix.
   */
  Ptr<CCNxRoute> GetRoute (void);

  /**
   * Dump the state of this prefix entry and all AnchorAdvertisements it contains.
   *
   * We define this as a friend method to avoid having to expose internal state via public functions
   *
   * @param os The stream to write to.
   * @return The `os`.
   */
  friend std::ostream & operator << (std::ostream &os, const NfpPrefix &prefix);

  NfpComputationCost GetComputationCost() const;

protected:
  /**
   * The prefix this object represents.
   */
  Ptr<const CCNxName> m_prefix;

  Time m_advertisementTimeout;

  PrefixStateChangeCallback m_stateChangeCallback;

  /**
   * Map from AnchorName -> AnchorAdvertisement
   */
  typedef std::map< Ptr<const CCNxName>, Ptr<NfpAnchorAdvertisement>, CCNxName::isLessPtrCCNxName > AnchorMapType;

  AnchorMapType m_anchors;

  NfpComputationCost m_computationCost;
};
}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNX_MODEL_ROUTING_NFP_NFP_PREFIX_H_ */
