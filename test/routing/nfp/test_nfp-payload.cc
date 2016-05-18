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
#include "../../TestMacros.h"

#include "ns3/log.h"
#include "ns3/nfp-payload.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-codec-name.h"
#include "ns3/ccnx-tlv.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpPayloadTests {
class TestState
{
public:
  const char *routerNameString;
  uint16_t messageSeqnum;

  const char *anchorNameString;
  const char *prefixNameString_1;
  const char *prefixNameString_2;
  uint32_t anchorSeqnum_1;
  uint32_t anchorSeqnum_2;
  uint16_t prefixDistance_1;
  uint16_t prefixDistance_2;

  Ptr<const CCNxName> routerName;
  Ptr<const CCNxName> anchorName;
  Ptr<const CCNxName> prefixName_1;
  Ptr<const CCNxName> prefixName_2;

  CCNxCodecName routerNameCodec;

  Ptr<NfpAdvertise> adv1;
  Ptr<NfpAdvertise> adv2;
  Ptr<NfpWithdraw> withdraw;

  TestState ()
  {
    routerNameString = "ccnx:/name=router";
    messageSeqnum = 0x1234;

    anchorNameString = "ccnx:/name=anchor";
    prefixNameString_1 = "ccnx:/name=prefix/name=1";
    prefixNameString_2 = "ccnx:/name=prefix/name=2";
    anchorSeqnum_1 = 0x01020304;
    anchorSeqnum_2 = 0x05060708;
    prefixDistance_1 = 0x0a0b;
    prefixDistance_2 = 0x0c0d;

    routerName = Create<CCNxName> (routerNameString);
    anchorName = Create<CCNxName> (anchorNameString);
    prefixName_1 = Create<CCNxName> (prefixNameString_1);
    prefixName_2 = Create<CCNxName> (prefixNameString_2);

    routerNameCodec.SetHeader (routerName);

    adv1 = Create<NfpAdvertise> (anchorName, prefixName_1, anchorSeqnum_1,
                                 prefixDistance_1);
    adv2 = Create<NfpAdvertise> (anchorName, prefixName_2, anchorSeqnum_2,
                                 prefixDistance_2);
    withdraw = Create<NfpWithdraw> (anchorName, prefixName_1);
  }

  Ptr<NfpPayload>
  CreatePayload () const
  {
    Ptr<NfpPayload> payload = Create<NfpPayload> (1000, routerName,
                                                  messageSeqnum);
    payload->AppendMessage (adv1);
    payload->AppendMessage (adv2);
    payload->AppendMessage (withdraw);
    return payload;
  }
};

BeginTest (TestConstructor_Args)
{
  TestState state;
  Ptr<NfpPayload> advertise = state.CreatePayload ();

  bool success = (advertise);
  NS_TEST_ASSERT_MSG_EQ (success, true, "Got null NfpPayload");
}
EndTest ()

BeginTest (TestGetRouterName)
{
  TestState state;
  Ptr<NfpPayload> payload = state.CreatePayload ();
  Ptr<const CCNxName> routerName = payload->GetRouterName ();
  NS_TEST_ASSERT_MSG_EQ (state.routerName, routerName,
                         "routerName not set correctly");
}
EndTest ()

BeginTest (TestGetMessageSeqnum)
{
  TestState state;
  Ptr<NfpPayload> payload = state.CreatePayload ();
  uint16_t seqnum = payload->GetMessageSeqnum ();
  NS_TEST_ASSERT_MSG_EQ (state.messageSeqnum, seqnum,
                         "messageSeqnum not set correctly");
}
EndTest ()

BeginTest (TestGetMessageCount)
{
  TestState state;
  Ptr<NfpPayload> payload = state.CreatePayload ();
  size_t count = payload->GetMessageCount ();
  NS_TEST_ASSERT_MSG_EQ (count, 3, "GetMessageCount incorrect");
}
EndTest ()

BeginTest (TestGetMessage_Index1)
{
  TestState state;
  Ptr<NfpPayload> payload = state.CreatePayload ();
  Ptr<NfpMessage> message = payload->GetMessage (1);

  NS_TEST_ASSERT_MSG_EQ (state.adv2->Equals (*message), true,
                         "GetMessage[1] incorrect");
}
EndTest ()

