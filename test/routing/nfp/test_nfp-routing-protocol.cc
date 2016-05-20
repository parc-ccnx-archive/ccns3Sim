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

#include <deque>
#include <map>
#include "ns3/log.h"
#include "ns3/test.h"
#include "ns3/type-id.h"
#include "ns3/ccnx-stack-helper.h"
#include "ns3/ccnx-packet.h"

#include "ns3/nfp-routing-protocol.h"
#include "ns3/nfp-routing-helper.h"
#include "ns3/nfp-payload.h"
#include "ns3/ccnx-standard-layer3.h"

#include "ns3/virtual-net-device.h"
#include "nfp-routing-protocol-friend.h"
#include "../../mockups/mockup_ccnx-virtual-connection.h"


#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

// =================================

namespace NfpRoutingProtocolTests {

/**
 * Create a Mac48Address as 11:00:uint32_t
 *
 * @param id
 * @return
 */
static Mac48Address
Mac48AddressFromInteger (uint32_t id)
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

/**
 * Create a virtual network device that we can add to CCNxL3Protocol to feed packets in
 * and out of the routing protocol.
 *
 * The MAC address will be "11:00::(4-byte id)".
 *
 * The MTU is set to 1500 bytes.  If the callers wants to change the MTU, you should take
 * the CCNxL3Interface down, change it, then bring it up again.  That will cause the change
 * to be seen by the routing protocol.
 *
 * @param id	An ID for the virtual interface.  Will become part of the MAC address.
 * @param send The callback from the virtual device to send a packet.
 * @param node The node to install the device on.
 */
static Ptr<VirtualNetDevice>
SetupTapDevice (Ptr<Node> node, uint32_t id,
                VirtualNetDevice::SendCallback send)
{
  Ptr<VirtualNetDevice> tap = CreateObject<VirtualNetDevice> ();

  tap->SetAddress (Mac48AddressFromInteger (id));
  tap->SetSendCallback (send);
  tap->SetMtu (1500);
  node->AddDevice (tap);
  Ptr<CCNxL3Protocol> ccnx = node->GetObject<CCNxL3Protocol> ();
  ccnx->AddInterface (tap);
  return tap;
}

BeginTest (TestGetTypeId)
{
  TypeId id = NfpRoutingProtocol::GetTypeId ();

  std::string name = id.GetName ();
  std::string truth ("ns3::ccnx::NfpRoutingProtocol");

  NS_TEST_EXPECT_MSG_EQ (truth, name, "wrong name: " << name);
}
EndTest ()

BeginTest (TestAddAnchor_FirstTime)
{
  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=apple/name=pie");
  nfp->AddAnchorPrefix (prefix);

  size_t mapsize = theFriend.GetAnchorMapSize ();
  NS_TEST_ASSERT_MSG_EQ (mapsize, 1, "Anchor map should have 1 entry");

  uint32_t refcount = theFriend.GetAnchorCount (prefix);
  NS_TEST_ASSERT_MSG_EQ (refcount, 1, "Anchor map should have 1 refcount");
}
EndTest ()

BeginTest (TestAddAnchor_SecondTime)
{
  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=apple/name=pie");
  nfp->AddAnchorPrefix (prefix);
  nfp->AddAnchorPrefix (prefix);

  size_t mapsize = theFriend.GetAnchorMapSize ();
  NS_TEST_ASSERT_MSG_EQ (mapsize, 1, "Anchor map should have 1 entry");

  uint32_t refcount = theFriend.GetAnchorCount (prefix);
  NS_TEST_ASSERT_MSG_EQ (refcount, 2, "Anchor map should have 2 refcount");
}
EndTest ()

BeginTest (TestRemoveAnchor_1refcount)
{
  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=apple/name=pie");
  nfp->AddAnchorPrefix (prefix);

  nfp->RemoveAnchorPrefix (prefix);

  size_t mapsize = theFriend.GetAnchorMapSize ();
  NS_TEST_ASSERT_MSG_EQ (mapsize, 0, "Anchor map should have 0 entry");
}
EndTest ()

BeginTest (TestRemoveAnchor_2refcount)
{
  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=apple/name=pie");
  nfp->AddAnchorPrefix (prefix);
  nfp->AddAnchorPrefix (prefix);
  nfp->RemoveAnchorPrefix (prefix);

  size_t mapsize = theFriend.GetAnchorMapSize ();
  NS_TEST_ASSERT_MSG_EQ (mapsize, 1, "Anchor map should have 1 entry");

  uint32_t refcount = theFriend.GetAnchorCount (prefix);
  NS_TEST_ASSERT_MSG_EQ (refcount, 1, "Anchor map should have 1 refcount");
}
EndTest ()

BeginTest (TestSetTimer_NoJitter)
{
  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Time jitter (0);
  Time interval (Seconds (1));
  Timer timer;
  timer.SetFunction (&TestSetTimer_NoJitter::TimerExpire, this);
  theFriend.SetTimer (timer, interval, jitter);

  NS_TEST_ASSERT_MSG_LT (timer.GetDelay (), interval,
                         "Timer value should be less than interval");
  timer.Cancel ();
}

void
TimerExpire (void)
{
}
EndTest ()

BeginTest (TestSetTimer_WithJitter)
{
  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Time jitter (MilliSeconds (50));
  Time interval (Seconds (1));
  Timer timer;
  timer.SetFunction (&TestSetTimer_WithJitter::TimerExpire, this);
  theFriend.SetTimer (timer, interval, jitter);

  NS_TEST_ASSERT_MSG_LT (timer.GetDelay (), interval,
                         "Timer value should be less than interval");
  timer.Cancel ();
}
void
TimerExpire (void)
{
}
EndTest ()

BeginTest (TestDoInitialize)
{

  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);

