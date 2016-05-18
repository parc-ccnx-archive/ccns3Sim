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
#include "ns3/ccnx-route-entry.h"
#include "../mockups/mockup_ccnx-virtual-connection.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxRouteEntry {

class TestVector
{
public:
  Ptr<const CCNxName> name;
  uint32_t cost;
  Ptr<CCNxVirtualConnection> connection;   //using CCNxVirtualConnection because it can initialize connid

  TestVector (CCNxConnection::ConnIdType connectionId = 55)
  {
    name = Create<const CCNxName> ("ccnx:/name=foo/name=bar");
    cost = 44;
    connection = Create<CCNxVirtualConnection> (connectionId);
  }
};

static Ptr<CCNxRouteEntry>
CreateRouteEntry (TestVector &input)
{
  Ptr<CCNxRouteEntry> entry = Create<CCNxRouteEntry> (input.name, input.cost, input.connection);
  return entry;
}

BeginTest (Constructor)
{
  TestVector t;
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  bool exists = (entry);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null CCNxRouteEntry");
}
EndTest ()

BeginTest (GetPrefix)
{
  TestVector t;
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  bool ok = entry->GetPrefix ()->Equals (*t.name);
  NS_TEST_EXPECT_MSG_EQ (ok, true, "Bad prefix");
}
EndTest ()

BeginTest (GetCost)
{
  TestVector t;
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  bool ok = entry->GetCost () == t.cost;
  NS_TEST_EXPECT_MSG_EQ (ok, true, "Bad cost");
}
EndTest ()

BeginTest (GetConnection)
{
  TestVector t;
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  bool ok = entry->GetConnection ()->GetConnectionId () == t.connection->GetConnectionId ();
  NS_TEST_EXPECT_MSG_EQ (ok, true, "Bad Connection ID");
}
EndTest ()

BeginTest (Equals)
{
  TestVector va, vb, vc, vx, vy, vz (0);
  vx.name = Create<const CCNxName> ("ccnx:/name=other");
  vy.cost = 0;
//  vz.connid = 0; now done in constructor argument

  Ptr<CCNxRouteEntry> a = CreateRouteEntry (va);
  Ptr<CCNxRouteEntry> b = CreateRouteEntry (vb);
  Ptr<CCNxRouteEntry> c = CreateRouteEntry (vc);
  Ptr<CCNxRouteEntry> x = CreateRouteEntry (vx);
  Ptr<CCNxRouteEntry> y = CreateRouteEntry (vy);
  Ptr<CCNxRouteEntry> z = CreateRouteEntry (vz);

  NS_TEST_EXPECT_MSG_EQ (a->Equals (*b), true, "A should equal B");
  NS_TEST_EXPECT_MSG_EQ (b->Equals (*c), true, "B should equal C");
  NS_TEST_EXPECT_MSG_EQ (c->Equals (*a), true, "C should equal A");

  NS_TEST_EXPECT_MSG_NE (a->Equals (*x), true, "A should not equal X");
  NS_TEST_EXPECT_MSG_NE (a->Equals (*y), true, "A should not equal Y");
  NS_TEST_EXPECT_MSG_NE (a->Equals (*z), true, "A should not equal Z");
}
EndTest ()

BeginTest (Operator_Output)
{
  std::string expected ("{ prefix: ccnx:/NAME=foo/NAME=bar, cost: 44, connId: 55 }");

  TestVector t;
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);

  std::stringstream ss;
  ss << *entry;
  std::string test = ss.str ();
  NS_TEST_EXPECT_MSG_EQ (expected, test, "Output operator does not match");
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxTime
 */
static class TestSuiteCCNxRouteEntry : public TestSuite
{
public:
  TestSuiteCCNxRouteEntry () : TestSuite ("ccnx-route-entry", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new GetPrefix (), TestCase::QUICK);

    AddTestCase (new GetCost (), TestCase::QUICK);
    AddTestCase (new GetConnection (), TestCase::QUICK);
    AddTestCase (new Equals (), TestCase::QUICK);
    AddTestCase (new Operator_Output (), TestCase::QUICK);
  }
} g_TestSuiteCCNxRouteEntry;

} // namespace TestSuiteCCNxRouteEntry
