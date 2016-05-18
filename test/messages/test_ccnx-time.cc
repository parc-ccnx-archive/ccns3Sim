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
#include "ns3/ccnx-time.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxTime {

BeginTest (Constructor_Value)
{
  uint64_t truth = 1235;
  CCNxTime *t = new CCNxTime (truth);

  NS_TEST_EXPECT_MSG_NE (t, NULL, "Constructor returned NULL");
  NS_TEST_EXPECT_MSG_EQ (truth, t->getTime (), "Value wrong");
}
EndTest ()

BeginTest (Constructor_CCNxTime)
{
  uint64_t truth = 1235;
  CCNxTime *t1 = new CCNxTime (truth);

  NS_TEST_EXPECT_MSG_NE (t1, NULL, "Constructor returned NULL");

  CCNxTime *t2 = new CCNxTime (*t1);
  NS_TEST_EXPECT_MSG_NE (t2, NULL, "Constructor returned NULL");

  NS_TEST_EXPECT_MSG_EQ (t1->getTime (), t2->getTime (), "Value wrong");
}
EndTest ()

BeginTest (TestEquals)
{
  Ptr<CCNxTime> a = Create<CCNxTime> (3600);
  Ptr<CCNxTime> b = Create<CCNxTime> (3600);
  Ptr<CCNxTime> c = Create<CCNxTime> (3600);

  Ptr<CCNxTime> w = Create<CCNxTime> (5600);

  // transitivity of equals
  NS_TEST_EXPECT_MSG_EQ (a->Equals (b), true, "not equal");
  NS_TEST_EXPECT_MSG_EQ (b->Equals (c), true, "not equal");
  NS_TEST_EXPECT_MSG_EQ (c->Equals (a), true, "not equal");

  // and the counter case
  NS_TEST_EXPECT_MSG_EQ (a->Equals (w), false, "not equal");

  // Negative test
  Ptr<CCNxTime> t = 0;
  NS_TEST_EXPECT_MSG_EQ (w->Equals (t), false, "not equal");
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxTime
 */
static class TestSuiteCCNxTime : public TestSuite
{
public:
  TestSuiteCCNxTime () : TestSuite ("ccnx-time", UNIT)
  {
    AddTestCase (new Constructor_Value (), TestCase::QUICK);
    AddTestCase (new Constructor_CCNxTime (), TestCase::QUICK);
    AddTestCase (new TestEquals (), TestCase::QUICK);
  }
} g_TestSuiteCCNxTime;

} // namespace TestSuiteCCNxTime
