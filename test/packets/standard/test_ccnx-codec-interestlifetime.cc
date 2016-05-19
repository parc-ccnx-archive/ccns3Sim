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
#include "ns3/ccnx-codec-interestlifetime.h"

#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxCodecInterestLifetime {

BeginTest (GetSerializedSize)
{
  Ptr<CCNxTime> time = Create<CCNxTime>(3600);
  Ptr<CCNxInterestLifetime> interestLifetime = Create<CCNxInterestLifetime> (time);

  // expected size = 4 (TL) + 2 (bytes needed to accommodate time value)
  size_t expectedSize = 4 + 2;

  CCNxCodecInterestLifetime ci;
  size_t test = ci.GetSerializedSize (interestLifetime);
  NS_TEST_EXPECT_MSG_EQ (test, expectedSize, "wrong size");
}
EndTest()

BeginTest (Serialize)
{
  Ptr<CCNxInterestLifetime> interestLifetime = Create<CCNxInterestLifetime> (Create<CCNxTime>(3600));

  CCNxCodecInterestLifetime ci;

  Buffer buffer (0);
  buffer.AddAtStart (ci.GetSerializedSize (interestLifetime));
  Buffer::Iterator iterator = buffer.Begin ();
  ci.Serialize (interestLifetime, &iterator);

  NS_TEST_EXPECT_MSG_EQ (buffer.GetSize (), ci.GetSerializedSize (interestLifetime), "Wrong size");

  const uint8_t truth[] = {
    // T_INT_LIFE
    0, 1, 0, 2,
    0xE, 0x10
  };

  uint8_t test[sizeof(truth)];
  buffer.CopyData (test, sizeof(truth));

  hexdump ("truth", sizeof(truth), truth);
  hexdump ("test ", sizeof(test), test);

  NS_TEST_EXPECT_MSG_EQ (memcmp (truth, test, sizeof(truth)), 0, "Data in buffer wrong");
}
EndTest()

BeginTest (Deserialize)
{
  // serialize it into a buffer
  Ptr<CCNxTime> time = Create<CCNxTime>(3600);
  Ptr<CCNxInterestLifetime> interestLifetime = Create<CCNxInterestLifetime> (time);

  CCNxCodecInterestLifetime ci;

  Buffer buffer (0);
  buffer.AddAtStart (ci.GetSerializedSize (interestLifetime));
  Buffer::Iterator iterator = buffer.Begin ();
  ci.Serialize (interestLifetime, &iterator);

  ci.Print(interestLifetime, std::cout);

  // now de-serialize and compare
  CCNxCodecInterestLifetime citest;
  size_t bytesRead = 0;
  Buffer::Iterator iter = buffer.Begin ();
  Ptr<CCNxPerHopHeaderEntry> perHopEntry = citest.Deserialize (&iter, &bytesRead);
  Ptr<CCNxInterestLifetime> test = DynamicCast<CCNxInterestLifetime, CCNxPerHopHeaderEntry> (perHopEntry);

  bool equal = interestLifetime->Equals (test);
  NS_TEST_EXPECT_MSG_EQ (equal, true, "Data in buffer wrong");
}
EndTest()

BeginTest (ComputeVarIntLength)
{
  Ptr<CCNxInterestLifetime> interestLifetime1 = Create<CCNxInterestLifetime> (Create<CCNxTime>(1152921504606846976));
  // expected size = 4 (TL) + 8 (bytes needed to accommodate time value)
  size_t expectedSize1 = 4 + 8;
  CCNxCodecInterestLifetime c1;
  size_t test1 = c1.GetSerializedSize (interestLifetime1);
  NS_TEST_EXPECT_MSG_EQ (test1, expectedSize1, "wrong size");

  Ptr<CCNxInterestLifetime> interestLifetime2 = Create<CCNxInterestLifetime> (Create<CCNxTime>(17592186044416));
  // expected size = 4 (TL) + 6 (bytes needed to accommodate time value)
  size_t expectedSize2 = 4 + 6;
  CCNxCodecInterestLifetime c2;
  size_t test2 = c2.GetSerializedSize (interestLifetime2);
  NS_TEST_EXPECT_MSG_EQ (test2, expectedSize2, "wrong size");
}
EndTest()

BeginTest (PrintEmpty)
{
  CCNxCodecInterestLifetime ci;
  ci.Print(0, std::cout);
}
EndTest()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for TestSuiteCCNxCodecInterestLifetime
 */
static class TestSuiteCCNxCodecInterestLifetime : public TestSuite
{
public:
  TestSuiteCCNxCodecInterestLifetime () : TestSuite ("ccnx-codec-interestlifetime", UNIT)
  {
    AddTestCase (new GetSerializedSize (), TestCase::QUICK);
    AddTestCase (new Serialize (), TestCase::QUICK);
    AddTestCase (new Deserialize (), TestCase::QUICK);
    AddTestCase (new ComputeVarIntLength (), TestCase::QUICK);
    AddTestCase (new PrintEmpty (), TestCase::QUICK);
  }
} g_TestSuiteCCNxCodecInterestLifetime;

} // namespace TestSuiteCCNxCodecInterestLifetime
