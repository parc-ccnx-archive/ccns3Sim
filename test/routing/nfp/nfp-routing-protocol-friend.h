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

#ifndef NFPROUTINGPROTOCOLFRIEND_H
#define NFPROUTINGPROTOCOLFRIEND_H

#include <map>
#include "ns3/ccnx-message-portal.h"

namespace ns3 {
namespace ccnx {

/**
 * Helper function to create an instance of the NfpRoutingProtocol.
 *
 * @param node The node to instantiate the protocol on
 * @return A smart pointer to the routing protocol
 */
static inline Ptr<NfpRoutingProtocol>
CreateProtocol (Ptr<Node> node)
{
  NfpRoutingHelper helper;

  CCNxStackHelper stack;
  stack.SetRoutingHelper (helper);
  stack.Install (node);

  Ptr<NfpRoutingProtocol> nfp = node->GetObject<NfpRoutingProtocol> ();
  return nfp;
}
}
}

/**
 * \ingroup ccnx-test
 *
 * This class is a Friend of NfpRoutingProtocol so we can access protected class members.
 *
 * It also gets underneath the portal to redirect the L3 traffic to local queues here so the
 * unit test can inspect everything NfpRoutingProtocol sends and can inject traffic too.
 */
class NfpRoutingProtocolFriend
{
public:
  NfpRoutingProtocolFriend (ns3::Ptr<ns3::ccnx::NfpRoutingProtocol> nfp) :
    m_nfp (&(*nfp))
  {
  }

  size_t
  GetAnchorMapSize (void) const
  {
    return m_nfp->m_anchorNames.size ();
  }

  uint32_t
  GetAnchorCount (ns3::Ptr<const ns3::ccnx::CCNxName> prefix) const
  {
    return m_nfp->m_anchorNames[prefix];
  }

  void
  SetTimer (ns3::Timer &timer, ns3::Time interval, ns3::Time jitter)
  {
    m_nfp->SetTimer (timer, interval, jitter);
  }

  void
  DoInitialize ()
  {
    m_nfp->DoInitialize ();
  }

  ns3::Timer *
  GetHelloTimer ()
  {
    return &m_nfp->m_helloTimer;
  }

  ns3::Ptr<const ns3::ccnx::CCNxName>
  GetRouterName ()
  {
    return m_nfp->m_routerName;
  }

  ns3::Ptr<ns3::ccnx::NfpPayload>
  CreatePayload (void)
  {
    return m_nfp->CreatePayload ();
  }

  ns3::Ptr<ns3::ccnx::CCNxPacket>
  CreatePacket (ns3::Ptr<ns3::ccnx::NfpPayload> payload)
  {
    return m_nfp->CreatePacket (payload);
  }

  /**
   * Returns the prefix NFP uses in Interest messages.
   * @return A smart pointer to a CCNxName
   */
  ns3::Ptr<const ns3::ccnx::CCNxName>
  GetNfpPrefix (void) const
  {
    return m_nfp->m_nfpPrefix;
  }

  void
  Broadcast (ns3::Ptr<ns3::ccnx::CCNxPacket> packet)
  {
    m_nfp->Broadcast (packet);
  }

  std::map< uint32_t, ns3::Ptr<ns3::ccnx::CCNxL3Interface> > *
  GetInterfaces (void)
  {
    return &m_nfp->m_interfaces;
  }

  void ReceivePayload (ns3::Ptr<ns3::ccnx::NfpPayload> payload, ns3::Ptr<ns3::ccnx::CCNxConnection> ingressConnection)
  {
    m_nfp->ReceivePayload (payload, ingressConnection);
  }

  void ReceiveAdvertise (ns3::Ptr<ns3::ccnx::NfpAdvertise> advertise, ns3::Ptr<ns3::ccnx::CCNxConnection> ingressConnection)
  {
    m_nfp->ReceiveAdvertise (advertise, ingressConnection);
  }

  void ReceiveWithdraw (ns3::Ptr<ns3::ccnx::NfpWithdraw> withdraw, ns3::Ptr<ns3::ccnx::CCNxConnection> ingressConnection)
  {
    m_nfp->ReceiveWithdraw (withdraw, ingressConnection);
  }

  bool ReceiveHello (ns3::Ptr<const ns3::ccnx::CCNxName> neighbor, uint16_t msgSeqnum, ns3::Ptr<ns3::ccnx::CCNxConnection> ingressConnection)
  {
    return m_nfp->ReceiveHello (neighbor, msgSeqnum, ingressConnection);
  }

  typedef std::map< ns3::Ptr<ns3::ccnx::NfpNeighborKey>, ns3::Ptr<ns3::ccnx::NfpNeighbor>, ns3::ccnx::NfpNeighborKey::isLessPtrNfpNeighborKey > NeighborMapType;
  NeighborMapType &
  GetNeighbors (void)
  {
    return m_nfp->m_neighbors;
  }

private:
  ns3::ccnx::NfpRoutingProtocol * m_nfp;
  typedef std::deque< ns3::Ptr< ns3::ccnx::CCNxPacket > > QueueType;



};

#endif