  Simulator::Stop (MicroSeconds (1));
  Simulator::Run ();
  NS_TEST_ASSERT_MSG_EQ (theFriend.GetHelloTimer ()->IsRunning (), true,
                         "Hello timer not running");
}
EndTest ()

BeginTest (TestCreatePayload)
{
  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Ptr<NfpPayload> payload = theFriend.CreatePayload ();
  bool exists = (payload);

  NS_TEST_ASSERT_MSG_EQ (exists, true, "Got empty payload");

  Ptr<const CCNxName> expectedName = theFriend.GetRouterName ();
  Ptr<const CCNxName> testName = payload->GetRouterName ();
  NS_TEST_ASSERT_MSG_EQ (expectedName->Equals (*testName), true,
                         "Wrong name in payload");

  NS_TEST_ASSERT_MSG_EQ (payload->GetMessageCount (), 0,
                         "Wrong message count");
}
EndTest ()

BeginTest (TestCreatePacket)
{
  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (Create<Node> ());
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Ptr<CCNxPacket> packet = theFriend.CreatePacket (
      theFriend.CreatePayload ());
  bool exists = (packet);
  NS_TEST_ASSERT_MSG_EQ (exists, true, "Got empty packet");

  NS_TEST_ASSERT_MSG_EQ (packet->GetFixedHeader ()->GetPacketType (),
                         CCNxFixedHeaderType_Interest, "Wrong packet type");

  Ptr<CCNxMessage> message = packet->GetMessage ();
  NS_TEST_ASSERT_MSG_EQ (message->GetMessageType (), CCNxMessage::Interest,
                         "Wrong message type");

  Ptr<const CCNxName> name = message->GetName ();
  NS_TEST_ASSERT_MSG_EQ (name->Equals (*theFriend.GetNfpPrefix ()), true,
                         "Wrong name in Interest");
}
EndTest ()

/**
 * Add an interface to L3 and verify that we get the Up notification.
 */
BeginTest (TestNotifyInterfaceUp)
{
  Ptr<Node> node = Create<Node> ();

  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (node);
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Ptr<VirtualNetDevice> if0 = SetupTapDevice (
      node, 0, MakeCallback (&TestNotifyInterfaceUp::SendCallback, this));

  std::map<uint32_t, ns3::Ptr<ns3::ccnx::CCNxL3Interface> > *theList = theFriend.GetInterfaces ();

  NS_TEST_EXPECT_MSG_EQ (theList->size (), 1, "Expected 1 interface");
}

