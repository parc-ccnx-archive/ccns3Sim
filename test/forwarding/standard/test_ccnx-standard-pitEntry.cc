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
#include "ns3/ccnx-standard-pitEntry.h"
#include "../../mockups/mockup_ccnx-virtual-connection.h"
#include "ns3/log.h"
#include "ns3/assert.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/ccns3Sim-module.h"

#include "../../TestMacros.h"
#include "../../mockups/mockup_ccnx-virtual-connection.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxStandardPitEntry {

  typedef struct _testDataRecord {
    Ptr<CCNxInterest> interest;
    Ptr<CCNxConnection> ingress;
    Time expiryTime;
  } TestDataRecord;

  typedef struct _testData {
    Ptr<CCNxConnection> ingress1;
    Ptr<CCNxConnection> ingress2;
    Ptr<CCNxConnection> ingress3;

    TestDataRecord interest_Ingress1_Expiry1;
    TestDataRecord interest_Ingress2_Expiry2;
    TestDataRecord interest_Ingress1_Expiry3;
    TestDataRecord interest_Ingress3_Expiry4;
  } TestData;

  static TestData
  CreateTestData()
  {
    TestData data;
    data.ingress1 = Create<CCNxVirtualConnection>(1);
    data.ingress2 = Create<CCNxVirtualConnection>(2);
    data.ingress3 = Create<CCNxVirtualConnection>(3);

    data.interest_Ingress1_Expiry1.interest = CreateObject<CCNxInterest>(Create<CCNxName>("ccnx:/name=foo"));
    data.interest_Ingress1_Expiry1.ingress = data.ingress1;
    data.interest_Ingress1_Expiry1.expiryTime = Seconds(1);

    data.interest_Ingress2_Expiry2.interest = CreateObject<CCNxInterest>(Create<CCNxName>("ccnx:/name=foo"));
    data.interest_Ingress2_Expiry2.ingress = data.ingress2;
    data.interest_Ingress2_Expiry2.expiryTime = Seconds(2);

    data.interest_Ingress1_Expiry3.interest = CreateObject<CCNxInterest>(Create<CCNxName>("ccnx:/name=foo"));
    data.interest_Ingress1_Expiry3.ingress = data.ingress1;
    data.interest_Ingress1_Expiry3.expiryTime = Seconds(3);

    data.interest_Ingress3_Expiry4.interest = CreateObject<CCNxInterest>(Create<CCNxName>("ccnx:/name=foo"));
    data.interest_Ingress3_Expiry4.ingress = data.ingress3;
    data.interest_Ingress3_Expiry4.expiryTime = Seconds(4);

    return data;
  }


BeginTest (Constructor)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  bool exists = (entry);
  NS_TEST_EXPECT_MSG_EQ(exists, true, "Failed to create a CCNxStandardPitEntry");
}
EndTest ()

/*
 * Not empty reverse route set, Pit Entry not expired
 * Should return reverse route set size
 */
BeginTest (Size_NotEmptyNotExpired)
{

}
EndTest ()

/*
 * Not empty reverse route set, Pit Entry expired
 * Should return 0
 */
BeginTest (Size_NotEmptyExpired)
{

}
EndTest ()

/*
 * Pit entry is not expired
 */
BeginTest (IsExpired_True)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
			 data.interest_Ingress1_Expiry1.ingress,
			 data.interest_Ingress1_Expiry1.expiryTime);

  bool expired = entry->isExpired();
  NS_TEST_EXPECT_MSG_EQ(expired, false, "PIT entry should not be expired");
}
EndTest ()

/*
 * Pit entry is expired.
 * This is the default state after creating a PIT entry
 */
BeginTest (IsExpired_False)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  bool expired = entry->isExpired();
  NS_TEST_EXPECT_MSG_EQ(expired, true, "PIT entry should be expired on creation");
}
EndTest ()

/*
 * The first interest should always be forwarded and the set size will be 1
 */
