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
#include "ns3/ccnx-message-portal.h"
#include "ns3/ccns3Sim-module.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxL4Protocol {

BeginTest (Constructor)
{
  Ptr<CCNxMessagePortal> mp = CreateObject<CCNxMessagePortal> ();

  bool exists = (mp);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");

  CCNxL4Protocol::CCNxL4Id id = mp->GetInstanceId ();

  NS_TEST_EXPECT_MSG_EQ (id, true, "Should be non zero value");
}
EndTest ()

static bool
testSendCallback (const CCNxL4Protocol &protocol, Ptr<CCNxPacket> ccnxPacket)
{
  return true;
}

BeginTest (GetSetLayer3SendCallback)
{
  Ptr<CCNxMessagePortal> mp = CreateObject<CCNxMessagePortal> ();

  CCNxL4Protocol::Layer3SendCallback callback = MakeCallback (&testSendCallback);
  mp->SetLayer3SendCallback (callback);

  CCNxL4Protocol::Layer3SendCallback callback1 = mp->GetLayer3SendCallback ();

  bool truth = callback.IsEqual (callback1);
  NS_TEST_EXPECT_MSG_EQ (truth, true, "Callbacks should be the same");
}
EndTest ()

static bool
testSendToCallback (const CCNxL4Protocol &protocol, Ptr<CCNxPacket> ccnxPacket, uint32_t connId)
{
  return true;
}

BeginTest (GetSetLayer3SendToCallback)
{
  Ptr<CCNxMessagePortal> mp = CreateObject<CCNxMessagePortal> ();

  CCNxL4Protocol::Layer3SendToCallback callback = MakeCallback (&testSendToCallback);
  mp->SetLayer3SendToCallback (callback);

  CCNxL4Protocol::Layer3SendToCallback callback1 = mp->GetLayer3SendToCallback ();

  bool truth = callback.IsEqual (callback1);
  NS_TEST_EXPECT_MSG_EQ (truth, true, "Callbacks should be the same");
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxTime
 */
static class TestSuiteCCNxL4Protocol : public TestSuite
{
public:
  TestSuiteCCNxL4Protocol () : TestSuite ("ccnx-l4-protocol", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new GetSetLayer3SendCallback (), TestCase::QUICK);
    AddTestCase (new GetSetLayer3SendToCallback (), TestCase::QUICK);
  }
} g_TestSuiteCCNxL4Protocol;

} // namespace TestSuiteCCNxL4Protocol
