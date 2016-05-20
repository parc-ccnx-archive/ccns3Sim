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

#include "ns3/nfp-prefix-timer-heap.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpPrefixTimerHeapTests {
BeginTest (NoDuplicates)
{
  // Insert entries without any duplicates

  // We insert the entries in their natural order in the test vector then start
  // poping off the queue.  We expect the queue order to match the 'order' struct member (0-base).
  struct test_vector
  {
    const char *prefixUri;
    const char *anchorUri;
    Time expiry;
    int order;
    bool matched;
  } testVectors [] = {
    { "ccnx:/name=aaa", "ccnx:/name=anchor", Seconds (1700),  7, false },
    { "ccnx:/name=bbb", "ccnx:/name=anchor", Seconds (1500),  5, false },
    { "ccnx:/name=ccc", "ccnx:/name=anchor", Seconds (1600),  6, false },
    { "ccnx:/name=ddd", "ccnx:/name=anchor", Seconds (1400),  4, false },
    { "ccnx:/name=eee", "ccnx:/name=anchor", Seconds (1300),  3, false },
    { "ccnx:/name=fff", "ccnx:/name=anchor", Seconds (1100),  1, false },
    { "ccnx:/name=ggg", "ccnx:/name=anchor", Seconds (1000),  0, false },
    { "ccnx:/name=hhh", "ccnx:/name=anchor", Seconds (1200),  2, false },
    { NULL, NULL, Seconds (0), false },
  };

  NfpPrefixTimerHeap heap;
  for (int i = 0; testVectors[i].prefixUri != NULL; i++)
    {
      heap.Insert (Create<CCNxName> (testVectors[i].prefixUri), Create<CCNxName> (testVectors[i].anchorUri), testVectors[i].expiry);
    }

  // Now pop off the top of the heap and make sure that we retrieve the element
  // in the right order, as defined in test_vector.order.
  Ptr<const NfpPrefixTimerEntry> entry;
  int order = 0;
  while ( (entry = heap.Peek ()) )
    {
      bool found = false;
      for (int i = 0; testVectors[i].prefixUri != NULL; i++)
        {
          if (testVectors[i].order == order)
            {
              NS_TEST_EXPECT_MSG_EQ (testVectors[i].matched, false, "We already matched this entry");
              testVectors[i].matched = true;
              NS_TEST_EXPECT_MSG_EQ (Create<CCNxName> (testVectors[i].prefixUri)->Equals (*entry->GetPrefix ()), true, "Prefix wrong order " << order);
              NS_TEST_EXPECT_MSG_EQ (Create<CCNxName> (testVectors[i].anchorUri)->Equals (*entry->GetAnchorName ()), true, "AnchorName wrong order " << order);
              NS_TEST_EXPECT_MSG_EQ (testVectors[i].expiry, entry->GetTime (), "Time wrong order " << order);
              found = true;
              break;
            }
        }
      NS_TEST_EXPECT_MSG_EQ (found, true, "Did not find entry for order " << order);

      heap.Pop ();
      order++;
    }

  // Make sure we matched all of them
  for (int i = 0; testVectors[i].prefixUri != NULL; i++)
    {
      NS_TEST_EXPECT_MSG_EQ (testVectors[i].matched, true, "test vector not matched index " << i);
    }

}
EndTest ()

BeginTest (Duplicates)
{
  // Insert entries without any duplicates

  // We insert the entries in their natural order in the test vector then start
  // poping off the queue.  We expect the queue order to match the 'order' struct member (0-base).
  // We use an order of "-1" for duplicated entries that should be marked as invalid.
  struct test_vector
  {
    const char *prefixUri;
    const char *anchorUri;
    Time expiry;
    int order;
    bool matched;
  } testVectors [] = {
    { "ccnx:/name=aaa", "ccnx:/name=anchor", Seconds (1700), -1, false },
    { "ccnx:/name=bbb", "ccnx:/name=anchor", Seconds (1500), -1, false },
    { "ccnx:/name=ccc", "ccnx:/name=anchor", Seconds (1600),  4, false },
    { "ccnx:/name=aaa", "ccnx:/name=anchor", Seconds (2000),  6, false },
    { "ccnx:/name=ddd", "ccnx:/name=anchor", Seconds (1400),  3, false },
    { "ccnx:/name=eee", "ccnx:/name=anchor", Seconds (1300),  2, false },
    { "ccnx:/name=fff", "ccnx:/name=anchor", Seconds (1100), -1, false },
    { "ccnx:/name=bbb", "ccnx:/name=anchor", Seconds (1900),  5, false },
    { "ccnx:/name=ggg", "ccnx:/name=anchor", Seconds (1000),  0, false },
    { "ccnx:/name=hhh", "ccnx:/name=anchor", Seconds (1200),  1, false },
    { "ccnx:/name=fff", "ccnx:/name=anchor", Seconds (2200),  7, false },
    { NULL, NULL, Seconds (0), false },
  };

  NfpPrefixTimerHeap heap;
  for (int i = 0; testVectors[i].prefixUri != NULL; i++)
    {
      heap.Insert (Create<CCNxName> (testVectors[i].prefixUri), Create<CCNxName> (testVectors[i].anchorUri), testVectors[i].expiry);
    }

  // Now pop off the top of the heap and make sure that we retrieve the element
  // in the right order, as defined in test_vector.order.
  Ptr<const NfpPrefixTimerEntry> entry;
  int order = 0;
  while ( (entry = heap.Peek ()) )
    {
      bool found = false;
      for (int i = 0; testVectors[i].prefixUri != NULL; i++)
        {
          if (testVectors[i].order == order)
            {
              NS_TEST_EXPECT_MSG_EQ (testVectors[i].matched, false, "We already matched this entry");
              testVectors[i].matched = true;
              NS_TEST_EXPECT_MSG_EQ (Create<CCNxName> (testVectors[i].prefixUri)->Equals (*entry->GetPrefix ()), true, "Prefix wrong order " << order);
              NS_TEST_EXPECT_MSG_EQ (Create<CCNxName> (testVectors[i].anchorUri)->Equals (*entry->GetAnchorName ()), true, "AnchorName wrong order " << order);
              NS_TEST_EXPECT_MSG_EQ (testVectors[i].expiry, entry->GetTime (), "Time wrong order " << order);
              found = true;
              break;
            }
        }
      NS_TEST_EXPECT_MSG_EQ (found, true, "Did not find entry for order " << order);

      heap.Pop ();
      order++;
    }

  // Make sure we matched all of them
  for (int i = 0; testVectors[i].prefixUri != NULL; i++)
    {
      if (testVectors[i].order >= 0)
        {
          NS_TEST_EXPECT_MSG_EQ (testVectors[i].matched, true, "test vector not matched index " << i);
        }
    }

}
EndTest ()
/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpRoutingProtocol
 */
static class TestSuiteNfpPrefixTimerHeap : public TestSuite
{
public:
  TestSuiteNfpPrefixTimerHeap () : TestSuite ("nfp-prefix-timer-heap", UNIT)
  {
    AddTestCase (new NoDuplicates (), TestCase::QUICK);
    AddTestCase (new Duplicates (), TestCase::QUICK);
  }
} g_TestSuiteNfpPrefixTimerHeap;
}

