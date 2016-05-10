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
 * # is up to the contributors to maintain their section in this file up to date
 * # and up to the user of the software to verify any claims herein.
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
#include "ns3/ccnx-hasher-sha256-sim.h"
#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxHasherSha256Sim {

BeginTest (Constructor)
{
  Ptr<CCNxHasherSha256Sim> hasher = CreateObject<CCNxHasherSha256Sim> ();
  bool exists = (hasher);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null hasher");
}
EndTest ()

BeginTest (Init)
{
  Ptr<CCNxHasherSha256Sim> hasher = CreateObject<CCNxHasherSha256Sim> ();
  hasher->Init ();

  Ptr<CCNxHashValue> truth = Create<CCNxHashValue> (0xCBF29CE484222325ULL, 32);
  Ptr<CCNxHashValue> test = hasher->Finalize ();

  bool equal = *truth == *test;
  NS_TEST_EXPECT_MSG_EQ (equal, true, "Got wrong inital value");
}
EndTest ()

BeginTest (UpdateBytes)
{
  Ptr<CCNxHasherSha256Sim> hasher = CreateObject<CCNxHasherSha256Sim> ();
  hasher->Init ();

  uint8_t buffer1[] = { 1, 2, 3, 4, 5 };
  uint8_t buffer2[] = { 3, 7, 11 };

  size_t length1 = hasher->Update (buffer1, sizeof(buffer1));
  NS_TEST_EXPECT_MSG_EQ (length1, sizeof(buffer1), "Got wrong length value");

  size_t length2 = hasher->Update (buffer2, sizeof(buffer2));
  NS_TEST_EXPECT_MSG_EQ (length2, sizeof(buffer2), "Got wrong length value");

  Ptr<CCNxHashValue> truth = Create<CCNxHashValue> (0x64bf6a8b27d8418bULL, 32);
  Ptr<CCNxHashValue> test = hasher->Finalize ();

  bool equal = *truth == *test;
  NS_TEST_EXPECT_MSG_EQ (equal, true, "Got wrong value: " << *test->GetValue () << " expected " << *truth->GetValue ());
}
EndTest ()

BeginTest (UpdateBuffer)
{
  Ptr<CCNxHasherSha256Sim> hasher = CreateObject<CCNxHasherSha256Sim> ();
  hasher->Init ();

  // Same data as used in UpdateBytes test
  uint8_t data[] = { 1, 2, 3, 4, 5, 3, 7, 11 };

  Buffer buffer;
  buffer.AddAtStart (sizeof(data));
  buffer.Begin ().Write (data, sizeof(data));

  size_t length1 = hasher->Update (buffer.Begin ());
  NS_TEST_EXPECT_MSG_EQ (length1, sizeof(data), "Got wrong length value");

  // This should be the same value as in UpdateBytes
  Ptr<CCNxHashValue> truth = Create<CCNxHashValue> (0x64bf6a8b27d8418bULL, 32);
  Ptr<CCNxHashValue> test = hasher->Finalize ();

  bool equal = *truth == *test;
  NS_TEST_EXPECT_MSG_EQ (equal, true, "Got wrong value: " << *test->GetValue () << " expected " << *truth->GetValue ());
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * @brief Test Suite for CCNxStandardFib
 */
static class TestSuiteCCNxHasherSha256Sim : public TestSuite
{
public:
  TestSuiteCCNxHasherSha256Sim () : TestSuite ("ccnx-hasher-sha256-sim", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new Init (), TestCase::QUICK);
    AddTestCase (new UpdateBytes (), TestCase::QUICK);
    AddTestCase (new UpdateBuffer (), TestCase::QUICK);
  }
} g_TestSuiteCCNxHasherSha256Sim;

} // namespace TestSuiteCCNxHasherSha256Sim




