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

#include "ns3/test.h"
#include "ns3/ccnx-standard-layer3.h"
#include "ns3/ccnx-standard-forwarder.h"
#include "ns3/nfp-routing-protocol.h"
#include "ns3/virtual-net-device.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ccns3Sim-module.h"
#include "ns3/net-device.h"

#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxStandardLayer3 {

BeginTest (Constructor)
{
  Ptr<CCNxStandardLayer3> ccnx = CreateObject<CCNxStandardLayer3> ();

  bool exists = (ccnx);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
}
EndTest ()

BeginTest (GetProtocolNumber)
{
  uint16_t prot = CCNxStandardLayer3::GetProtocolNumber ();
  bool truth = (prot == 0x0801);
  NS_TEST_EXPECT_MSG_EQ (truth, true, "Values should match");
}
EndTest ()

BeginTest (GetSetNode)
{
  Ptr<CCNxStandardLayer3> ccnx = CreateObject<CCNxStandardLayer3> ();
  Ptr<Node> node = Create<Node> ();
  ccnx->SetNode (node);
  bool truth = (node == ccnx->GetNode ());
  NS_TEST_EXPECT_MSG_EQ (truth, true, "Values should match");
}
EndTest ()

BeginTest (GetSetRoutingProtocol)
{
  Ptr<NfpRoutingProtocol> nfp = CreateObject<NfpRoutingProtocol> ();
  Ptr<CCNxStandardLayer3> ccnx = CreateObject<CCNxStandardLayer3> ();
  ccnx->SetRoutingProtocol (nfp);
  bool truth = (nfp == ccnx->GetRoutingProtocol ());
  NS_TEST_EXPECT_MSG_EQ (truth, true, "Values should match");
}
EndTest ()

BeginTest (GetSetForwarder)
{
  Ptr<CCNxStandardForwarder> forwarder = CreateObject<CCNxStandardForwarder> ();
  Ptr<CCNxStandardLayer3> ccnx = CreateObject<CCNxStandardLayer3> ();
  ccnx->SetForwarder (forwarder);
  bool truth = (forwarder == ccnx->GetForwarder ());
  NS_TEST_EXPECT_MSG_EQ (truth, true, "Values should match");
}
EndTest ()

BeginTest (SetDefaultTtl)
{
  Ptr<CCNxStandardLayer3> ccnx = CreateObject<CCNxStandardLayer3> ();
  ccnx->SetDefaultTtl (100);
}
EndTest ()

static void
PortalReceive (Ptr<CCNxPortal> portal)
{
  Ptr<CCNxPacket> packet;
  while ((packet = portal->Recv ()))
    {
      Ptr<CCNxMessage> ccnxMessage = packet->GetMessage ();
      NS_ASSERT_MSG (ccnxMessage, "Got null message from packet");

      printf ("Packet received successfully \n");
    }
}

BeginTest (SendRecv)
{
  Ptr<Node> node = CreateObject<Node> ();

  CCNxStackHelper ccnx;
  ccnx.Install (node);

  // Create a CCNxPortal and have it register a Name
  TypeId tid = TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory");

  Ptr<CCNxPortal> sendPortal = CCNxPortal::CreatePortal (node, tid);
  Ptr<CCNxPortal> recvPortal = CCNxPortal::CreatePortal (node, tid);
  recvPortal->SetRecvCallback (MakeCallback (&PortalReceive));
  Ptr<CCNxName> recvName = Create<CCNxName> ("ccnx:/name=foo/name=sink");
  recvPortal->RegisterPrefix (recvName);

  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (100, true);
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (Create<CCNxName> ("ccnx:/name=foo/name=sink"), payload);
  Ptr<CCNxPacket> packet = CCNxPacket::CreateFromMessage (interest);

  if ((sendPortal->Send (packet)) == true)
    {
      printf ("Packet sent successfully \n");
    }

  recvPortal->UnregisterPrefix (recvName);
  sendPortal->Close ();
  recvPortal->Close ();
  sendPortal = 0;
  recvPortal = 0;
  node = 0;
}
EndTest ()