BeginTest (TestGetSerializedSize)
{
  TestState state;

  uint32_t expected = 0;
  expected += CCNxTlv::GetTLSize ();      // The T_MSG header
  expected += state.routerNameCodec.GetSerializedSize ();
  expected += CCNxTlv::GetTLSize () + 2;
  expected += state.adv1->GetSerializedSize ();
  expected += state.adv2->GetSerializedSize ();
  expected += state.withdraw->GetSerializedSize ();

  Ptr<NfpPayload> payload = state.CreatePayload ();
  uint32_t actual = payload->GetSerializedSize ();
  NS_TEST_ASSERT_MSG_EQ (expected, actual, "GetSerializedSize() incorrect");
}
EndTest ()

BeginTest (TestSerialize)
{
  TestState state;
  Ptr<NfpPayload> payload = state.CreatePayload ();

  uint8_t expected[] = {
    // msg header
    0, 4, 0,151,
    0, 0, 0, 10,
    0, 1, 0,  6, 'r', 'o', 'u', 't', 'e', 'r',
    0, 5, 0,  2, 0x12, 0x34,
    // offset = 24
    // adv1
    0, 2, 0, 43,
    0, 0, 0, 10,
    0, 1, 0,  6, 'a', 'n', 'c', 'h', 'o', 'r',
    0, 0, 0, 15,
    0, 1, 0,  6, 'p', 'r', 'e', 'f', 'i', 'x',
    0, 1, 0,  1, '1',
    0, 6, 0,  6, 0x01, 0x02, 0x03, 0x04, 0x0a, 0x0b,
    // adv2
    0, 2, 0, 43,
    0, 0, 0, 10,
    0, 1, 0,  6, 'a', 'n', 'c', 'h', 'o', 'r',
    0, 0, 0, 15,
    0, 1, 0,  6, 'p', 'r', 'e', 'f', 'i', 'x',
    0, 1, 0,  1, '2',
    0, 6, 0,  6, 0x05, 0x06, 0x07, 0x08, 0x0c, 0x0d,
    // withdraw
    0, 3, 0, 33,
    0, 0, 0, 10,
    0, 1, 0,  6, 'a', 'n', 'c', 'h', 'o', 'r',
    0, 0, 0, 15,
    0, 1, 0,  6, 'p', 'r', 'e', 'f', 'i', 'x',
    0, 1, 0,  1, '1',
  };

  Buffer actual (0);
  actual.AddAtStart (payload->GetSerializedSize ());
  payload->Serialize (actual.Begin ());

  int result = memcmp (expected, actual.PeekData (), sizeof(expected));
  if (result != 0)
    {
      hexdump ("expected", sizeof(expected), expected);
      hexdump ("actual", actual.GetSize (), actual.PeekData ());
    }
  NS_TEST_ASSERT_MSG_EQ (result, 0, "Incorrect serialized form");
}
EndTest ()

BeginTest (TestDeserialize)
{
  TestState state;
  Ptr<NfpPayload> payload = state.CreatePayload ();

  Buffer truth (0);
  truth.AddAtStart (payload->GetSerializedSize ());
  payload->Serialize (truth.Begin ());

  Ptr<NfpPayload> test = Create<NfpPayload> ();
  test->Deserialize (truth.Begin ());

  NS_TEST_ASSERT_MSG_EQ (payload->Equals (*test), true, "Incorrect deserialize");
}
EndTest ()

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpPayload
 */
static class TestSuiteNfpPayload : public TestSuite
{
public:
  TestSuiteNfpPayload () :
    TestSuite ("nfp-payload", UNIT)
  {
    AddTestCase (new TestConstructor_Args (), TestCase::QUICK);
    AddTestCase (new TestGetRouterName (), TestCase::QUICK);
    AddTestCase (new TestGetMessageSeqnum (), TestCase::QUICK);
    AddTestCase (new TestGetMessageCount (), TestCase::QUICK);

    AddTestCase (new TestGetMessage_Index1 (), TestCase::QUICK);
    AddTestCase (new TestGetSerializedSize (), TestCase::QUICK);
    AddTestCase (new TestSerialize (), TestCase::QUICK);
    AddTestCase (new TestDeserialize (), TestCase::QUICK);
  }
} g_TestSuiteNfpPayload;
}

