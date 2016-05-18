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


#ifndef CCNS3_CCNXPORTAL_H
#define CCNS3_CCNXPORTAL_H

#include "ns3/object.h"
#include "ns3/ccnx-l3-protocol.h"
#include "ns3/ccnx-connection.h"
#include "ns3/ccnx-packet.h"

namespace ns3 {
namespace ccnx {

/**
 * @defgroup ccnx-portal CCNx Layer 4 Portal
 * @ingroup ccnx
 */

/**
 * @ingroup ccnx-portal
 *
 * The approximate equivalent of a Socket.  This is the abstraction that an Application
 * uses to communicate with a CCNxL4Protocol.
 *
 * Similar semantics to the NS3 Socket, but based on CCNxPacket not bytes.   Like the NS3
 * Socket model, one aggregates the implementation class factories to the Node and then
 * creates a portal with CCNxPortal::Create(node, typeid).
 *
 */
class CCNxPortal : public Object
{
public:
  static TypeId GetTypeId (void);

  /**
   * Constructor to create CCNxPortal.
   */
  CCNxPortal (void);

  /**
   * Destructor for CCNxPortal.
   */
  virtual ~CCNxPortal (void);

  /**
   * This method wraps the creation of sockets that is performed
   * on a given node by a SocketFactory specified by TypeId.
   *
   * @param [in] node The node on which to create the socket
   * @param [in] tid The TypeId of a SocketFactory class to use
   *
   * @return A smart pointer to a newly created socket.
   *
   * @code
   * Ptr<CCNxPortal>
   * foo(Ptr<Node> node)
   * {
   *   TypeId tid = TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory");
   *   Ptr<CCNxPortal> portal = CCNxPortal::CreatePortal (node, tid);
   *   return portal;
   * }
   * @endcode
   */
  static Ptr<CCNxPortal> CreatePortal (Ptr<Node> node, TypeId tid);

  /**
   * Notify the application when a CCNxPacket sent from transport to forwarder.
   *
   * The callback include the CCNxPortal pointer plus the CCNxPacket pointer
   */
  void SetDataSentCallback (Callback<void, Ptr<CCNxPortal>, Ptr<CCNxPacket> > dataSentCallback);

  /**
   * Notify application that a call to Read() will not block
   *
   * Example:
   * @code
   * class Foo
   * {
   *  private:
   *    Ptr<CCNxPortal> m_portal;
   *
   *    void ReceiveNotify(Ptr<CCNxPortal> portal) {
   *      Ptr<CCNxPacket> packet;
   *      while ((packet = portal->Recv ()))
   *        {
   *          std::cout << packet->GetMessage()->GetName () << std::endl;
   *        }
   *   }
   *  public:
   *    Foo(Ptr<Node> node) {
   *      m_portal = CCNxPortal::CreatePortal (node, TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory"));
   *      NS_ASSERT_MSG(m_portal, "Failed to create CCNxMessagePortal on node " << m_node);
   *      m_portal->SetRecvCallback (MakeCallback (&Foo::ReceiveNotify, this));
   *    }
   *  };   * @endcode
   */
  void SetRecvCallback (Callback<void, Ptr<CCNxPortal> > receiveDataCallback);

protected:
  /*
   * Call the DataSentCallback
   */
  void NotifyDataSent (Ptr<CCNxPacket> packet);

  /*
   * Call the RecvCallback
   */
  void NotifyRecv (void);

private:
  Callback<void, Ptr<CCNxPortal>, Ptr<CCNxPacket> >     m_dataSent;
  Callback<void, Ptr<CCNxPortal> >                      m_receivedData;

public:
  /**
   * Pure virtual functions to be implemented by concrete class
   */

  /**
   * Returns the CCNx L3 protocol this socket is bound to
   */
  virtual Ptr<CCNxL3Protocol> GetCCNx (void) const = 0;


  /**
   * Register a name with the local forwarder to listen on
   *
   * @param [in] prefix The CCNxName to listen on
   *
   * @return true = success, false = failure
   */
  virtual bool RegisterPrefix (Ptr<const CCNxName> prefix) = 0;

  /**
   * Un-Register a name with the local forwarder
   *
   * @param [in] prefix The CCNxName to listen on
   *
   * @return true = success, false = failure
   */
  virtual void UnregisterPrefix (Ptr<const CCNxName> prefix) = 0;

  /**
   * Like RegisterPrefix, but also tell the CCNxRoutingProtocol to advertise the name
   */
  virtual bool RegisterAnchor (Ptr<const CCNxName> prefix) = 0;

  /**
   * Like UnregisterPrefix, but also tell the CCNxRoutingProtocol to advertise the name
   */
  virtual void UnregisterAnchor (Ptr<const CCNxName> prefix) = 0;

  /**
   * Close a portal
   *
   * @return true = success, false = failure
   */
  virtual bool Close (void) = 0;

  /**
   * Send a CCNxPacket
   *
   * @param [in] CCNxPacket to send
   *
   * @return true = success, false = failure
   *
   * @see SetSendCallback
   */
  virtual bool Send (Ptr<CCNxPacket> packet) = 0;

  /**
   * Send a packet bypassing normal routing.
   *
   * Sends the packet to the connection identified by the connection id.  An
   * application can learn about a connection id, for instance, by doing
   * RecvFrom() and looking at incomingConneciton->GetConnectionId().  It
   * could also learn about them by using the CCNxL3Protocol API.
   *
   * @param [in] CCNxPacket to send
   * @param [in] connectionId Connection Id to send the CCNxPacket on
   *
   * @return true = success, false = failure
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
  virtual bool SendTo (Ptr<CCNxPacket> packet, uint32_t connectionId) = 0;

  /**
   * The application calls this to read data.
   *
   * When the L4 protocol receives data from L3, it will call NotifyRecv() which in turn
   * calls the application's receive callback.  The application will then call either
   * Recv() or RecvFrom() to read data from L4.
   *
   * @return CCNxPacket
   *
   * @see SetRecvCallback
   */
  virtual Ptr<CCNxPacket> Recv (void) = 0;

  /**
   * The application calls this to read data.
   *
   * When the L4 protocol receives data from L3, it will call NotifyRecv() which in turn
   * calls the application's receive callback.  The application will then call either
   * Recv() or RecvFrom() to read data from L4.
   *
   * @param [in] incomingConnection Will be set to the in-coming connection
   *
   * @return CCNxPacket
   *
   * @see SetRecvCallback
   */
  virtual Ptr<CCNxPacket> RecvFrom (Ptr<CCNxConnection> & incomingConnection) = 0;

};

}
}


#endif //CCNS3_CCNXPORTAL_H