bool
SendCallback (Ptr<Packet> packet, const Address& source,
              const Address& dest, uint16_t protocolNumber)
{
  return false;
}

EndTest ()

BeginTest (TestBroadcast)
{
  Ptr<Node> node = Create<Node> ();

  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (node);
  NfpRoutingProtocolFriend theFriend (nfp);

  // To broadcast a packet, we need to add at least two interfaces to L3 so
  // the routing protocol know what to send to.  We add two virtual devices
  // whose output will be saved in m_sendQueue deque.
  // These calls to SetupTapDevice will add them to the L3 protocol which should
  // result in the NotifyInterfaceUp() method being called.

  Ptr<VirtualNetDevice> if0 = SetupTapDevice (
      node, 0, MakeCallback (&TestBroadcast::SendCallback, this));
  Ptr<VirtualNetDevice> if1 = SetupTapDevice (
      node, 1, MakeCallback (&TestBroadcast::SendCallback, this));

  // the simulator needs to be running so DoInitialize is called
  Simulator::Stop (MicroSeconds (1));
  Simulator::Run ();

  // now try to broadcast the packet
  Ptr<CCNxPacket> packet = theFriend.CreatePacket (
      theFriend.CreatePayload ());
  theFriend.Broadcast (packet);

  // Give it time to delver the packets
  // the simulator needs to be running so DoInitialize is called
  Simulator::Stop (MicroSeconds (10));
  Simulator::Run ();

  // we should now see the packet in the two output queues.

  NS_TEST_EXPECT_MSG_EQ (m_sendQueue.size (), 2, "Queue is wrong size");
}

// the packets queued in the SendCallback for us to read.
// The pair is (Ptr<Packet>, Destination Address).
typedef std::pair<Ptr<Packet>, Address> PairType;
std::deque<PairType> m_sendQueue;

bool
SendCallback (Ptr<Packet> packet, const Address& source,
              const Address& dest, uint16_t protocolNumber)
{
  std::cout << "TestBroadcast::SendCallback(" << packet << ", " << source
            << ", " << dest << ", 0x" << std::hex << protocolNumber << std::dec << ")"
            << std::endl;
  m_sendQueue.push_back (PairType (packet, dest));
  return true;
}
EndTest ()

BeginTest (TestReceiveHello_FirstTime)
{
  // receive a hello message for a neighbor for the first time
  Ptr<Node> node = Create<Node> ();

  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (node);
  NfpRoutingProtocolFriend theFriend (nfp);
  theFriend.DoInitialize ();

  Ptr<const CCNxName> neighborName = Create<CCNxName> ("ccnx:/name=martian");

  uint16_t msgSeqnum = 1;
  Ptr<CCNxVirtualConnection> ingressConnection = Create<CCNxVirtualConnection> (99);

  bool result = theFriend.ReceiveHello (neighborName, msgSeqnum, ingressConnection);
  NS_TEST_EXPECT_MSG_EQ (result, true, "Should have accepted a new neighbor advertisement");

  // It should now be in our neighbor table
  NfpRoutingProtocolFriend::NeighborMapType neighbors = theFriend.GetNeighbors ();
  NfpRoutingProtocolFriend::NeighborMapType::iterator i = neighbors.find (Create<NfpNeighborKey> (neighborName, ingressConnection));

  NS_TEST_EXPECT_MSG_EQ (
    (i == neighbors.end ()), false,
    "Could not find neighbor that should be in neighbor table");

  uint16_t testSeqnum = i->second->GetMessageSeqnum ();
  NS_TEST_EXPECT_MSG_EQ (msgSeqnum, testSeqnum, "neighbors seqnum is wrong");
}
EndTest ()

