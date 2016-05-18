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


#ifndef CCNS3_FIB_H
#define CCNS3_FIB_H

#include <vector>
#include <map>
#include "ns3/ccnx-name.h"
#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-connection.h"
#include "ns3/ccnx-forwarder.h"
#include "ns3/ccnx-forwarder-message.h"


namespace ns3 {
namespace ccnx {


/**
 * @ingroup ccnx-forwarder
 *
 * Abstract base class for the CCNx fib.
 */
class CCNxFib : public ns3::Object
{
public:
  CCNxFib ();
  virtual ~CCNxFib ();               //c++ thang

  /**
   *  Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * RemoveConnection - Remove a ConnId from all routes. Delete FibEntry if this was it's only ConnId.
   * A FIB_AddConn method is not required as it's implied in AddRoute.

   *
   * @param [in] ConnId
   * @return true on success (currently only returns true).
   *
   * Example:
   * @code
   * {
   *     CCNxStandardFib fib;
   *
   *     CCNxConnection connId = 3;
   *     bool ok = fib.FibRemoveConn(connId);
   * }
   * @endcode
   */
  virtual bool RemoveConnection ( Ptr<CCNxConnection>  connection) = 0;

  /**
   * @brief Lookup - lookup a CCNxName in the CCNxFib
   *
   * This is an asynchronous call to lookup a route in the routing table (FIB).  The
   * result of the lookup will be returned at a later time via the `LookupCallback()`
   *
   * @param message [in] The forwarder message to lookup in the FIB.  Must be a CCNxInterest.
   *
   * Example:
   * @code
   * {
   *
   * }
   * @endcode
   *
   * @see LookupCallback
   */
  virtual void Lookup (Ptr<CCNxForwarderMessage> message) = 0;

  /**
   * The `LookupCallback` is an asynchronous callback from the FIB after it has processed
   * an Interest sent to `Lookup()`.
   *
   * The FIB should ensure there is always one call to the callback for each call to `Lookup()`.
   *
   * @param Ptr<CCNxForwarderMessage> [in] The forwarder message being routed
   * @param Ptr<CCNxConnectionList> [in] The set of connections to forward the Content Object on (may be empty)
   */
  typedef Callback<void, Ptr<CCNxForwarderMessage>, Ptr<CCNxConnectionList> > LookupCallback;

  /**
   * Configure the callback for `Lookup()`.  This must be set before using the FIB table.
   *
   * @param lookupCallback [in] The callback to use for `Lookup()`.
   */
  virtual void SetLookupCallback (LookupCallback lookupCallback) = 0;

  /**
   * AddRoute - add a prefix to the Fib. A new FibEntry is created if necessary.
   *
   * @param [in]  Ptr<CCNxName> ccnxName - a Ptr to the ccnxName object of the prefix
   * @return true on success (currently only returns true).
   *
   * Example:
   * @code
   * {
   *  Ptr<CCNxName> name1 = Create<CCNxName>("ccnx:/name=his/name=whitehorse/name=was/name=abox/name=oftoothpicks");
   *  CCNxStandardFib fib;
   *  CCNxConnection connId = 3;
   *  bool ok  = fib.AddRoute(name1,connId);
   *
   * }
   * @endcode
   *
   * @see ccnxForwarding_Release
   */
  virtual bool AddRoute (Ptr<const CCNxName> ccnxName, Ptr<CCNxConnection>  connection) = 0;

  /**
   * DeleteRoute - delete a route from the Fib. The FibEntry is deleted if empty.
   *
   * @param [in]  Ptr<CCNxName> ccnxName - a Ptr to the ccnxName object of the prefix
   * @return true if route found and deleted, false otherwise.
   *
   * Example:
   * @code
   * {
   *  Ptr<CCNxName> name1 = Create<CCNxName>("ccnx:/name=he/name=gallopedaround/name=until/name=hitby/name=acar");
   *  CCNxStandardFib fib;
   *  CCNxConnection connId = 3;
   *  bool ok = fib.DeleteRoute(name1,connId);
   *  if (ok == false) { do error processing; }
   *
   * }
   * @endcode
   *
   * @see ccnxForwarding_Release
   */
  virtual bool DeleteRoute (Ptr<const CCNxName> ccnxName, Ptr<CCNxConnection>  connection) = 0;

  /**
   * PrintRoute(s) - print one or more Fib entries.
   *
   * @param [in] ccnxName (PrintRoute only)
   * @return single route (PrintRoute) or route table (PrintRoutes)
   *
   * Example:
   * @code
   * {
   * }
   */
  virtual std::ostream & PrintRoutes (std::ostream & os) = 0;

  virtual std::ostream & PrintRoute (std::ostream & os, Ptr<const CCNxName> ccnxName) = 0;

  virtual int CountEntries () = 0;


};          //class
}    //namespace
} //namespace



#endif //CCNS3_FIB_H


