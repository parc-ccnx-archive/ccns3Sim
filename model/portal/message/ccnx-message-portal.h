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

/*
 * Packet-by-packet portal.
 */

#ifndef CCNS3_CCNXMESSAGEPORTAL_H
#define CCNS3_CCNXMESSAGEPORTAL_H

#include <deque>

#include "ns3/node.h"
#include "ns3/ccnx-l3-protocol.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-portal.h"
#include "ns3/ccnx-l4-protocol.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-portal
 *
 * A Message portal does no L4 processing on packets, it passes them one for one.
 *
 * @see CCNxMessagePortalFactory
 */
class CCNxMessagePortal : public CCNxPortal,
                          public CCNxL4Protocol
{
public:
  static TypeId GetTypeId (void);

  /**
   * Constructor to create CCNxMessagePortal.
   */
  CCNxMessagePortal (void);

  /**
   * Destructor for CCNxMessagePortal.
   */
  virtual ~CCNxMessagePortal (void);

  /**
   * Sets the pointer to the L3 CCNx protocol.
   *
   * Can only be set once.
   *
   * @param [in] L3Protocol to set to
   */
  void SetCCNx (Ptr<CCNxL3Protocol> ccnx);


  /**
   * From CCNxL4Protocol
   */

  /**
   * Receives a packet from the L3 protocol for forwarding up to the application.
   * When the L4 protocol receives a packet, it will call NotifyRecv(), which the
   * base class CCNxPortal will use to notify the applications receive callback.
   *
   * Usually when the L3 protocol calls this function, it makes a tight call loop of
   * Receive() -> NotifyRecv() -> m_receiveCallback() -> Recv().
   *
   * @param [in] packet CCNx packet to be received
   * @param [in] incoming Connection to receive packet from
   *
   * @return true = success, false = failure
   *
   * @see CCNxMessagePortalFactory
   */
  virtual bool ReceiveFromLayer3 (Ptr<CCNxPacket> packet, Ptr<CCNxConnection> incoming);


  /**
   * From CCNxPortal
   */

  /**
   * Return the L3 protocol instance under this L4 protocol.  Set with SetCCNx().
   */
  virtual Ptr<CCNxL3Protocol> GetCCNx (void) const;

  /**
   * Register a name with the local forwarder to listen on
   *
   * @param [in] prefix The CCNxName to listen on
   *
   * @return true = success, false = failure
   */
  virtual bool RegisterPrefix (Ptr<const CCNxName> prefix);

  /**
   * Un-Register a name with the local forwarder
   *
   * @param [in] prefix The CCNxName to listen on
   *
   * @return true = success, false = failure
   */
  virtual void UnregisterPrefix (Ptr<const CCNxName> prefix);

  /**
   * Like RegisterPrefix but have the routing protocol advertise the name too.
   */
  virtual bool RegisterAnchor (Ptr<const CCNxName> prefix);

  /**
   * Like UnregisterPrefix but have the routing protocol advertise the name too.
   */
  virtual void UnregisterAnchor (Ptr<const CCNxName> prefix);

  /**
   * Close a portal.
   *
   * @return true = success, false = failure
   */
  virtual bool Close (void);

  /**
   * Send a CCNxPacket
   *
   * @return true = success, false = failure
   *
   * @see SetSendCallback
   */
  virtual bool Send (Ptr<CCNxPacket> packet);

  /**
   * Send a packet bypassing normal routing.
   *
   * Sends the packet to the connection identified by the connection id.  An
   * application can learn about a connection id, for instance, by doing
   * RecvFrom() and looking at incomingConneciton->GetConnectionId().
   *
   * Example:
   * @code
   * void ReceiveCallback(Ptr<CCNxPortal> portal)
   * {
   *    Ptr<CCNxConnection> conn;
   *    while (Ptr<CCNxPacket> request = portal->RecvFrom(conn)) {
   *       Ptr<CCNxPacket> reply = GenerateReply(request);
   *       portal->SendTo(reply, conn->GetConnectionId());
   *    }
   * }
   * @endcode
   */
  virtual bool SendTo (Ptr<CCNxPacket> packet, uint32_t egressConnectionId);

  /**
   * The application calls this to read data.
   *
   * When the L4 protocol receives data from L3, it will call NotifyRecv() which in turn
   * calls the application's receive callback.  The application will then call either
   * Recv() or RecvFrom() to read data from L4.
   *
   * @see SetRecvCallback
   */
  virtual Ptr<CCNxPacket> Recv (void);

  /**
   * The application calls this to read data.
   *
   * When the L4 protocol receives data from L3, it will call NotifyRecv() which in turn
   * calls the application's receive callback.  The application will then call either
   * Recv() or RecvFrom() to read data from L4.
   *
   * @param [in] incomingConnection Will be set to the in-coming connection
   * @return CCNxPacket
   * @see SetRecvCallback
   */
  virtual Ptr<CCNxPacket> RecvFrom (Ptr<CCNxConnection> &incomingConnection);

private:
  friend class CCNxMessagePortalFactory;

  Ptr<CCNxL3Protocol> m_ccnx;

  typedef std::pair< Ptr<CCNxPacket>, Ptr<CCNxConnection> > QueueEntryType;
  typedef std::deque< QueueEntryType  * > QueueType;

  /**
   * The L4 protocol queues packets from L3 until the application reads them via the
   * Recv() or RecvFrom() functions.
   */
  QueueType m_inputQueue;
};
}
}



#endif //CCNS3_CCNXMESSAGEPORTAL_H
