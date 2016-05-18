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

#ifndef CCNX_MODEL_ROUTING_NFP_NFP_ANCHOR_ADVERTISEMENT_H_
#define CCNX_MODEL_ROUTING_NFP_NFP_ANCHOR_ADVERTISEMENT_H_

#include <set>
#include <stdint.h>
#include "ns3/simple-ref-count.h"
#include "ns3/nstime.h"
#include "ns3/nfp-advertise.h"
#include "ns3/ccnx-route.h"
#include "ns3/nfp-computation-cost.h"

namespace ns3 {
namespace ccnx {

/**
 * Represents an advertisement by an anchor for a specific prefix.
 *
 * These entries are stored in the prefixes table (NfpPrefix).
 */
class NfpAnchorAdvertisement : public SimpleRefCount<NfpAnchorAdvertisement>
{
public:
  /**
   * Create a new AnchorAdvertisement based on `advertise`.  The nexthop set is
   * initialized to `{ ingressConnection }`.
   *
   * @param advertise The advertisement to start with
   * @param ingressConnection The first nexthop.
   * @param expiryTime The time after which the nexthop is no longer valid
   */
  NfpAnchorAdvertisement (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection, Time expiryTime);

  virtual ~NfpAnchorAdvertisement ();

  /**
   * Returns true if the given advertisement is feasible (higher seqnum or
   * same seqnum and shorter or same distance).
   *
   * Also, to be feasible the anchorName and Prefix must match the names given
   * when this object was created.
   *
   * @param other
   * @return true if feasible, false otherwise
   */
  bool IsFeasibleAdvertisement (Ptr<NfpAdvertise> other) const;

  typedef enum
  {
    REPLACE,
    EQUAL_COST,
    IGNORE
  } CompareResult;

  /**
   * Update the advertisement table with the given advertisement.
   *
   * PRECONDITION: IsFeasibleAdvertisement() must be true.
   *
   * @param advertise The advertisement to update to
   * @param ingressConnection The nexthop.
   * @param expiryTime The time at which the nexthop expires (is no longer valid)
   * @return IGNORE Advertisement is worse than current route
   * @return REPLACE Advertisement is strictly better than what we have
   * @return EQUAL_COST Advertisement is same as what we have (multipath)
   */
  CompareResult UpdateAdvertisement (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection, Time expiryTime);

  /**
  * Returns the route entry appropriate to this anchor for the prefix.
  *
  * This will update the internal state by pruning the nexthop list of any expired nexthops.
  *
  * NOTE: The Cost of the route entries does not include link costs to the next hop connection IDs
  *
  * @return The set of valid next hops.
  */
  Ptr<const CCNxRoute> GetRouteNoLinkCosts (void);

  /**
   * Return the name of the anchor whose advertisements we're tracking
   * @return
   */
  Ptr<const CCNxName> GetAnchorName (void) const;

  /**
   * Returns a const version of the best advertisement.  It is const just to make sure
   * that the caller does not change any state
   */
  Ptr<const NfpAdvertise> GetBestAdvertisement (void) const;

  /**
   * Remove the Connection from the nexthop list.
   *
   * It is ok if the nexthop is not in the list, nothing changes.
   *
   * This method may leave the advertisement with no nexthops.  That is ok too.  One may want
   * to store the sequence number state for some time, even if there are no nexthps, before
   * removing the state.
   *
   * @param Connection The nexthop to remove.
   * @return true if nexthop was removed from nexthop list, false if was not in list
   */
  bool RemoveNexthop (Ptr<CCNxConnection> connection);

  /**
   * Returns the time of the last state change.
   *
   * This is useful if the route is in holddown due to no nexthops so one could
   * determine if it is time to erase the entry.
   *
   * @return The simulation time of the last state change.
   */
  Time GetLastChangeTime (void) const;

  /**
   * Returns the number of next hops to the anchor
   * @return Number of next hops
   */
  size_t GetNexthopCount (void);

  /**
   * Overload output operator as a friend method so we can do this without exposing non-cost methods.
   *
   * @param os
   * @return
   */
  friend std::ostream & operator << (std::ostream &os, const NfpAnchorAdvertisement &anchorAdvertise);

  NfpComputationCost GetComputationCost() const;

protected:
  NfpComputationCost m_computationCost;

  Ptr<const CCNxName> m_anchorName;
  Ptr<const CCNxName> m_prefix;
  Ptr<NfpAdvertise> m_bestAdvertisement;

  Time m_lastChangeTime;

  /**
   * The list of all valid nexthops.
   *
   * The expiry time of each nexthop is stored in m_expiryTimes (see below).  For a nexthop to be valid,
   * it must be in this set and it must not be expired.
   */
  Ptr<CCNxRoute> m_equalCostPaths;

  typedef std::map< Ptr<CCNxConnection>, Time > ExpiryTimeMap;

  /**
   * Stores the expiry time of a nexthop (stored in m_equalCostPaths).
   */
  ExpiryTimeMap m_expiryTimes;

  /**
   * Compare the sequence number and distance of the advertisement to our best advertisement.
   *
   * If our seqnum < other seqnum => REPLACE
   * If our seqnum = other seqnum and our distance > other distnace => REPLACE
   * If our seqnum = other seqnum and our distance = other distnace => EQUAL_COST
   * otherwise IGNORE
   *
   * @param advertise
   * @return
   */
  CompareResult CompareSeqnumAndDistance (Ptr<NfpAdvertise> advertise) const;

  /**
   * The advertisement is better than what we have right now.  Replace our data.
   *
   * @param advertise The advertisement to update our state to
   * @param ingressConnection The connection the advertisement came in on
   */
  void Replace (Ptr<NfpAdvertise> advertise, Ptr<CCNxConnection> ingressConnection, Time expiryTime);

  /**
   * The advertisement represents an equal cost path.  Add the ingress connection id to
   * our list of equal cost paths.
   *
   * @param ingressConnection The connection the advertisement came in on
   */
  void AddEqualCostPath (Ptr<CCNxConnection> ingressConnection, Time expiryTime);

  /**
   * Erase all next hops.  Also removes their expiry times.
   *
   * This is used to clear all the next hops when we learn a high sequence number advertisement
   */
  void ClearEqualCostPaths (void);

  /**
   * Given the current time `now`, remove any nexthops from m_equalCostPaths that have expired.
   *
   * @param now The current time
   */
  void PruneEqualCostPaths (Time now);

  /**
   * Log our current state.
   */
  void LogState (void) const;

  /**
   * Updates the time of the last change to the record
   */
  void UpdateLastChangeTime (void);
};


}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNX_MODEL_ROUTING_NFP_NFP_ANCHOR_ADVERTISEMENT_H_ */