static void
PortalReceiveFrom (Ptr<CCNxPortal> portal)
{
  Ptr<CCNxPacket> packet;
  Ptr<CCNxConnection> connectionId;
  while ((packet = portal->RecvFrom (connectionId)))
    {
      NS_ASSERT_MSG (connectionId, "Received portal message with NULL connection");

      Ptr<CCNxMessage> ccnxMessage = packet->GetMessage ();
      NS_ASSERT_MSG (ccnxMessage, "Got null message from packet");

      printf ("Packet received from %d successfully \n", connectionId->GetConnectionId ());

      if ((portal->SendTo (packet, connectionId->GetConnectionId ())) == true)
        {
          printf ("Packet sent successfully \n");
        }
    }
}

BeginTest (SendToRecvFrom)
{
  NodeContainer c;
  c.Create (1);

  CCNxStackHelper ccnx;
  NfpRoutingHelper routingHelper;
  ccnx.SetRoutingHelper (routingHelper);
  ccnx.Install (c);

  // Create a CCNxPortal and have it register a Name
  TypeId tid = TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory");
  Ptr<Node> node = c.Get (0);

  Ptr<CCNxPortal> sendPortal = CCNxPortal::CreatePortal (node, tid);
  Ptr<CCNxPortal> recvPortal = CCNxPortal::CreatePortal (node, tid);
  recvPortal->SetRecvCallback (MakeCallback (&PortalReceiveFrom));
  Ptr<CCNxName> recvName = Create<CCNxName> ("ccnx:/name=foo/name=sink");
  recvPortal->RegisterAnchor (recvName);

  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (100, true);
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (Create<CCNxName> ("ccnx:/name=foo/name=sink"), payload);
  Ptr<CCNxPacket> packet = CCNxPacket::CreateFromMessage (interest);

  if ((sendPortal->Send (packet)) == true)
    {
      printf ("Packet sent successfully \n");
    }

  recvPortal->UnregisterAnchor (recvName);
  sendPortal->Close ();
  recvPortal->Close ();
  sendPortal = 0;
  recvPortal = 0;
}
EndTest ()

static const int mtu = 1400;

static Mac48Address Mac48AddressFromInteger (uint32_t id)
{
  uint8_t b3 = id >> 24;
  uint8_t b2 = (id >> 16) & 0xFF;
  uint8_t b1 = (id >> 8) & 0xFF;
  uint8_t b0 = id & 0xFF;

  uint8_t buffer[6] =
  { 0x11, 0x00, b3, b2, b1, b0 };
  Mac48Address addr = Mac48Address::Allocate ();
  addr.CopyFrom (buffer);
  return addr;
}

static bool
SendCallback (Ptr<Packet> packet, const Address& source,
              const Address& dest, uint16_t protocolNumber)
{
  return true;
}

static Ptr<VirtualNetDevice>
SetupTapDevice (Ptr<Node> node, uint32_t id, VirtualNetDevice::SendCallback send)
{
  Ptr<VirtualNetDevice> tap = CreateObject<VirtualNetDevice> ();

  tap->SetAddress (Mac48AddressFromInteger (id));
  tap->SetSendCallback (send);
  tap->SetMtu (mtu);
  node->AddDevice (tap);
  return tap;
}

BeginTest (GetAddInterface)
{
  Ptr<Node> node = Create<Node> ();

  CCNxStackHelper stack;
  stack.Install (node);

  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 0, MakeCallback (&SendCallback));

  Ptr<CCNxL3Protocol> ccnx = node->GetObject<CCNxL3Protocol> ();
  bool exists = (ccnx);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Value should match");

  uint32_t ifid = ccnx->AddInterface (device);
  Ptr<CCNxL3Interface> intf = ccnx->GetInterface (ifid);
  bool truth = (ifid == intf->GetIfIndex ());
  NS_TEST_EXPECT_MSG_EQ (truth, true, "Value should match");

  bool test = (ccnx->GetInterfaceForwarding (ifid) == intf->IsForwarding ());
  NS_TEST_EXPECT_MSG_EQ (test, true, "Value should match");
}
EndTest ()