BeginTest (TestReceiveHello_OldSeqnum)
{
  // receive a hello message for a neighbor for the first time
  Ptr<Node> node = Create<Node> ();

  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (node);
  NfpRoutingProtocolFriend theFriend (nfp);

  Ptr<const CCNxName> neighborName = Create<CCNxName> ("ccnx:/name=martian");

  uint16_t msgSeqnum = 100;
  Ptr<CCNxVirtualConnection> ingressConnection = Create<CCNxVirtualConnection> (99);
  theFriend.ReceiveHello (neighborName, msgSeqnum, ingressConnection);

  // Now recieve another with an old seqnum
  bool result = theFriend.ReceiveHello (neighborName, msgSeqnum - 1, ingressConnection);
  NS_TEST_EXPECT_MSG_EQ (result, false, "Should have rejected a stale neighbor advertisement");

  // It should now be in our neighbor table
  NfpRoutingProtocolFriend::NeighborMapType neighbors = theFriend.GetNeighbors ();
  NfpRoutingProtocolFriend::NeighborMapType::iterator i = neighbors.find (Create<NfpNeighborKey> (neighborName, ingressConnection));

  // make sure we still have the right seqnum in cache
  uint16_t testSeqnum = i->second->GetMessageSeqnum ();
  NS_TEST_EXPECT_MSG_EQ (msgSeqnum, testSeqnum, "neighbors seqnum is wrong");
}
EndTest ()

BeginTest (TestReceiveHello_Update)
{
  // receive a hello message for a neighbor for the first time
  Ptr<Node> node = Create<Node> ();

  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (node);
  NfpRoutingProtocolFriend theFriend (nfp);

  Ptr<const CCNxName> neighborName = Create<CCNxName> ("ccnx:/name=martian");

  uint16_t msgSeqnum = 100;
  Ptr<CCNxVirtualConnection> ingressConnection = Create<CCNxVirtualConnection> (99);
  theFriend.ReceiveHello (neighborName, msgSeqnum, ingressConnection);

  // Now recieve another with an old seqnum
  bool result = theFriend.ReceiveHello (neighborName, msgSeqnum + 1, ingressConnection);
  NS_TEST_EXPECT_MSG_EQ (
    result, true,
    "Should have accepted an update neighbor advertisement");

  // It should now be in our neighbor table
  NfpRoutingProtocolFriend::NeighborMapType neighbors = theFriend.GetNeighbors ();
  NfpRoutingProtocolFriend::NeighborMapType::iterator i = neighbors.find (Create<NfpNeighborKey> (neighborName, ingressConnection));

  // make sure we still have the right seqnum in cache
  uint16_t testSeqnum = i->second->GetMessageSeqnum ();
  NS_TEST_EXPECT_MSG_EQ (msgSeqnum + 1, testSeqnum, "neighbors seqnum is wrong");
}
EndTest ()

BeginTest (TestReceiveNotify_Hello)
{
  // Send a packet up a virtual network device with a Hello
  Ptr<Node> node = Create<Node> ();

  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (node);
  NfpRoutingProtocolFriend theFriend (nfp);

  Ptr<VirtualNetDevice> if0 = SetupTapDevice (node, 0, MakeCallback (&TestReceiveNotify_Hello::SendCallback, this));

  // the simulator needs to be running so DoInitialize is called
  Simulator::Stop (MicroSeconds (1));
  Simulator::Run ();

  // now try to broadcast the packet
  Ptr<const CCNxName> neighborName = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpPayload> payload = Create<NfpPayload> (1500, neighborName, 100);
  Ptr<CCNxPacket> packet = theFriend.CreatePacket (payload);

  Address source = Mac48Address ("11:00:00:00:11:11");
  Address dest = Mac48Address ("ff:ff:ff:ff:ff:ff");

  Simulator::ScheduleWithContext (node->GetId (), Time (0),
                                  &VirtualNetDevice::Receive, if0,
                                  packet->CreateNs3Packet (),
                                  CCNxL3Protocol::GetProtocolNumber (),
                                  source, dest,
                                  NetDevice::PACKET_BROADCAST);
  Simulator::Stop (MicroSeconds (5));
  Simulator::Run ();

  // TODO: HACK we need to get this number from the Layer 3 protocol
  Ptr<CCNxVirtualConnection> ingressConnection = Create<CCNxVirtualConnection> (20);

  // It should now be in our neighbor table
  NfpRoutingProtocolFriend::NeighborMapType neighbors = theFriend.GetNeighbors ();
  NfpRoutingProtocolFriend::NeighborMapType::iterator i = neighbors.find (Create<NfpNeighborKey> (neighborName, ingressConnection));

  Ptr<OutputStreamWrapper> osw = Create<OutputStreamWrapper> (&std::cout);
  nfp->PrintNeighborTable (osw);

  bool found = (i != neighbors.end ());
  NS_TEST_EXPECT_MSG_EQ (
    found, true,
    "Could not find neighbor that should be in neighbor table");
}

