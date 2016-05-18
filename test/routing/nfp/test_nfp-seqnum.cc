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

#include "ns3/log.h"
#include "ns3/test.h"
#include "../../TestMacros.h"

#include "ns3/nfp-seqnum.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpSeqnumTests {
BeginTest (TestUint16_0x0000)
{
  // Test sequence numbers against a base of 0
  uint16_t base = 0;
  struct test_vector
  {
    uint16_t value;
    int result;
    bool sentinel;
  } testVectors [] = {
    { 0x8001 + base, +1,   false },
    { 0x8002 + base, +1,   false },
    { 0xFFFE + base, +1,   false },
    { 0xFFFF + base, +1,   false },
    {      0 + base,  0,   false },
    {      1 + base, -1,   false },
    {      2 + base, -1,   false },
    { 0x7FFE + base, -1,   false },
    { 0x7FFF + base, -1,   false },
    { 0x8000 + base, -1,   false },
    {      0 + base,  0,   true }
  };

  for (int i = 0; !testVectors[i].sentinel; i++)
    {
      int compare = NfpSeqnum::Compare (base, testVectors[i].value);
      NS_TEST_EXPECT_MSG_EQ (compare, testVectors[i].result, "Failed test vector " << i);
    }
}
EndTest ()

BeginTest (TestUint16_0x1234)
{
  // Test sequence numbers against a base of 0
  uint16_t base = 0x1234;
  struct test_vector
  {
    uint16_t value;
    int result;
    bool sentinel;
  } testVectors [] = {
    { 0x8001 + base, +1,   false },
    { 0x8002 + base, +1,   false },
    { 0xFFFE + base, +1,   false },
    { 0xFFFF + base, +1,   false },
    {      0 + base,  0,   false },
    {      1 + base, -1,   false },
    {      2 + base, -1,   false },
    { 0x7FFE + base, -1,   false },
    { 0x7FFF + base, -1,   false },
    { 0x8000 + base, -1,   false },
    {      0 + base,  0,   true }
  };

  for (int i = 0; !testVectors[i].sentinel; i++)
    {
      int compare = NfpSeqnum::Compare (base, testVectors[i].value);
      NS_TEST_EXPECT_MSG_EQ (compare, testVectors[i].result, "Failed test vector " << i);
    }
}
EndTest ()

BeginTest (TestUint32_0x00000000)
{
  // Test sequence numbers against a base of 0
  uint32_t base = 0;
  struct test_vector
  {
    uint32_t value;
    int result;
    bool sentinel;
  } testVectors [] = {
    { 0x80000001 + base, +1,   false },
    { 0x80000002 + base, +1,   false },
    { 0xFFFFFFFE + base, +1,   false },
    { 0xFFFFFFFF + base, +1,   false },
    {          0 + base,  0,   false },
    {          1 + base, -1,   false },
    {          2 + base, -1,   false },
    { 0x7FFFFFFE + base, -1,   false },
    { 0x7FFFFFFF + base, -1,   false },
    { 0x80000000 + base, -1,   false },
    {          0 + base,  0,   true }
  };

  for (int i = 0; !testVectors[i].sentinel; i++)
    {
      int compare = NfpSeqnum::Compare (base, testVectors[i].value);
      NS_TEST_EXPECT_MSG_EQ (compare, testVectors[i].result, "Failed test vector " << i);
    }
}
EndTest ()

BeginTest (TestUint32_0xF0000000)
{
  // Test sequence numbers against a base of 0
  uint32_t base = 0xF0000000;
  struct test_vector
  {
    uint32_t value;
    int result;
    bool sentinel;
  } testVectors [] = {
    { 0x80000001 + base, +1,   false },
    { 0x80000002 + base, +1,   false },
    { 0xFFFFFFFE + base, +1,   false },
    { 0xFFFFFFFF + base, +1,   false },
    {          0 + base,  0,   false },
    {          1 + base, -1,   false },
    {          2 + base, -1,   false },
    { 0x7FFFFFFE + base, -1,   false },
    { 0x7FFFFFFF + base, -1,   false },
    { 0x80000000 + base, -1,   false },
    {          0 + base,  0,   true }
  };

  for (int i = 0; !testVectors[i].sentinel; i++)
    {
      int compare = NfpSeqnum::Compare (base, testVectors[i].value);
      NS_TEST_EXPECT_MSG_EQ (compare, testVectors[i].result, "Failed test vector " << i);
    }
}
EndTest ()

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpRoutingProtocol
 */
static class TestSuiteNfpSeqnum : public TestSuite
{
public:
  TestSuiteNfpSeqnum () : TestSuite ("nfp-seqnum", UNIT)
  {
    AddTestCase (new TestUint16_0x0000 (), TestCase::QUICK);
    AddTestCase (new TestUint16_0x1234 (), TestCase::QUICK);
    AddTestCase (new TestUint32_0x00000000 (), TestCase::QUICK);
    AddTestCase (new TestUint32_0xF0000000 (), TestCase::QUICK);
  }
} g_TestSuiteNfpSeqnum;
}