BeginTest (GetInterfaceNonexistent)
{
  Ptr<Node> node = Create<Node> ();

  CCNxStackHelper stack;
  stack.Install (node);

  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 0, MakeCallback (&SendCallback));

  Ptr<CCNxL3Protocol> ccnx = node->GetObject<CCNxL3Protocol> ();

  Ptr<CCNxL3Interface> intf = ccnx->GetInterface (0);
  bool exists = (intf);
  NS_TEST_EXPECT_MSG_EQ (exists, false, "Value should match");
}
EndTest ()

BeginTest (GetNInterfaces)
{
  Ptr<Node> node = Create<Node> ();

  CCNxStackHelper stack;
  stack.Install (node);

  Ptr<VirtualNetDevice> device1 = SetupTapDevice (node, 0, MakeCallback (&SendCallback));
  Ptr<VirtualNetDevice> device2 = SetupTapDevice (node, 1, MakeCallback (&SendCallback));

  Ptr<CCNxL3Protocol> ccnx = node->GetObject<CCNxL3Protocol> ();
  bool exists = (ccnx);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Value should match");

  ccnx->AddInterface (device1);
  ccnx->AddInterface (device2);

  bool truth = (ccnx->GetNInterfaces () == 2);
  NS_TEST_EXPECT_MSG_EQ (truth, true, "Value should match");
}
EndTest ()

BeginTest (ReceiveFromLayer2)
{
  Ptr<Node> node = Create<Node> ();

  CCNxStackHelper stack;
  stack.Install (node);

  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 0, MakeCallback (&SendCallback));

  Ptr<CCNxName> prefixName = Create<CCNxName> ("ccnx:/name=foo/name=sink");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (100, true);
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (prefixName, payload);
  Ptr<CCNxPacket> packet = CCNxPacket::CreateFromMessage (interest);

  NetDevice::PacketType packetType = ns3::NetDevice::PACKET_BROADCAST;

  Ptr<CCNxL3Protocol> ccnx = node->GetObject<CCNxL3Protocol> ();
  uint16_t protocol = ccnx->GetProtocolNumber ();
  ccnx->AddInterface (device);

  Address from = Mac48Address ("11:00:00:00:11:11");
  Address to = Mac48Address ("ff:ff:ff:ff:ff:ff");

  Ptr<Packet> ns3Packet = packet->CreateNs3Packet ();

  ccnx->ReceiveFromLayer2 (device, ns3Packet, protocol, from, to, packetType);
}
EndTest ()


/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxStandardLayer3
 */
static class TestSuiteCCNxStandardLayer3 : public TestSuite
{
public:
  TestSuiteCCNxStandardLayer3 () : TestSuite ("ccnx-standard-layer3", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new GetProtocolNumber (), TestCase::QUICK);
    AddTestCase (new GetSetNode (), TestCase::QUICK);
    AddTestCase (new GetSetRoutingProtocol (), TestCase::QUICK);
    AddTestCase (new GetSetForwarder (), TestCase::QUICK);
    AddTestCase (new SetDefaultTtl (), TestCase::QUICK);
    AddTestCase (new SendRecv (), TestCase::QUICK);
    AddTestCase (new SendToRecvFrom (), TestCase::QUICK);
    AddTestCase (new GetAddInterface (), TestCase::QUICK);
    AddTestCase (new GetInterfaceNonexistent (), TestCase::QUICK);
    AddTestCase (new GetNInterfaces (), TestCase::QUICK);
    AddTestCase (new ReceiveFromLayer2 (), TestCase::QUICK);
  }
} g_TestSuiteCCNxStandardLayer3;

} // namespace TestSuiteCCNxStandardLayer3
