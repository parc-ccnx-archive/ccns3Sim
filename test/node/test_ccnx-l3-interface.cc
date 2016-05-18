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

#include <map>
#include "ns3/test.h"
#include "ns3/ccnx-l3-interface.h"
#include "ns3/ccnx-connection-device.h"
#include "ns3/ccnx-routing-protocol.h"
#include "ns3/node.h"
#include "ns3/virtual-net-device.h"
#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxL3Interface {

static const int mtu = 1400;

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
* The MTU is set to `mtu` bytes.  If the callers wants to change the MTU, you should take
* the CCNxL3Interface down, change it, then bring it up again.  That will cause the change
* to be seen by the routing protocol.
*
* @param id	An ID for the virtual interface.  Will become part of the MAC address.
* @param send The callback from the virtual device to send a packet.
* @param node The node to install the device on.
*/
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

static bool
SendCallback (Ptr<Packet> packet, const Address& source,
              const Address& dest, uint16_t protocolNumber)
{
  return false;
}

class TestRoutingProtocol : public CCNxRoutingProtocol
{
public:
  static TypeId GetTypeId (void)
  {
    static TypeId tid = TypeId ("ns3::ccnx::TestSuiteCCNxL3Interface::TestRoutingProtocol")
      .SetParent<CCNxRoutingProtocol> ()
      .SetGroupName ("CCNx")
      .AddConstructor<TestRoutingProtocol> ();
    return tid;
  }

  std::map<uint32_t, bool> interfaceState;

  virtual void NotifyInterfaceUp (uint32_t interface)
  {
    interfaceState[interface] = true;
  }

  virtual void NotifyInterfaceDown (uint32_t interface)
  {
    interfaceState[interface] = false;
  }

  virtual void SetNode (Ptr<Node> node)
  {
  }

  virtual void AddAnchorPrefix (Ptr<const CCNxName> prefix)
  {
  }

  virtual void RemoveAnchorPrefix (Ptr<const CCNxName> prefix)
  {
  }

  virtual int64_t AssignStreams (int64_t stream)
  {
    return 0;
  }

  virtual void PrintNeighborTable (Ptr<OutputStreamWrapper> stream) const
  {
  }

  virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const
  {
  }

  virtual void PrintAnchorPrefixes (Ptr<OutputStreamWrapper> stream) const
  {
  }
};

BeginTest (Constructor)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  bool exists = (l3if);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null layer 3 interface");
}
EndTest ()

BeginTest (GetMtu)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  int test = l3if->GetMtu ();
  NS_TEST_EXPECT_MSG_EQ (test, mtu, "Got wrong MTU");
}
EndTest ()

BeginTest (GetIfIndex)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  uint32_t ifindex = l3if->GetIfIndex ();
  Ptr<NetDevice> test = node->GetDevice (ifindex);
  NS_TEST_EXPECT_MSG_EQ (test, device, "Got wrong device");
}
EndTest ()

BeginTest (IsUp_true)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  l3if->SetUp ();
  bool isUp = l3if->IsUp ();
  NS_TEST_EXPECT_MSG_EQ (isUp, true, "Should be UP after calling SetUp()");
}
EndTest ()

BeginTest (IsUp_false)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  bool isUp = l3if->IsUp ();
  NS_TEST_EXPECT_MSG_EQ (isUp, false, "Default state should be DOWN");
}
EndTest ()

BeginTest (SetUp_NoNotify)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  l3if->SetDown ();
  l3if->SetUp ();
  bool isUp = l3if->IsUp ();
  NS_TEST_EXPECT_MSG_EQ (isUp, true, "Should be UP after calling SetUp()");
}
EndTest ()

BeginTest (SetDown_NoNotify)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  l3if->SetDown ();
  bool isUp = l3if->IsUp ();
  NS_TEST_EXPECT_MSG_EQ (isUp, false, "Should be DOWN after calling SetDown()");
}
EndTest ()

/*
 * Add a routing protocol and make sure it gets notified of the state change
 */
