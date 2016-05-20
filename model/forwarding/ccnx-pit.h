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

#ifndef CCNS3_CCNXPIT_H
#define CCNS3_CCNXPIT_H

#include <map>
#include <vector>
#include <list>
#include "ns3/ccnx-packet.h"
#include "ns3/ccnx-connection-list.h"
#include <ns3/ccnx-interest.h>
#include "ns3/ccnx-forwarder.h"
#include "ns3/ccnx-forwarder-message.h"

namespace ns3 {
namespace ccnx {


/**
 * @ingroup ccnx-forwarder
 *
 * Abstract base class of a PIT.
 *
 * The PIT API is asynchronous so the PIT table can impose its own delays in packet processing.
 * When then forwarder calls `ReceiveInterest()`, the PIT table will send the result to
 * `ReceiveInterestCallback`.  Likewise for `SatisfyInterest()`, the PIT table will return
 * the result to `SatisyfInterestCallback`.
 *
 * TODO CCN: Probably want to make an Iterator on the PIT table with find() and erase() calls
 * rather than have RemoveEntry().
 */
class CCNxPit : public ns3::Object
{
public:
  /*
   * result of Pit interest processing
   */
  enum Verdict
  {
    Forward,
    Aggregate,
    Error,
  };

  static std::string VerdictToString(enum Verdict verdict);

  CCNxPit ();
  virtual ~CCNxPit ();

  /**
    * Get the type ID.
    * @return the object TypeId
    */
  static TypeId GetTypeId ();

  /**
   * Receive an Interest.  If it is added to the Pit, the Forward verdict is returned indicating
   * that the Interest should be forwarded.  If the Interest is aggregated with a prior Interest,
   * the Verdict Aggregate is returned.
   *
   * @param message [in] - The forwarder message to be looked up in the PIT (must be an Interest)
   *
   * @return Verdict (enum)
   *    Forward - forward this packet (using next hop from fib)
   *    Aggregrate - do not forward
   *    Error - discard this interest, something is deeply wrong with it
   **/
  virtual void ReceiveInterest (Ptr<CCNxForwarderMessage> message) = 0;

  /**
   * The `ReceiveInterestCallback` is an asynchronous callback from the PIT after it has processed
   * an Interest sent to `ReceiveInterest()`.
   *
   * The PIT should ensure there is always one call to the callback for each call to `ReceiveInterest()`.
   *
   * @param Ptr<CCNxForwarderMessage> The forwarder message being routed
   * @param CCNxPit::Verdict The result of the PIT table processing
   */
  typedef Callback<void, Ptr<CCNxForwarderMessage>, enum CCNxPit::Verdict > ReceiveInterestCallback;

  /**
   * Configure the callback for `ReceiveInterest()`.  This must be set before using the PIT table.
   *
   * @param routeCallback The callback to use for `ReceiveInterest()`.
   */
  virtual void SetReceiveInterestCallback (ReceiveInterestCallback receiveInterestCallback) = 0;

  /**
   * Receive a ContentObject packet and see what interests it satisfies in the Pit.  Returns a list
   * of the Connections of satisfied interests to forward the ContentObject to.  The corresponding Pit entries
   * are removed.  The caller must then resolve each Connection in the list to an
   * egress connection. The list may be empty if there is no matching interest.
    *
    * @param [in] packet - Pointer to CCNxPacket object
    * @param [in] ingress  - source of this packet (not used)
    *
    *
    * @return ConnectionsList - returns list of connections to which this packet should be forwarded. list may be empty.
    */
  virtual void SatisfyInterest (Ptr<CCNxForwarderMessage> message) = 0;

  /**
   * The `SatisfyInterestCallback` is an asynchronous callback from the PIT after it has processed
   * an Interest sent to `SatisfyInterest()`.
   *
   * The PIT should ensure there is always one call to the callback for each call to `SatisfyInterest()`.
   *
   * @param Ptr<CCNxForwarderMessage> The forwarder message being routed
   * @param Ptr<CCNxConnectionList> The set of connections to forward the Content Object on (may be empty)
   */
  typedef Callback<void, Ptr<CCNxForwarderMessage>, Ptr<CCNxConnectionList> > SatisfyInterestCallback;

  /**
   * Configure the callback for `SatisfyInterest()`.  This must be set before using the PIT table.
   *
   * @param routeCallback The callback to use for `ReceiveInterest()`.
   */
  virtual void SetSatisfyInterestCallback (SatisfyInterestCallback satisfyInterestCallback) = 0;

  /**
   * Removes the PIT entry that corresponds to the given Interest
   */
  virtual void RemoveEntry (Ptr<CCNxInterest> interest) = 0;

  virtual int CountEntries () = 0;

  /**
   * Display the PIT table
   */
  virtual void Print (std::ostream & os) const = 0;

};
}
}



#endif //CCNS3_CCNXPIT_H
