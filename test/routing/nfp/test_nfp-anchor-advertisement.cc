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

#include "ns3/nfp-anchor-advertisement.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-codec-name.h"
#include "ns3/ccnx-tlv.h"
#include "../../mockups/mockup_ccnx-virtual-connection.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpAnchorAdvertisementTests {

class TestState
{
public:
  Ptr<CCNxName> anchorName;
  Ptr<CCNxName> prefix;
  uint32_t anchorSeqnum;
  uint16_t distance;

  Ptr<NfpAdvertise> advertise;

  TestState ()
  {
    anchorName = ns3::Create<CCNxName> ("ccnx:/name=anchor_name");
    prefix = ns3::Create<CCNxName> ("ccnx:/name=parc.com/name=csl");
    anchorSeqnum = 0x12345678;
    distance = 0xabcd;
    advertise = Create<NfpAdvertise> (anchorName, prefix, anchorSeqnum, distance);
  }

  Ptr<NfpAnchorAdvertisement> CreateAnchorAdvertisement (uint32_t connId) const
  {
    Ptr<CCNxVirtualConnection> conn = Create<CCNxVirtualConnection> (connId);
    return ns3::Create<NfpAnchorAdvertisement> (advertise, conn, Time (100));
  }
};


class TestVector
{
public:
  Ptr<const CCNxName> name;
  uint32_t cost;
  Ptr<CCNxVirtualConnection> connection;   //using CCNxVirtualConnection because it can initialize connid

  TestVector (Ptr<CCNxName> iname, uint32_t icost = 44, CCNxConnection::ConnIdType connectionId = 55) : name (iname), cost (icost)
  {
    connection = Create<CCNxVirtualConnection> (connectionId);
  }
};


static Ptr<CCNxRouteEntry>
CreateRouteEntry (TestVector &input)
{
  Ptr<CCNxRouteEntry> entry = Create<CCNxRouteEntry> (input.name, input.cost, input.connection);
  return entry;
}

BeginTest (TestConstructor)
{
  TestState state;
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (5);
  // invoke the test operator ()
  bool success = (aa);
  NS_TEST_ASSERT_MSG_EQ (success, true, "Got null NfpAnchorAdvertisement");
}
EndTest ()

BeginTest (TestIsFeasibleAdvertisement_WrongAnchorName)
{
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (wrongname, state.prefix, state.anchorSeqnum, state.distance);
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (1);
  bool result = aa->IsFeasibleAdvertisement (advertise);
  NS_TEST_ASSERT_MSG_EQ (result, false, "Wrong anchor name should have been false match");
}
EndTest ()

BeginTest (TestIsFeasibleAdvertisement_WrongPrefix)
{
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (state.anchorName, wrongname, state.anchorSeqnum, state.distance);
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (1);
  bool result = aa->IsFeasibleAdvertisement (advertise);
  NS_TEST_ASSERT_MSG_EQ (result, false, "Wrong prefix should have been false match");
}
EndTest ()

BeginTest (TestIsFeasibleAdvertisement_SeqNumLess)
{
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (state.anchorName, state.prefix, state.anchorSeqnum - 1, state.distance);
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (1);
  bool result = aa->IsFeasibleAdvertisement (advertise);
  NS_TEST_ASSERT_MSG_EQ (result, false, "Advertisement with lower seqnum should fail");
}
EndTest ()

BeginTest (TestIsFeasibleAdvertisement_SeqNumEqualDistLess)
{
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (state.anchorName, state.prefix, state.anchorSeqnum, state.distance - 1);
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (1);
  bool result = aa->IsFeasibleAdvertisement (advertise);
  NS_TEST_ASSERT_MSG_EQ (result, true, "Advertisement with same seqnum and lower distance should succeed");
}
EndTest ()

BeginTest (TestIsFeasibleAdvertisement_SeqNumEqualDistEqual)
{
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (state.anchorName, state.prefix, state.anchorSeqnum, state.distance);
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (1);
  bool result = aa->IsFeasibleAdvertisement (advertise);
  NS_TEST_ASSERT_MSG_EQ (result, true, "Advertisement with same seqnum and same distance should succeed");
}
EndTest ()

BeginTest (TestIsFeasibleAdvertisement_SeqNumEqualDistGreater)
{
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (state.anchorName, state.prefix, state.anchorSeqnum, state.distance + 1);
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (1);
  bool result = aa->IsFeasibleAdvertisement (advertise);
  NS_TEST_ASSERT_MSG_EQ (result, false, "Advertisement with same seqnum and greater distance should fail");
}
EndTest ()

BeginTest (TestIsFeasibleAdvertisement_SeqNumGreater)
{
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (state.anchorName, state.prefix, state.anchorSeqnum + 1, state.distance);
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (1);
  bool result = aa->IsFeasibleAdvertisement (advertise);
  NS_TEST_ASSERT_MSG_EQ (result, true, "Advertisement with greater seqnum should succeed");
}
EndTest ()