BeginTest (SetUp_Notify)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);

  Ptr<TestRoutingProtocol> routing = CreateObject<TestRoutingProtocol> ();
  node->AggregateObject (routing);

  l3if->SetUp ();
  bool notifyWasCalled = routing->interfaceState.find (l3if->GetIfIndex ()) != routing->interfaceState.end ();
  NS_TEST_EXPECT_MSG_EQ (notifyWasCalled, true, "Routing protocol was not called for the interface index");

  bool isUp = routing->interfaceState[l3if->GetIfIndex ()];
  NS_TEST_EXPECT_MSG_EQ (isUp, true, "Routing protocol did not set the interface up");
}
EndTest ()

/*
 * Add a routing protocol and make sure it gets notified of the state change
 */
BeginTest (SetDown_Notify)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);

  Ptr<TestRoutingProtocol> routing = CreateObject<TestRoutingProtocol> ();
  node->AggregateObject (routing);

  l3if->SetDown ();
  bool notifyWasCalled = routing->interfaceState.find (l3if->GetIfIndex ()) != routing->interfaceState.end ();
  NS_TEST_EXPECT_MSG_EQ (notifyWasCalled, true, "Routing protocol was not called for the interface index");

  bool isUp = routing->interfaceState[l3if->GetIfIndex ()];
  NS_TEST_EXPECT_MSG_EQ (isUp, false, "Routing protocol did not set the interface down");
}
EndTest ()
BeginTest (IsForwarding)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  bool isForwarding = l3if->IsForwarding ();
  NS_TEST_EXPECT_MSG_EQ (isForwarding, false, "Default state should be NOT FORWARDING");
}
EndTest ()

BeginTest (SetForwarding_true)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  l3if->SetForwarding (true);
  bool isForwarding = l3if->IsForwarding ();
  NS_TEST_EXPECT_MSG_EQ (isForwarding, true, "State should be FORWARDING");
}
EndTest ()

BeginTest (SetForwarding_false)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);
  l3if->SetForwarding (true);
  l3if->SetForwarding (false);
  bool isForwarding = l3if->IsForwarding ();
  NS_TEST_EXPECT_MSG_EQ (isForwarding, false, "State should be NOT FORWARDING");
}
EndTest ()

BeginTest (SetBroadcastConnection)
{
  Ptr<Node> node = Create<Node> ();
  Ptr<VirtualNetDevice> device = SetupTapDevice (node, 1, MakeCallback (&SendCallback));
  Ptr<CCNxL3Interface> l3if = Create<CCNxL3Interface> (device);

  Ptr<CCNxConnectionDevice> bcast = Create<CCNxConnectionDevice> ();
  l3if->SetBroadcastConnection (bcast);

  Ptr<CCNxConnection> test = l3if->GetBroadcastConnection ();
  NS_TEST_EXPECT_MSG_EQ (test, bcast, "Did not get back pointer to same object");
}
EndTest ()


/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxTime
 */
static class TestSuiteCCNxL3Interface : public TestSuite
{
public:
  TestSuiteCCNxL3Interface () : TestSuite ("ccnx-l3-interface", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);

    AddTestCase (new GetMtu (), TestCase::QUICK);
    AddTestCase (new GetIfIndex (), TestCase::QUICK);
    AddTestCase (new IsUp_true (), TestCase::QUICK);
    AddTestCase (new IsUp_false (), TestCase::QUICK);
    AddTestCase (new SetUp_NoNotify (), TestCase::QUICK);
    AddTestCase (new SetDown_NoNotify (), TestCase::QUICK);
    AddTestCase (new SetUp_Notify (), TestCase::QUICK);
    AddTestCase (new SetDown_Notify (), TestCase::QUICK);
    AddTestCase (new IsForwarding (), TestCase::QUICK);
    AddTestCase (new SetForwarding_true (), TestCase::QUICK);
    AddTestCase (new SetForwarding_false (), TestCase::QUICK);
    AddTestCase (new SetBroadcastConnection (), TestCase::QUICK);
  }
} g_TestSuiteCCNxL3Interface;

} // namespace TestSuiteCCNxL3Interface
