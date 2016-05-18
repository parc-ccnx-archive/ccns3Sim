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
#include "ns3/ccnx-codec-interest.h"
#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

// =================================

static const uint8_t payloadBytes[] = "an apple a day";

static Ptr<CCNxBuffer>
CreatePayload (size_t length, const uint8_t *bytes)
{
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (0);
  payload->AddAtStart (length);
  payload->Begin ().Write ((uint8_t *) bytes, length);
  return payload;
}

namespace TestCCNxCodecInterest {

BeginTest (TestGetSerializedSize)
{
  printf ("TestGetSerializedSize DoRun\n");
  Ptr<const CCNxName> name = Create<CCNxName> ("ccnx:/name=apple/ver=pie/chunk=crust");

  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (77);
  Ptr<CCNxHashValue> hash  = Create<CCNxHashValue> (99);
  Ptr<CCNxBuffer> payload = CreatePayload (sizeof(payloadBytes), payloadBytes);

  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  CCNxCodecInterest ci;
  ci.SetHeader (interest);

  // expected size = 4 + 29 (name) + 36 + 36 + 4 (payload TLV) + payloadBytes
  size_t expectedSize = 4 + 29 + 36 + 36 + 4 + sizeof(payloadBytes);

  size_t test = ci.GetSerializedSize ();
  NS_TEST_EXPECT_MSG_EQ (test, expectedSize, "wrong size");
}
EndTest ()

BeginTest (TestSerialize)
{
  printf ("TestSerialize DoRun\n");
  Ptr<const CCNxName> name = Create<CCNxName> ("ccnx:/name=apple/ver=pie/chunk=crust");

  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (0x102030);
  Ptr<CCNxHashValue> hash  = Create<CCNxHashValue> (0x405060);
  Ptr<CCNxBuffer> payload = CreatePayload (sizeof(payloadBytes), payloadBytes);

  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  CCNxCodecInterest ci;
  ci.SetHeader (interest);

  Buffer buffer (0);
  buffer.AddAtStart (ci.GetSerializedSize ());
  ci.Serialize (buffer.Begin ());

  NS_TEST_EXPECT_MSG_EQ (buffer.GetSize (), ci.GetSerializedSize (), "Wrong size");

  const uint8_t truth[] = {
    // T_INTEREST
    0,  1, 0, 120,
    // T_NAME
    0,  0, 0, 25,
    0,  1, 0,  5, 'a', 'p', 'p', 'l', 'e',
    0, 19, 0,  3, 'p', 'i', 'e',
    0, 16, 0,  5, 'c', 'r', 'u', 's', 't',
    // T_KEYID_REST
    0,  2, 0, 32,
    0, 0, 0, 0, 0, 0x10, 0x20, 0x30, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0, 0,
    // T_HASH_REST
    0,  3, 0, 32,
    0, 0, 0, 0, 0, 0x40, 0x50, 0x60, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0, 0,
    // T_PAYLOAD
    0,  1, 0, 15,
    'a', 'n', ' ', 'a', 'p', 'p', 'l', 'e', ' ', 'a', ' ', 'd', 'a', 'y', 0
  };

  uint8_t test[sizeof(truth)];
  buffer.CopyData (test, sizeof(truth));

  hexdump ("truth", sizeof(truth), truth);
  hexdump ("test ", sizeof(test), test);

  NS_TEST_EXPECT_MSG_EQ (memcmp (truth, test, sizeof(truth)), 0, "Data in buffer wrong");
}
EndTest ()

BeginTest (TestDeserialize)
{
  printf ("TestDeserialize DoRun\n");

  // serialize it into a buffer
  Ptr<const CCNxName> name = Create<CCNxName> ("ccnx:/name=apple/ver=pie/chunk=crust");

  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (0x102030);
  Ptr<CCNxHashValue> hash  = Create<CCNxHashValue> (0x405060);
  Ptr<CCNxBuffer> payload = CreatePayload (sizeof(payloadBytes), payloadBytes);

  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  CCNxCodecInterest ci;
  ci.SetHeader (interest);

  Buffer buffer (0);
  buffer.AddAtStart (ci.GetSerializedSize ());
  ci.Serialize (buffer.Begin ());

  // now de-serialize and compare
  CCNxCodecInterest citest;
  citest.Deserialize (buffer.Begin ());

  Ptr<CCNxInterest> test = citest.GetHeader ();
  bool equal = interest->Equals (test);

  NS_TEST_EXPECT_MSG_EQ (equal, true, "Data in buffer wrong");
}
EndTest ()
}

// =================================
// Finally, define the TestSuite

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for CCNxCodecInterest
 */
static class TestSuiteCCNxCodecInterest : public TestSuite
{
public:
  TestSuiteCCNxCodecInterest () : TestSuite ("ccnx-codec-interest", UNIT)
  {
    AddTestCase (new TestCCNxCodecInterest::TestGetSerializedSize (), TestCase::QUICK);
    AddTestCase (new TestCCNxCodecInterest::TestSerialize (), TestCase::QUICK);
    AddTestCase (new TestCCNxCodecInterest::TestDeserialize (), TestCase::QUICK);
  }
} g_TestSuiteCCNxCodecInterest;

