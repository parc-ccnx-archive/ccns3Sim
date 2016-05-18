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


#ifndef CCNS3SIM_CCNXSTANDARDPITENTRY_H
#define CCNS3SIM_CCNXSTANDARDPITENTRY_H

#include "ns3/simple-ref-count.h"


#include "ns3/ccnx-connection.h"
#include "ns3/ccnx-forwarder.h"
#include "ns3/ccnx-pit.h"

#include <set>

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-standard-forwarder
 *
 * A PitEntry stores the latest expiry time of the PitEntry and a list of reverse route connections.
 *
 * The Pit manipulates the PitEntry by calling `ReceiveInterest()` or `SatisfyInterest()`.
 *
 * Interest aggregation strategy:
 * - The first Interest for a {name, keyid, hash} is forwarded
 * - A second Interest for a {name, keyid, hash} from a different reverse path may be aggregated
 * - A second Interest for a {name, keyid, hash} from an existing Interest is forwarded
 * - The Interest Lifetime is like a subscription time.  A reverse path entry is removed once the lifetime
 *   is exceeded.
 * - Whan an Interest arrives or is aggregated, the Lifetime for that reverse hop is extended.  As a simplification,
 *   we only keep a single lifetime not per reverse hop.
 *
 * Expiration: When an interest is added, it's expiryTime is set = Now()+InterestTimeout. This interest is discarded
 * when a related interest or contentObject is being processed  if Now > expiryTime.
 */
class CCNxStandardPitEntry : public ns3::SimpleRefCount<CCNxStandardPitEntry>
{
public:
  /**
   * This is a set defined on the Ptr<>.  It will be sorted by the memory address of the Ptr.
   */
  typedef std::set< Ptr<CCNxConnection> > ReverseRouteType;

  /**
   * When a PitEntry is created, it is expired and has no reverse routes.  You need
   * to prime it with an actual Interest via `ReceiveInterest()` which will both update
   * the expiry time (extend it) and add a reverse route.
   */
  CCNxStandardPitEntry ();
  virtual ~CCNxStandardPitEntry ();

  /**
  * ReceiveInterest - process incoming Interest packet
  *
  * @param interest [in] Pointer to CCNxInterest object
  * @param ingress [in] source of this packet
  * @param expiryTime [in] The time at which this particular Interest expires
  *
  * @return Verdict::Forward - forward this packet (using next hop from fib)
  * @return Verdict::Aggregrate - do not forward
  * @return Verdict::Error - something has gone badly wrong
  */
  virtual CCNxPit::Verdict ReceiveInterest (Ptr<CCNxInterest> interest, Ptr<CCNxConnection> ingress, Time expiryTime);

  /**
  * SatisfyInterest - Will satisfy all reverse routes, except its ingress connection
  *
  * SatisfyInterest will always return a non-null list of connections, though it may be empty.
  *
  * @param ingressConnection [in] source of this packet (not used)
  *
  * @return A set of the connections satisfied by the content object (may be empty)
  virtual std::pair <bool, Ptr<CCNxConnectionList> > SatisfyInterest (Ptr<CCNxPacket> packet,  bool nameMap);
  */
  virtual ReverseRouteType SatisfyInterest (Ptr<CCNxConnection> ingressConnection);

  /**
   * The number of reverse route entries in the Pit entry
   *
   * Will be 0 if the PitEntry is expired (regardless of the data structure size)
   *
   * @return The number of non-expired reverse route entries.
   */
  size_t size() const;

  /**
   * Determines if the PitEntry is valid (not expired)
   * @return true if the pit entry is expired (not valid)
   * @return false if the pit entry is not expired (is valid)
   */
  bool isExpired() const;

  /*
   *  Two methods to display this pit entry
   */
  std::ostream & PrintPitEntry (std::ostream & os);

  friend std::ostream &operator<< (std::ostream& os, Ptr<CCNxStandardPitEntry> ccnxStandardPitEntry);

private:

  /**
   * The set of reverse routes, may be empty
   */
  ReverseRouteType m_reverseRoutes;

  /**
   * The interest expiry time is the time at which this PIT entry expires.
   */
  Time m_expiryTime;

};          //class


}    //namespace
} //namespace






#endif //CCNS3SIM_CCNXSTANDARDPITENTRY_H
