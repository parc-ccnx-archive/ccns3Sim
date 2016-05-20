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

#include "ns3/nfp-workqueue.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpWorkQueueTests {
BeginTest (TestConstructor)
{
  Ptr<NfpWorkQueue> queue = Create<NfpWorkQueue> ();
  // invoke the test operator ()
  bool success = (queue);
  NS_TEST_ASSERT_MSG_EQ (success, true, "Got null NfpWorkQueue");
}
EndTest ()

BeginTest (TestAddUnique)
{
  Ptr<NfpWorkQueue> queue = Create<NfpWorkQueue> ();

  struct Test
  {
    const char *anchor;
    const char *prefix;
  } testVectors[] = {
    {"ccnx:/name=aaa", "ccnx:/name=bbb"},
    {"ccnx:/name=aaa", "ccnx:/name=bar"},
    {"ccnx:/name=foo", "ccnx:/name=bar"},
    {"ccnx:/name=foo", "ccnx:/name=car"},
    {"ccnx:/name=goo", "ccnx:/name=bar"},
    {NULL, NULL},
  };

  int count = 0;
  for (int i = 0; testVectors[i].anchor != NULL; i++)
    {
      Ptr<CCNxName> a = Create<CCNxName> (testVectors[i].anchor);
      Ptr<CCNxName> p = Create<CCNxName> (testVectors[i].prefix);
      Ptr<NfpWorkQueueEntry> entry = Create<NfpWorkQueueEntry> (a, p);
      queue->push_back (entry);
      count++;
    }

  int offset = 0;
  while (!queue->empty ())
    {
      NS_ASSERT_MSG (testVectors[offset].anchor != NULL, "Went past end of test data");

      Ptr<CCNxName> a = Create<CCNxName> (testVectors[offset].anchor);
      Ptr<CCNxName> p = Create<CCNxName> (testVectors[offset].prefix);
      Ptr<NfpWorkQueueEntry> entry = queue->pop_front ();

      NS_TEST_EXPECT_MSG_EQ (a->Equals (*entry->GetAnchorName ()), true, "Wrong anchor name " << offset);
      NS_TEST_EXPECT_MSG_EQ (p->Equals (*entry->GetPrefix ()), true, "Wrong prefix name " << offset);
      offset++;
    }
  NS_TEST_EXPECT_MSG_EQ (offset, count, "Queue did not process all elements");
}
EndTest ()

BeginTest (TestAddDuplicate)
{
  Ptr<NfpWorkQueue> queue = Create<NfpWorkQueue> ();

  struct Test
  {
    const char *anchor;
    const char *prefix;
  } testVectors[] = {
    {"ccnx:/name=aaa", "ccnx:/name=bbb"},
    {"ccnx:/name=aaa", "ccnx:/name=bar"},
    {"ccnx:/name=foo", "ccnx:/name=car"},
    {"ccnx:/name=foo", "ccnx:/name=car"},         // duplicate entry
    {"ccnx:/name=goo", "ccnx:/name=bar"},
    {NULL, NULL},
  };

  for (int i = 0; testVectors[i].anchor != NULL; i++)
    {
      Ptr<CCNxName> a = Create<CCNxName> (testVectors[i].anchor);
      Ptr<CCNxName> p = Create<CCNxName> (testVectors[i].prefix);
      Ptr<NfpWorkQueueEntry> entry = Create<NfpWorkQueueEntry> (a, p);
      queue->push_back (entry);
    }

  int offset = 0;
  while (!queue->empty ())
    {
      NS_ASSERT_MSG (testVectors[offset].anchor != NULL, "Went past end of test data");

      Ptr<CCNxName> a = Create<CCNxName> (testVectors[offset].anchor);
      Ptr<CCNxName> p = Create<CCNxName> (testVectors[offset].prefix);
      Ptr<NfpWorkQueueEntry> entry = queue->pop_front ();
      offset++;
    }
  NS_TEST_EXPECT_MSG_EQ (offset, 4, "Should have only had 4 entries");
}
EndTest ()

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpWorkQueueEntry
 */
static class TestSuiteNfpWorkQueue : public TestSuite
{
public:
  TestSuiteNfpWorkQueue () : TestSuite ("nfp-workqueue", UNIT)
  {
    AddTestCase (new TestConstructor (), TestCase::QUICK);
    AddTestCase (new TestAddUnique (), TestCase::QUICK);
    AddTestCase (new TestAddDuplicate (), TestCase::QUICK);
  }
} g_TestSuiteNfpWorkQueue;
}

