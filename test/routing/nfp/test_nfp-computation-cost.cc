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

#include "ns3/nfp-computation-cost.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpComputationCostTests {

BeginTest (Constructor_NoArg)
{
  NfpComputationCost c;

  NS_TEST_ASSERT_MSG_EQ(c.GetEvents(), 0, "Wrong event count");
  NS_TEST_ASSERT_MSG_EQ(c.GetLoopIterations(), 0, "Wrong iterations count");
}
EndTest ()

BeginTest (Constructor_Copy)
{
  NfpComputationCost a;

  a.IncrementEvents();
  a.IncrementLoopIterations(3);

  NfpComputationCost b(a);

  NS_TEST_ASSERT_MSG_EQ(b.GetEvents(), 1, "Wrong event count");
  NS_TEST_ASSERT_MSG_EQ(b.GetLoopIterations(), 3, "Wrong iterations count");
}
EndTest ()

BeginTest (IncrementEvents)
{
  NfpComputationCost a;

  a.IncrementEvents();
  NS_TEST_ASSERT_MSG_EQ(a.GetEvents(), 1, "Wrong event count");
  NS_TEST_ASSERT_MSG_EQ(a.GetLoopIterations(), 0, "Wrong iterations count");
}
EndTest ()

BeginTest (IncrementLoopIterations)
{
  NfpComputationCost a;

  a.IncrementLoopIterations();
  NS_TEST_ASSERT_MSG_EQ(a.GetEvents(), 0, "Wrong event count");
  NS_TEST_ASSERT_MSG_EQ(a.GetLoopIterations(), 1, "Wrong iterations count");
}
EndTest ()

BeginTest (IncrementLoopIterationsCount)
{
  NfpComputationCost a;

  a.IncrementLoopIterations(7);
  NS_TEST_ASSERT_MSG_EQ(a.GetEvents(), 0, "Wrong event count");
  NS_TEST_ASSERT_MSG_EQ(a.GetLoopIterations(), 7, "Wrong iterations count");
}
EndTest ()

BeginTest (OperatorPlus)
{
}
EndTest ()

BeginTest (OperatorPlusEquals)
{
}
EndTest ()

BeginTest (OperatorEqualEqual)
{
  NfpComputationCost a;
  a.IncrementEvents();
  a.IncrementLoopIterations(2);
  // a = (1, 2)

  NfpComputationCost b(a);
  NfpComputationCost c(a);

  NfpComputationCost x(a);
  x.IncrementEvents();
  // x = (2,2)

  NfpComputationCost y(a);
  y.IncrementEvents();
  // y = (1, 3)

  NS_TEST_ASSERT_MSG_EQ(a, a, "a should equal a");
  NS_TEST_ASSERT_MSG_EQ(a, b, "a should equal b");
  NS_TEST_ASSERT_MSG_EQ(b, c, "b should equal c");
  NS_TEST_ASSERT_MSG_EQ(c, a, "c should equal a");
  NS_TEST_ASSERT_MSG_NE(a, x, "a should not equal x");
  NS_TEST_ASSERT_MSG_NE(a, y, "a should not equal y");
}
EndTest ()

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpRoutingProtocol
 */
static class TestSuiteNfpComputationCost : public TestSuite
{
public:
  TestSuiteNfpComputationCost () : TestSuite ("nfp-computation-cost", UNIT)
  {
    AddTestCase (new Constructor_NoArg (), TestCase::QUICK);
    AddTestCase (new Constructor_Copy (), TestCase::QUICK);
    AddTestCase (new IncrementEvents (), TestCase::QUICK);
    AddTestCase (new IncrementLoopIterations (), TestCase::QUICK);
    AddTestCase (new IncrementLoopIterationsCount (), TestCase::QUICK);
    AddTestCase (new OperatorPlus (), TestCase::QUICK);
    AddTestCase (new OperatorPlusEquals (), TestCase::QUICK);
    AddTestCase (new OperatorEqualEqual (), TestCase::QUICK);
  }
} g_TestSuiteNfpComputationCost;
}