bool
SendCallback (Ptr<Packet> packet, const Address& source,
              const Address& dest, uint16_t protocolNumber)
{
  // don't do anything, we're not testing downward behavior
  return true;
}

EndTest ()

BeginTest (TestReceivePayload_ForSelf)
{
  // receive a payload message with our name in it

}
EndTest ()

BeginTest (TestHelloExpired)
{
  // do not add any anchors and let the simulator free-run for a while.  We should see the
  // routing process emit Hello packets every HelloInterval.  We need to setup with a virtual
  // net device so we receive the hello packets.

  // Send a packet up a virtual network device with a Hello
  Ptr<Node> node = Create<Node> ();

  Ptr<NfpRoutingProtocol> nfp = CreateProtocol (node);
  NfpRoutingProtocolFriend theFriend (nfp);

  Ptr<VirtualNetDevice> if0 = SetupTapDevice (node, 0, MakeCallback (&TestHelloExpired::SendCallback, this));

  // The expected count is how many hello we expect to see.  There will be 1 sent immediately and
  // then other scheduled with delay.  So we wait until (expectedCount-1) * HelloInterval.  We also
  // add an extra millisecond at the end in the rare case the last hello is scheduled without jitter.
  size_t expectedCount = 5;
  Simulator::Stop (MilliSeconds ((expectedCount - 1) * nfp->GetHelloInterval ().GetMilliSeconds () + 1));
  Simulator::Run ();

  size_t actualCount = m_receiveQueue.size ();

  NS_TEST_EXPECT_MSG_EQ (actualCount, expectedCount, "Wrong number of hello packets");
}

std::deque<Ptr<Packet> > m_receiveQueue;

bool
SendCallback (Ptr<Packet> packet, const Address& source,
              const Address& dest, uint16_t protocolNumber)
{
  // save the packet so we can examine them in the unit test
  m_receiveQueue.push_back (packet);
  return true;
}
EndTest ()

// =================================
// Finally, define the TestSuite

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpRoutingProtocol
 */
static class TestSuiteNfpRoutingProtocol : public TestSuite
{
public:
  TestSuiteNfpRoutingProtocol () :
    TestSuite ("nfp-routing-protocol", UNIT)
  {
    AddTestCase (new TestGetTypeId (), TestCase::QUICK);
    AddTestCase (new TestAddAnchor_FirstTime (), TestCase::QUICK);
    AddTestCase (new TestAddAnchor_SecondTime (), TestCase::QUICK);
    AddTestCase (new TestRemoveAnchor_1refcount (), TestCase::QUICK);
    AddTestCase (new TestRemoveAnchor_2refcount (), TestCase::QUICK);
    AddTestCase (new TestSetTimer_NoJitter (), TestCase::QUICK);
    AddTestCase (new TestSetTimer_WithJitter (), TestCase::QUICK);
    AddTestCase (new TestDoInitialize (), TestCase::QUICK);

    AddTestCase (new TestCreatePayload (), TestCase::QUICK);
    AddTestCase (new TestCreatePacket (), TestCase::QUICK);
    AddTestCase (new TestNotifyInterfaceUp (), TestCase::QUICK);
    AddTestCase (new TestBroadcast (), TestCase::QUICK);

    AddTestCase (new TestReceiveHello_FirstTime (), TestCase::QUICK);
    AddTestCase (new TestReceiveHello_OldSeqnum (), TestCase::QUICK);
    AddTestCase (new TestReceiveHello_Update (), TestCase::QUICK);
    AddTestCase (new TestReceiveNotify_Hello (), TestCase::QUICK);
    AddTestCase (new TestHelloExpired (), TestCase::QUICK);

    //      AddTestCase (new TestReceivePayload_ForSelf (), TestCase::QUICK);
  }
} g_TestSuiteNfpRoutingProtocol;

} // namespace