BeginTest (TestUpdateAdvertisement_SeqNumGreater)
{
  // We should replace our advertisement and nexthops with the new advertisement
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (state.anchorName, state.prefix, state.anchorSeqnum + 1, state.distance);
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (1);
  Ptr<CCNxVirtualConnection> newConn = Create<CCNxVirtualConnection> (99);

  aa->UpdateAdvertisement (advertise, newConn, Time (100));

  NS_TEST_ASSERT_MSG_EQ (advertise->Equals (*aa->GetBestAdvertisement ()), true, "Advertisement did not get updated");
  NS_TEST_ASSERT_MSG_EQ (aa->GetRouteNoLinkCosts ()->size (), 1, "Egress list should be 1 element long");
  NS_TEST_ASSERT_MSG_EQ (aa->GetRouteNoLinkCosts ()->GetNexthops ()[0]->GetConnection ()->GetConnectionId (), newConn->GetConnectionId (), "Egress list element 0 should be newConnId");
}
EndTest ()

BeginTest (TestUpdateAdvertisement_SeqNumEqualDistEqual)
{
  // We should replace our advertisement and nexthops with the new advertisement
  TestState state;
  Ptr<CCNxName> wrongname = Create<CCNxName> ("ccnx:/name=martian");
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> (state.anchorName, state.prefix, state.anchorSeqnum, state.distance);
  uint32_t firstConnId = 1;
  Ptr<NfpAnchorAdvertisement> aa = state.CreateAnchorAdvertisement (firstConnId);
  uint32_t newConnId = 99;
  Ptr<CCNxVirtualConnection> newConn = Create<CCNxVirtualConnection> (newConnId);

  aa->UpdateAdvertisement (advertise, newConn, Time (100));

  NS_TEST_ASSERT_MSG_EQ (advertise->Equals (*aa->GetBestAdvertisement ()), true, "Advertisement did not get updated");
  NS_TEST_ASSERT_MSG_EQ (aa->GetRouteNoLinkCosts ()->size (), 2, "Egress list should be 2 elements long");

  Ptr<CCNxRoute> truth = Create<CCNxRoute> (state.prefix);
  TestVector t (state.prefix, state.distance,firstConnId);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  truth->AddNexthop (entry);
  TestVector t1 (state.prefix, state.distance,newConnId);
  Ptr<CCNxRouteEntry> entry1 = CreateRouteEntry (t1);
  truth->AddNexthop (entry1);

  Ptr<const CCNxRoute> test = aa->GetRouteNoLinkCosts ();

  bool ok = truth->Equals (*test);
  NS_TEST_ASSERT_MSG_EQ (ok, true, "Egress set " << *test << " does not match expected " << *truth);
}
EndTest ()

BeginTest (TestUpdateAdvertisement_RemoveNexthop)
{
}
EndTest ()

BeginTest (TestUpdateAdvertisement_GetNexthopCount_AllValid)
{
}
EndTest ()

BeginTest (TestUpdateAdvertisement_GetNexthopCount_WithExpiry)
{
  // Setup several next hops and have one expire.  Make sure call to GetNexthopCount removes it.
}
EndTest ()

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpRoutingProtocol
 */
static class TestSuiteNfpAnchorAdvertisement : public TestSuite
{
public:
  TestSuiteNfpAnchorAdvertisement () : TestSuite ("nfp-anchor-advertisement", UNIT)
  {
    AddTestCase (new TestConstructor (), TestCase::QUICK);
    AddTestCase (new TestIsFeasibleAdvertisement_WrongAnchorName (), TestCase::QUICK);
    AddTestCase (new TestIsFeasibleAdvertisement_WrongPrefix (), TestCase::QUICK);
    AddTestCase (new TestIsFeasibleAdvertisement_SeqNumLess (), TestCase::QUICK);
    AddTestCase (new TestIsFeasibleAdvertisement_SeqNumEqualDistLess (), TestCase::QUICK);
    AddTestCase (new TestIsFeasibleAdvertisement_SeqNumEqualDistEqual (), TestCase::QUICK);
    AddTestCase (new TestIsFeasibleAdvertisement_SeqNumEqualDistGreater (), TestCase::QUICK);
    AddTestCase (new TestIsFeasibleAdvertisement_SeqNumGreater (), TestCase::QUICK);

    AddTestCase (new TestUpdateAdvertisement_SeqNumGreater (), TestCase::QUICK);
    AddTestCase (new TestUpdateAdvertisement_SeqNumEqualDistEqual (), TestCase::QUICK);
  }
} g_TestSuiteNfpAnchorAdvertisement;
}

