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
#include "ns3/ccnx-perhopheader.h"
#include "ns3/ccnx-interestlifetime.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxPerHopHeader {

BeginTest (Constructor)
{
  Ptr<CCNxPerHopHeader> perHopHeader = Create<CCNxPerHopHeader>();
  bool exists = (perHopHeader);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");
}
EndTest ()

BeginTest (AddHeader)
{
  Ptr<CCNxPerHopHeader> perHopHeader = Create<CCNxPerHopHeader>();
  Ptr<CCNxTime> time1 = Create<CCNxTime>(3600);
  Ptr<CCNxInterestLifetime> interestLifetime1 = Create<CCNxInterestLifetime> (time1);
  perHopHeader->AddHeader(interestLifetime1);
  bool truth1 = perHopHeader->size() == 1;
  NS_TEST_EXPECT_MSG_EQ (truth1, true, "The values should match");

  Ptr<CCNxTime> time2 = Create<CCNxTime>(1200);
  Ptr<CCNxInterestLifetime> interestLifetime2 = Create<CCNxInterestLifetime> (time2);
  perHopHeader->AddHeader(interestLifetime2);
  bool truth2 = perHopHeader->size() == 2;
  NS_TEST_EXPECT_MSG_EQ (truth2, true, "The values should match");

  std::cout << *perHopHeader << std::endl;
}
EndTest ()

BeginTest (RemoveHeader)
{
  Ptr<CCNxPerHopHeader> perHopHeader = Create<CCNxPerHopHeader>();
  Ptr<CCNxTime> time1 = Create<CCNxTime>(3600);
  Ptr<CCNxInterestLifetime> interestLifetime1 = Create<CCNxInterestLifetime> (time1);
  perHopHeader->AddHeader(interestLifetime1);

  Ptr<CCNxTime> time2 = Create<CCNxTime>(1200);
  Ptr<CCNxInterestLifetime> interestLifetime2 = Create<CCNxInterestLifetime> (time2);
  perHopHeader->AddHeader(interestLifetime2);

  perHopHeader->RemoveHeader(0);
  bool truth = perHopHeader->size() == 1;
  NS_TEST_EXPECT_MSG_EQ (truth, true, "The values should match");
}
EndTest ()

BeginTest (GetHeader)
{
  Ptr<CCNxPerHopHeader> perHopHeader = Create<CCNxPerHopHeader>();
  Ptr<CCNxTime> time1 = Create<CCNxTime>(3600);
  Ptr<CCNxInterestLifetime> interestLifetime1 = Create<CCNxInterestLifetime> (time1);
  perHopHeader->AddHeader(interestLifetime1);

  Ptr<CCNxTime> time2 = Create<CCNxTime>(1200);
  Ptr<CCNxInterestLifetime> interestLifetime2 = Create<CCNxInterestLifetime> (time2);
  perHopHeader->AddHeader(interestLifetime2);

  bool truth = perHopHeader->GetHeader(0) == interestLifetime1;
  NS_TEST_EXPECT_MSG_EQ (truth, true, "The values should match");
}
EndTest ()

BeginTest (Clear)
{
  Ptr<CCNxPerHopHeader> perHopHeader = Create<CCNxPerHopHeader>();
  Ptr<CCNxTime> time1 = Create<CCNxTime>(3600);
  Ptr<CCNxInterestLifetime> interestLifetime1 = Create<CCNxInterestLifetime> (time1);
  perHopHeader->AddHeader(interestLifetime1);

  Ptr<CCNxTime> time2 = Create<CCNxTime>(1200);
  Ptr<CCNxInterestLifetime> interestLifetime2 = Create<CCNxInterestLifetime> (time2);
  perHopHeader->AddHeader(interestLifetime2);

  perHopHeader->clear();
  bool truth = perHopHeader->size() == 0;
  NS_TEST_EXPECT_MSG_EQ (truth, true, "The values should match");
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxPerHopHeader
 */
static class TestSuiteCCNxPerHopHeader : public TestSuite
{
public:
  TestSuiteCCNxPerHopHeader () : TestSuite ("ccnx-perhopheader", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new AddHeader (), TestCase::QUICK);
    AddTestCase (new RemoveHeader (), TestCase::QUICK);
    AddTestCase (new GetHeader (), TestCase::QUICK);
    AddTestCase (new Clear (), TestCase::QUICK);
  }
} g_TestSuiteCCNxPerHop;

} // namespace TestSuiteCCNxPerHopHeader
