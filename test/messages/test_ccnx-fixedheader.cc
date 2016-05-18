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
#include "ns3/ccnx-codec-fixedheader.h"

#include <iostream>

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxFixedHeader {

static uint8_t g_version = 3;
static CCNxFixedHeaderType g_packetType = CCNxFixedHeaderType_Object;
static uint16_t g_packetLength = 0x1234;
static uint8_t g_hopLimit = 7;
static uint8_t g_returnCode = 99;
static uint8_t g_headerLength = 0xe7;

static const CCNxFixedHeader g_header (g_version, g_packetType, g_packetLength, g_hopLimit, g_returnCode, g_headerLength);

BeginTest (TestGetVersion)
{
  uint8_t test = g_header.GetVersion ();
  NS_TEST_EXPECT_MSG_EQ (g_version, test, "Wrong value");
}
EndTest ()

BeginTest (TestGetPacketType)
{
  CCNxFixedHeaderType test = g_header.GetPacketType ();
  NS_TEST_EXPECT_MSG_EQ (g_packetType, test, "Wrong value");
}
EndTest ()

BeginTest (TestGetPacketLength)
{
  uint16_t test = g_header.GetPacketLength ();
  NS_TEST_EXPECT_MSG_EQ (g_packetLength, test, "Wrong value");
}
EndTest ()

BeginTest (TestGetHopLimit)
{
  uint8_t test = g_header.GetHopLimit ();
  NS_TEST_EXPECT_MSG_EQ (g_hopLimit, test, "Wrong value");
}
EndTest ()

BeginTest (TestGetReturnCode)
{
  uint8_t test = g_header.GetReturnCode ();
  NS_TEST_EXPECT_MSG_EQ (g_returnCode, test, "Wrong value");
}
EndTest ()

BeginTest (TestGetHeaderLength)
{
  uint8_t test = g_header.GetHeaderLength ();
  NS_TEST_EXPECT_MSG_EQ (g_headerLength, test, "Wrong value");
}
EndTest ()

BeginTest (TestSetHopLimit)
{
  uint8_t hopLimit = g_hopLimit + 3;
  CCNxFixedHeader fh (g_header);
  fh.SetHopLimit (hopLimit);
  uint8_t test = fh.GetHopLimit ();
  NS_TEST_EXPECT_MSG_EQ (hopLimit, test, "Wrong value");
}
EndTest ()

BeginTest (TestSetPacketLength)
{
  uint16_t packetLength = g_packetLength + 3;
  CCNxFixedHeader fh (g_header);
  fh.SetPacketLength (packetLength);
  uint16_t test = fh.GetPacketLength ();
  NS_TEST_EXPECT_MSG_EQ (packetLength, test, "Wrong value");
}
EndTest ()

BeginTest (TestSetHeaderLength)
{
  uint8_t headerLength = g_headerLength + 3;
  CCNxFixedHeader fh (g_header);
  fh.SetHeaderLength (headerLength);
  uint8_t test = fh.GetHeaderLength ();
  NS_TEST_EXPECT_MSG_EQ (headerLength, test, "Wrong value");
}
EndTest ()

BeginTest (TestEquals)
{
  Ptr<CCNxFixedHeader> a = Create<CCNxFixedHeader> (g_header);
  Ptr<CCNxFixedHeader> b = Create<CCNxFixedHeader> (g_header);
  Ptr<CCNxFixedHeader> c = Create<CCNxFixedHeader> (g_header);
  Ptr<CCNxFixedHeader> x = Create<CCNxFixedHeader> (0, CCNxFixedHeaderType_Interest, 0, 0, 0, 0);

  // transitivity of equals
  NS_TEST_EXPECT_MSG_EQ (a->Equals (b), true, "Wrong value");
  NS_TEST_EXPECT_MSG_EQ (b->Equals (c), true, "Wrong value");
  NS_TEST_EXPECT_MSG_EQ (a->Equals (c), true, "Wrong value");

  // and the counter case
  NS_TEST_EXPECT_MSG_NE (x->Equals (a), true, "Wrong value");

  // Negative test
  NS_TEST_EXPECT_MSG_EQ (a->Equals (0), false, "null value");
}
EndTest ()

BeginTest (TestPrint)
{
  CCNxFixedHeader header (g_header);

  std::cout << header << std::endl;
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for TestSuiteCCNxFixedHeader
 */
static class TestSuiteCCNxFixedHeader : public TestSuite
{
public:
  TestSuiteCCNxFixedHeader () : TestSuite ("ccnx-fixedheader", UNIT)
  {
    AddTestCase (new TestGetVersion (), TestCase::QUICK);
    AddTestCase (new TestGetPacketType (), TestCase::QUICK);
    AddTestCase (new TestGetPacketLength (), TestCase::QUICK);
    AddTestCase (new TestGetHopLimit (), TestCase::QUICK);
    AddTestCase (new TestGetReturnCode (), TestCase::QUICK);
    AddTestCase (new TestGetHeaderLength (), TestCase::QUICK);

    AddTestCase (new TestSetHopLimit (), TestCase::QUICK);
    AddTestCase (new TestSetPacketLength (), TestCase::QUICK);
    AddTestCase (new TestSetHeaderLength (), TestCase::QUICK);

    AddTestCase (new TestEquals (), TestCase::QUICK);
    AddTestCase (new TestPrint (), TestCase::QUICK);
  }
} g_TestSuiteCCNxFixedHeader;

} // namespace TestSuiteCCNxFixedHeader

