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

#include "ns3/nfp-prefix-timer-entry.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpPrefixTimerEntryTests {
BeginTest (GetPrefix)
{
  const Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=prefix");
  const Ptr<const CCNxName> anchor = Create<CCNxName> ("ccnx:/name=anchor");
  Time expiry (Seconds (100));

  Ptr<NfpPrefixTimerEntry> entry = Create<NfpPrefixTimerEntry> (prefix, anchor, expiry);
  Ptr<const CCNxName> test = entry->GetPrefix ();
  NS_TEST_EXPECT_MSG_EQ (prefix->Equals (*test), true, "Prefix does not match");
}
EndTest ()

BeginTest (GetAnchorName)
{
  const Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=prefix");
  const Ptr<const CCNxName> anchor = Create<CCNxName> ("ccnx:/name=anchor");
  Time expiry (Seconds (100));

  Ptr<NfpPrefixTimerEntry> entry = Create<NfpPrefixTimerEntry> (prefix, anchor, expiry);
  Ptr<const CCNxName> test = entry->GetAnchorName ();
  NS_TEST_EXPECT_MSG_EQ (anchor->Equals (*test), true, "Anchor does not match");
}
EndTest ()

BeginTest (GetExpiry)
{
  const Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=prefix");
  const Ptr<const CCNxName> anchor = Create<CCNxName> ("ccnx:/name=anchor");
  Time expiry (Seconds (100));

  Ptr<NfpPrefixTimerEntry> entry = Create<NfpPrefixTimerEntry> (prefix, anchor, expiry);
  Time test = entry->GetTime ();
  NS_TEST_EXPECT_MSG_EQ (test, expiry, "Expiry time does not match");
}
EndTest ()



BeginTest (IsTimeLess)
{
  const Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=prefix");
  const Ptr<const CCNxName> anchor = Create<CCNxName> ("ccnx:/name=anchor");
  Time expiry (Seconds (100));

  Ptr<NfpPrefixTimerEntry> base = Create<NfpPrefixTimerEntry> (prefix, anchor, expiry);

  struct test_vector
  {
    const char *prefixUri;
    const char *anchorUri;
    Time expiry;
    bool result;
  } testVectors [] = {
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (99),  false },
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (100), false },
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (101), true },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (99),  false },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (100), false },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (101), true },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (99),  false },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (100), false },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (101), true },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (99),  false },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (100), false },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (101), true },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (99),  false },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (100), false },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (101), true },
    { NULL, NULL, Seconds (0), false },
  };

  for (int i = 0; testVectors[i].prefixUri != NULL; i++)
    {
      Ptr<NfpPrefixTimerEntry> entry = Create<NfpPrefixTimerEntry> (Create<CCNxName> (testVectors[i].prefixUri),
                                                                    Create<CCNxName> (testVectors[i].anchorUri), testVectors[i].expiry);
      bool test = NfpPrefixTimerEntry::IsTimeLess ()(base, entry);
      NS_TEST_EXPECT_MSG_EQ (test, testVectors[i].result, "Failed test vector " << i);
    }
}
EndTest ()

BeginTest (IsTimeGreater)
{
  const Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=prefix");
  const Ptr<const CCNxName> anchor = Create<CCNxName> ("ccnx:/name=anchor");
  Time expiry (Seconds (100));

  Ptr<NfpPrefixTimerEntry> base = Create<NfpPrefixTimerEntry> (prefix, anchor, expiry);

  struct test_vector
  {
    const char *prefixUri;
    const char *anchorUri;
    Time expiry;
    bool result;
  } testVectors [] = {
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (99),  true },
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (100), false },
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (101), false },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (99),  true },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (100), false },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (101), false },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (99),  true },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (100), false },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (101), false },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (99),  true },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (100), false },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (101), false },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (99),  true },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (100), false },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (101), false },
    { NULL, NULL, Seconds (0), false },
  };

  for (int i = 0; testVectors[i].prefixUri != NULL; i++)
    {
      Ptr<NfpPrefixTimerEntry> entry = Create<NfpPrefixTimerEntry> (Create<CCNxName> (testVectors[i].prefixUri),
                                                                    Create<CCNxName> (testVectors[i].anchorUri), testVectors[i].expiry);
      bool test = NfpPrefixTimerEntry::IsTimeGreater ()(base, entry);
      NS_TEST_EXPECT_MSG_EQ (test, testVectors[i].result, "Failed test vector " << i);
    }
}
EndTest ()

BeginTest (IsNameLess)
{
  const Ptr<const CCNxName> prefix = Create<CCNxName> ("ccnx:/name=prefix");
  const Ptr<const CCNxName> anchor = Create<CCNxName> ("ccnx:/name=anchor");
  Time expiry (Seconds (100));

  Ptr<NfpPrefixTimerEntry> base = Create<NfpPrefixTimerEntry> (prefix, anchor, expiry);

  struct test_vector
  {
    const char *prefixUri;
    const char *anchorUri;
    Time expiry;
    bool result;
  } testVectors [] = {
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (99),  false },
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (100), false },
    { "ccnx:/name=prefix", "ccnx:/name=anchor", Seconds (101), false },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (99),  false },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (100), false },
    { "ccnx:/name=aaaaaa", "ccnx:/name=anchor", Seconds (101), false },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (99),  false },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (100), false },
    { "ccnx:/name=prefix", "ccnx:/name=aaaaaa", Seconds (101), false },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (99),  true },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (100), true },
    { "ccnx:/name=zzzzzz", "ccnx:/name=anchor", Seconds (101), true },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (99),  true },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (100), true },
    { "ccnx:/name=prefix", "ccnx:/name=zzzzzz", Seconds (101), true },
    { NULL, NULL, Seconds (0), false },
  };

  for (int i = 0; testVectors[i].prefixUri != NULL; i++)
    {
      Ptr<NfpPrefixTimerEntry> entry = Create<NfpPrefixTimerEntry> (Create<CCNxName> (testVectors[i].prefixUri),
                                                                    Create<CCNxName> (testVectors[i].anchorUri), testVectors[i].expiry);
      bool test = NfpPrefixTimerEntry::IsNameLess ()(base, entry);
      NS_TEST_EXPECT_MSG_EQ (test, testVectors[i].result, "Failed test vector " << i);
    }
}
EndTest ()

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpRoutingProtocol
 */
static class TestSuiteNfpPrefixTimerEntry : public TestSuite
{
public:
  TestSuiteNfpPrefixTimerEntry () : TestSuite ("nfp-prefix-timer-entry", UNIT)
  {
    AddTestCase (new GetPrefix (), TestCase::QUICK);
    AddTestCase (new GetAnchorName (), TestCase::QUICK);
    AddTestCase (new GetExpiry (), TestCase::QUICK);
    AddTestCase (new IsTimeLess (), TestCase::QUICK);
    AddTestCase (new IsTimeGreater (), TestCase::QUICK);
    AddTestCase (new IsNameLess (), TestCase::QUICK);
  }
} g_TestSuiteNfpPrefixTimerEntry;
}