BeginTest (ReceiveInterest_FirstInterest)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  CCNxPit::Verdict verdict = entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
						    data.interest_Ingress1_Expiry1.ingress,
						    data.interest_Ingress1_Expiry1.expiryTime);

  NS_TEST_EXPECT_MSG_EQ(verdict, CCNxPit::Forward, "1st pit reverse route should be forwarded");
}
EndTest ()

/*
 * Second interest from different connection should be aggregated
 */
BeginTest (ReceiveInterest_SecondDifferentConnection)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
			  data.interest_Ingress1_Expiry1.ingress,
			  data.interest_Ingress1_Expiry1.expiryTime);

  CCNxPit::Verdict verdict = entry->ReceiveInterest(data.interest_Ingress2_Expiry2.interest,
						    data.interest_Ingress2_Expiry2.ingress,
						    data.interest_Ingress2_Expiry2.expiryTime);

  NS_TEST_EXPECT_MSG_EQ(entry->size(), 2, "PIT should be 2 entries");
  NS_TEST_EXPECT_MSG_EQ(verdict, CCNxPit::Aggregate, "2st pit reverse route on different connection should be aggregated");
}
EndTest ()

/*
 * Second interest from same connection should be forwarded
 */
BeginTest (ReceiveInterest_SecondSameConnection)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
			  data.interest_Ingress1_Expiry1.ingress,
			  data.interest_Ingress1_Expiry1.expiryTime);

  CCNxPit::Verdict verdict = entry->ReceiveInterest(data.interest_Ingress1_Expiry3.interest,
						    data.interest_Ingress1_Expiry3.ingress,
						    data.interest_Ingress1_Expiry3.expiryTime);

  NS_TEST_EXPECT_MSG_EQ(entry->size(), 1, "PIT should be 1 entry");
  NS_TEST_EXPECT_MSG_EQ(verdict, CCNxPit::Forward, "2st pit reverse route on same connection should be forwarded");
}
EndTest ()

/*
 * An expired PIT entry should return an empty set
 */
BeginTest (SatisfyInterest_Expired)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  // put something in there, but have it be expired
  entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
			  data.interest_Ingress1_Expiry1.ingress,
			  Seconds(-2));

  CCNxStandardPitEntry::ReverseRouteType reverse = entry->SatisfyInterest(data.ingress3);
  NS_TEST_EXPECT_MSG_EQ( reverse.size(), 0, "Expired PIT entry should return 0 results.");
}
EndTest ()

/*
 * Return two PIT connections, neither of which is the ingress
 */
BeginTest (SatisfyInterest_TwoPaths)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  // Put in two entries with different reverse paths
  entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
			  data.interest_Ingress1_Expiry1.ingress,
			  data.interest_Ingress1_Expiry1.expiryTime);

  entry->ReceiveInterest(data.interest_Ingress2_Expiry2.interest,
			  data.interest_Ingress2_Expiry2.ingress,
			  data.interest_Ingress2_Expiry2.expiryTime);

  CCNxStandardPitEntry::ReverseRouteType reverse = entry->SatisfyInterest(data.ingress3);
  NS_TEST_EXPECT_MSG_EQ( reverse.size(), 2, "Two valid reverse paths, not same as ingress");
  NS_TEST_EXPECT_MSG_EQ( entry->size(), 0, "PIT entry should be empty now");
}
EndTest ()

/*
 * PIT has 2 paths, but one matches the ingress.  The ingress
 * should be left in the PIT entry.
 */
