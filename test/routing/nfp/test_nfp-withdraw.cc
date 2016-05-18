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

#include "ns3/nfp-withdraw.h"
#include "ns3/ccnx-codec-name.h"
#include "ns3/ccnx-tlv.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpWithdrawTests {

class TestState
{
public:
  Ptr<const CCNxName> anchorName;
  Ptr<const CCNxName> prefix;
  CCNxCodecName anchorNameCodec;
  CCNxCodecName prefixCodec;

  TestState ()
  {
    anchorName = Create<CCNxName> ("ccnx:/name=anchor_name");
    prefix = Create<CCNxName> ("ccnx:/name=parc.com/name=csl");

    anchorNameCodec.SetHeader (anchorName);
    prefixCodec.SetHeader (prefix);
  }
};

BeginTest (TestConstructor_NoArg)
{
  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> ();
  // invoke the test operator ()
  bool success = (withdraw);
  NS_TEST_ASSERT_MSG_EQ (success, true, "Got null NfpWithdraw");
}
EndTest ()

BeginTest (TestConstructor_Args)
{
  TestState state;
  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> (state.anchorName, state.prefix);

  bool success = (withdraw);
  NS_TEST_ASSERT_MSG_EQ (success, true, "Got null NfpWithdraw");
}
EndTest ()

BeginTest (TestGetAnchorName)
{
  TestState state;
  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> (state.anchorName, state.prefix);
  Ptr<const CCNxName> anchorName = withdraw->GetAnchorName ();
  NS_TEST_ASSERT_MSG_EQ (state.anchorName, anchorName, "anchorName not set correctly");
}
EndTest ()

BeginTest (TestGetPrefix)
{
  TestState state;
  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> (state.anchorName, state.prefix);
  Ptr<const CCNxName> prefix = withdraw->GetPrefix ();
  NS_TEST_ASSERT_MSG_EQ (state.prefix, prefix, "prefix not set correctly");
}
EndTest ()

BeginTest (TestGetSerializedSize)
{
  TestState state;

  uint32_t expected = 0;
  expected += CCNxTlv::GetTLSize ();        // The T_WITHDRAW header
  expected += state.anchorNameCodec.GetSerializedSize ();
  expected += state.prefixCodec.GetSerializedSize ();

  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> (state.anchorName, state.prefix);
  uint32_t actual = withdraw->GetSerializedSize ();
  NS_TEST_ASSERT_MSG_EQ (expected, actual, "GetSerializedSize() incorrect");
}
EndTest ()

BeginTest (TestSerialize)
{
  TestState state;
  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> (state.anchorName, state.prefix);

  uint8_t expected[] = {
    0, 3, 0, 42,
    0, 0, 0, 15,
    0, 1, 0, 11, 'a', 'n', 'c', 'h', 'o', 'r', '_', 'n', 'a', 'm', 'e',
    0, 0, 0, 19,
    0, 1, 0, 8, 'p', 'a', 'r', 'c', '.', 'c', 'o', 'm',
    0, 1, 0, 3, 'c', 's', 'l'
  };

  Buffer actual (0);
  actual.AddAtStart (withdraw->GetSerializedSize ());
  withdraw->Serialize (actual.Begin ());

  int result = memcmp (expected, actual.PeekData (), sizeof (expected));
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
  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> (state.anchorName, state.prefix);

  Buffer truth (0);
  truth.AddAtStart (withdraw->GetSerializedSize ());
  withdraw->Serialize (truth.Begin ());

  Ptr<NfpWithdraw> test = Create<NfpWithdraw> ();
  test->Deserialize (truth.Begin ());

  NS_TEST_ASSERT_MSG_EQ (withdraw->Equals (*test), true, "Incorrect deserialize");
}
EndTest ()

BeginTest (TestEquals)
{
  Ptr<const CCNxName> base_anchor = Create<CCNxName> ("ccnx:/name=seizure");
  Ptr<const CCNxName> base_prefix = Create<CCNxName> ("ccnx:/name=grab");
  Ptr<NfpWithdraw> base = Create<NfpWithdraw> (base_anchor, base_prefix);

  struct
  {
    const char *anchor;
    const char *prefix;
    bool result;
  } testVectors[] = {
    { "ccnx:/name=apple", "ccnx:/name=berry", false},
    { "ccnx:/name=apple", "ccnx:/name=grab", false},
    { "ccnx:/name=seizure", "ccnx:/name=crepe", false},
    { "ccnx:/name=seizure", "ccnx:/name=grab", true},
    { NULL, NULL, false}
  };

  for (int i = 0; testVectors[i].anchor != NULL; i++)
    {
      Ptr<const CCNxName> anchor = Create<CCNxName> (testVectors[i].anchor);
      Ptr<const CCNxName> prefix = Create<CCNxName> (testVectors[i].prefix);
      Ptr<NfpWithdraw> other = Create<NfpWithdraw> (anchor, prefix);
      bool test = (base->Equals (*other));
      NS_TEST_ASSERT_MSG_EQ (testVectors[i].result, test, "Incorrect result");
    }
}
EndTest ()

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpRoutingProtocol
 */
static class TestSuiteNfpWithdraw : public TestSuite
{
public:
  TestSuiteNfpWithdraw () : TestSuite ("nfp-withdraw", UNIT)
  {
    AddTestCase (new TestConstructor_NoArg (), TestCase::QUICK);
    AddTestCase (new TestConstructor_Args (), TestCase::QUICK);
    AddTestCase (new TestGetAnchorName (), TestCase::QUICK);
    AddTestCase (new TestGetPrefix (), TestCase::QUICK);
    AddTestCase (new TestGetSerializedSize (), TestCase::QUICK);
    AddTestCase (new TestEquals (), TestCase::QUICK);
    AddTestCase (new TestSerialize (), TestCase::QUICK);
    AddTestCase (new TestDeserialize (), TestCase::QUICK);
  }
} g_TestSuiteNfpWithdraw;
}

