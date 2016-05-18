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

#ifndef CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_FORWARDER_WORK_ITEM_H_
#define CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_FORWARDER_WORK_ITEM_H_

#include "ns3/ccnx-forwarder-message.h"
#include "ns3/ccnx-forwarder.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-standard-forwarder
 *
 * A specialization of `CCNxForwarderMessage` to carry extra state used by the `CCNxStandardForwarder`.
 *
 * The standard forwarder uses the `CCNxStandardForwarderWorkItem` to encapsulate a message it receives
 * from the CCNxL3Protocol via `RouteIn()` or `RouteOut()` to track the message through the PIT, FIB, and CS.
 * It encapsulates things like the CCNxPacket, and the ingressConnection.
 *
 * This is the class that gets stored in all the `CCNxDelayQueue`s.
 */
class CCNxStandardForwarderWorkItem : public CCNxForwarderMessage
{
public:
  CCNxStandardForwarderWorkItem (Ptr<CCNxPacket> packet,
                                 Ptr<CCNxConnection> ingressConnection,
                                 Ptr<CCNxConnection> egressConnection);

  virtual ~CCNxStandardForwarderWorkItem ();

  Ptr<CCNxConnection> GetEgressConnection () const;

  /**
   * Flag the work item as having an error of the specified type.
   *
   * The default value of the route error is CCNxRoutingError_NoError.
   *
   * @param errno [in] The error encountered while processing the message.
   */
  void SetRouteError (enum CCNxRoutingError::RoutingErrno routingErrno);

  /**
   * Returns the error associated with this work item.
   *
   * The default value of the route error is CCNxRoutingError_NoError.
   *
   * @return The error associated with this work item.
   */
  CCNxRoutingError::RoutingErrno GetRouteError () const;

  /**
   * Returns true if the route error is anything besides CCNxRoutingError_NoError.
   * @return true if error is not CCNxRoutingError_NoError
   * @return false if error is CCNxRoutingError_NoError
   */
  bool HasRouteError () const;

  /**
   * On a successful route lookup, set the list of egress connections to use.
   *
   * if not set, the list of egress connections will be empty.
   *
   * @param list [in] The list of connections to use.
   */
  void SetConnectionsList (Ptr<CCNxConnectionList> list);

  /**
   * If there was a successful routing lookup, this contains the list of egress connections
   *
   * If there were no matching or a failure, this will be an empty list.
   *
   * @return  The list of egress connections.
   */
  Ptr<CCNxConnectionList> GetConnectionsList () const;

  /**
   * If there was a successful content store lookup, use this to store a ptr to the matching content object.
   *
   *
   */
  void SetContentStorePacket (Ptr<CCNxPacket> packet) ;

  /**
   * If there was a successful content store lookup, this contains the matching content object.
   *
   * If there were no matching or a failure, this will be an null ptr.
   *
   * @return Ptr to the matching content object packet.
   */
  Ptr<CCNxPacket> GetContentStorePacket () const;


  /**
  * set this flag to indicate if content was added successfully.
  *
  */
 void SetContentAddedFlag (bool added) ;

 /**
  * get this flag to see if content was added successfully.
  *
  * @return true if content was added successfully.
  */
 bool GetContentAddedFlag () const;


private:
  Ptr<CCNxConnection> m_egressConnection;
  CCNxRoutingError::RoutingErrno m_routingErrno;

  Ptr<CCNxConnectionList> m_connectionsList;

  Ptr<CCNxPacket> m_contentStorePacket;

  bool m_contentAddedFlag;
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_FORWARDER_WORK_ITEM_H_ */