BeginTest (SatisfyInterest_OnePathOneIngress)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  // Put in two entries with different reverse paths
  entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
			  data.interest_Ingress1_Expiry1.ingress,
			  data.interest_Ingress1_Expiry1.expiryTime);

  entry->ReceiveInterest(data.interest_Ingress2_Expiry2.interest,
			  data.interest_Ingress2_Expiry2.ingress,
			  data.interest_Ingress2_Expiry2.expiryTime);

  // Content Object is from connection 1, so we should only get connection 2 in the reverse path
  // and connection 1 should still be in the PIT
  CCNxStandardPitEntry::ReverseRouteType reverse = entry->SatisfyInterest(data.ingress1);
  NS_TEST_EXPECT_MSG_EQ( reverse.size(), 1, "Should only have gotten 1 path back");

  Ptr<CCNxConnection> conn = *reverse.begin();
  unsigned connid = conn->GetConnectionId();
  NS_TEST_EXPECT_MSG_EQ( connid, 2, "Connection ID in reverse path should be 2");

  NS_TEST_EXPECT_MSG_EQ( entry->size(), 0, "PIT entry should be empty now");
}
EndTest ()

BeginTest (PrintPitEntry_Expired)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  // put something in there, but have it be expired.  It should not show up in the output
  entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
			  data.interest_Ingress1_Expiry1.ingress,
			  Seconds(-2));

  // Print to a string output stream
  std::stringstream ss;
  entry->PrintPitEntry(ss);
  std::string test = ss.str ();

  std::string expected("PitEntry { ExpiryTime : -1000000000.0ns EXPIRED ReverseRouteSize : 0 }\n");
  NS_TEST_EXPECT_MSG_EQ (expected, test, "PitEntry stream output wrong");
}
EndTest ()

BeginTest (PrintPitEntry_NotExpired)
{

  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  TestData data = CreateTestData();

  entry->ReceiveInterest(data.interest_Ingress1_Expiry1.interest,
			  data.interest_Ingress1_Expiry1.ingress,
			  data.interest_Ingress1_Expiry1.expiryTime);

  entry->ReceiveInterest(data.interest_Ingress2_Expiry2.interest,
			  data.interest_Ingress2_Expiry2.ingress,
			  data.interest_Ingress2_Expiry2.expiryTime);

  // Print to a string output stream
  std::stringstream ss;
  entry->PrintPitEntry(ss);
  std::string test = ss.str ();

  std::cout << test << std::endl;

  std::string expected1("PitEntry { ExpiryTime : +2000000000.0ns ReverseRouteSize : 2 ReverseRoutes: 1, 2,  }\n");
  std::string expected2("PitEntry { ExpiryTime : +2000000000.0ns ReverseRouteSize : 2 ReverseRoutes: 2, 1,  }\n");
  if (test != expected1 and test != expected2)
    {
      NS_TEST_EXPECT_MSG_EQ (false, true, "PitEntry stream output wrong");
      std::cout<<"PrintPitEntry output - actual  ="<<test<<std::endl;
      std::cout<<"PrintPitEntry output - expected="<<expected1<<std::endl;
    }
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * @brief Test Suite for CCNxStandardPitEntry
 */
static class TestSuiteCCNxStandardPitEntry : public TestSuite
{
public:
  TestSuiteCCNxStandardPitEntry () : TestSuite ("ccnx-standard-pitEntry", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new Size_NotEmptyNotExpired (), TestCase::QUICK);
    AddTestCase (new Size_NotEmptyExpired (), TestCase::QUICK);
    AddTestCase (new IsExpired_True (), TestCase::QUICK);
    AddTestCase (new IsExpired_False (), TestCase::QUICK);
    AddTestCase (new ReceiveInterest_FirstInterest (), TestCase::QUICK);
    AddTestCase (new ReceiveInterest_SecondDifferentConnection (), TestCase::QUICK);
    AddTestCase (new ReceiveInterest_SecondSameConnection (), TestCase::QUICK);
    AddTestCase (new SatisfyInterest_Expired (), TestCase::QUICK);
    AddTestCase (new SatisfyInterest_TwoPaths (), TestCase::QUICK);
    AddTestCase (new SatisfyInterest_OnePathOneIngress (), TestCase::QUICK);

    AddTestCase (new PrintPitEntry_Expired (), TestCase::QUICK);
    AddTestCase (new PrintPitEntry_NotExpired (), TestCase::QUICK);
  }
} g_TestSuiteCCNxStandardPitEntry;


} // namespace TestSuiteCCNxStandardPitEntry
