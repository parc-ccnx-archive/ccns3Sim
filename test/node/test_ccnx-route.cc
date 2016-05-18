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
#include "ns3/ccnx-route.h"
#include "../mockups/mockup_ccnx-virtual-connection.h"
#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxRoute {

class TestVector
{
public:
  Ptr<const CCNxName> name;
  uint32_t cost;
  Ptr<CCNxVirtualConnection> connection;   //using CCNxVirtualConnection because it can initialize connid

  TestVector (Ptr<const CCNxName> iname, uint32_t icost = 44, CCNxConnection::ConnIdType connectionId = 55) : name (iname), cost (icost)
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


BeginTest (Constructor)
{
  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix);
  bool exists = (route);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null route from constructor");
}
EndTest ()

BeginTest (Constructor_Copy)
{

  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix);
  TestVector t (prefix,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  Ptr<CCNxRoute> copy = Create<CCNxRoute> (*route);
  bool equals = route->Equals (*copy);
  NS_TEST_EXPECT_MSG_EQ (equals, true, "Copy does not equal original");
}
EndTest ()

BeginTest (AddNexthop_Unique)
{
  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix);
  TestVector t (prefix,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);
  NS_TEST_EXPECT_MSG_EQ (route->size (), 1, "Size should be 1 after adding a next hop");
}
EndTest ()

BeginTest (AddNexthop_Duplicate_SameDistance)
{
  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix);
  TestVector t (prefix,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);
  route->AddNexthop (entry);
  NS_TEST_EXPECT_MSG_EQ (route->size (), 1, "Size should be 1 after adding same next hop twice");
}
EndTest ()

BeginTest (AddNexthop_Duplicate_LessDistance)
{
  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix);
  TestVector t1 (prefix,10,100);
  Ptr<CCNxRouteEntry> entry1 = CreateRouteEntry (t1);
  route->AddNexthop (entry1);

  // Entry with lower distance should prevail
  TestVector t2 (prefix,1,100);
  Ptr<CCNxRouteEntry> entry2 = CreateRouteEntry (t2);
  route->AddNexthop (entry2);

  Ptr<const CCNxRouteEntry> test = *(route->begin ());
  NS_TEST_EXPECT_MSG_EQ (test->GetCost (), 1, "Cost should be 1, not 10");
}
EndTest ()

BeginTest (AddRoute)
{
  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> first = Create<CCNxRoute> (prefix);
  TestVector t (prefix,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  first->AddNexthop (entry);

  Ptr<CCNxRoute> second = Create<CCNxRoute> (prefix);
  second->AddRoute (first);
  NS_TEST_EXPECT_MSG_EQ (second->Equals (*first), true, "second should equal first");
}
EndTest ()

BeginTest (RemoveNexthop_Missing)
{
  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix);
  TestVector t (prefix,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);
  // This is a different connection id, so should not affect anything
  Ptr<CCNxVirtualConnection> newConnection = Create<CCNxVirtualConnection> (200);
  route->RemoveNexthop (newConnection);
  NS_TEST_EXPECT_MSG_EQ (route->empty (), false, "The route should not be empty");
}
EndTest ()

BeginTest (RemoveNexthop_Exists)
{
  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix);
  TestVector t (prefix,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  Ptr<CCNxVirtualConnection> newConnection = Create<CCNxVirtualConnection> (100);
  route->RemoveNexthop (newConnection);
  NS_TEST_EXPECT_MSG_EQ (route->empty (), true, "The route should be empty");
}
EndTest ()

BeginTest (Equals)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  TestVector t1a (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry1a = CreateRouteEntry (t1a);
  TestVector t1b (prefix1,1,100);
  Ptr<CCNxRouteEntry> entry1b = CreateRouteEntry (t1b);
  TestVector t1c (prefix1,10,200);
  Ptr<CCNxRouteEntry> entry1c = CreateRouteEntry (t1c);


  Ptr<const CCNxName> prefix2 = Create<const CCNxName> ("ccnx:/name=bar");
  TestVector t (prefix2,10,100);
  Ptr<CCNxRouteEntry> entry2a = CreateRouteEntry (t);

  Ptr<CCNxRoute> a = Create<CCNxRoute> (prefix1);
  a->AddNexthop (entry1a);

  Ptr<CCNxRoute> b = Create<CCNxRoute> (prefix1);
  b->AddNexthop (entry1a);

  Ptr<CCNxRoute> c = Create<CCNxRoute> (prefix1);
  c->AddNexthop (entry1a);

  // not equal

  Ptr<CCNxRoute> x = Create<CCNxRoute> (prefix2);
  x->AddNexthop (entry2a);

  Ptr<CCNxRoute> y = Create<CCNxRoute> (prefix1);
  y->AddNexthop (entry1b);

  Ptr<CCNxRoute> z = Create<CCNxRoute> (prefix1);
  z->AddNexthop (entry1c);

  NS_TEST_EXPECT_MSG_EQ (a->Equals (*b), true, "A should equal B");
  NS_TEST_EXPECT_MSG_EQ (b->Equals (*c), true, "B should equal C");
  NS_TEST_EXPECT_MSG_EQ (c->Equals (*a), true, "C should equal A");

  NS_TEST_EXPECT_MSG_NE (a->Equals (*x), true, "A should not equal X");
  NS_TEST_EXPECT_MSG_NE (a->Equals (*y), true, "A should not equal Y");
  NS_TEST_EXPECT_MSG_NE (a->Equals (*z), true, "A should not equal Z");
}
EndTest ()

BeginTest (GetNexthops)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);

  TestVector t1 (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry1 = CreateRouteEntry (t1);
  TestVector t2 (prefix1,10,200);
  Ptr<CCNxRouteEntry> entry2 = CreateRouteEntry (t2);

  route->AddNexthop (entry1);
  route->AddNexthop (entry2);

  CCNxRoute::NexthopListType nexthops = route->GetNexthops ();
  NS_TEST_EXPECT_MSG_EQ (nexthops.size (), 2, "Should be 2 nexthops");
}
EndTest ()


BeginTest (Operator_Output)
{
  std::string expected ("{ Route ccnx:/NAME=foo, Nexthops : { { prefix: ccnx:/NAME=foo, cost: 10, connId: 100 }} }");

  Ptr<const CCNxName> prefix = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix);
  TestVector t (prefix,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  std::stringstream ss;
  ss << *route;
  std::string test = ss.str ();
  NS_TEST_EXPECT_MSG_EQ (expected, test, "Output operator does not match");
}
EndTest ()

BeginTest (size)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  TestVector t1a (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry1a = CreateRouteEntry (t1a);
  TestVector t1b (prefix1,10,200);
  Ptr<CCNxRouteEntry> entry1b = CreateRouteEntry (t1b);

  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  route->AddNexthop (entry1a);
  route->AddNexthop (entry1b);

  size_t test = route->size ();
  NS_TEST_EXPECT_MSG_EQ (test, 2, "Size should be 2");
}
EndTest ()

BeginTest (empty_true)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  NS_TEST_EXPECT_MSG_EQ (route->empty (), true, "The route should be empty");
}
EndTest ()

BeginTest (empty_false)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  TestVector t (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  NS_TEST_EXPECT_MSG_EQ (route->empty (), false, "The route should not be empty");
}
EndTest ()

BeginTest (clear)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  TestVector t (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);
  route->clear ();
  NS_TEST_EXPECT_MSG_EQ (route->empty (), true, "The route should be empty after a clear()");
}
EndTest ()

BeginTest (const_iterator_empty)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);

  CCNxRoute::const_iterator i = route->begin ();
  bool isEnd = i == route->end ();
  NS_TEST_EXPECT_MSG_EQ (isEnd, true, "Begin iterator should equal end for empty route");
}
EndTest ()

BeginTest (const_iterator_not_empty)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  TestVector t (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  CCNxRoute::const_iterator i = route->begin ();
  bool isNotEnd = i != route->end ();
  NS_TEST_EXPECT_MSG_EQ (isNotEnd, true, "Begin iterator should not equal end for empty route");
}
EndTest ()

BeginTest (const_iterator_cbegin_cend)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  TestVector t (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  CCNxRoute::const_iterator i = route->cbegin ();
  bool isNotEnd = i != route->cend ();
  NS_TEST_EXPECT_MSG_EQ (isNotEnd, true, "Begin iterator should not equal end for empty route");
}
EndTest ()

BeginTest (const_iterator_operator_assignmant)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  TestVector t (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  CCNxRoute::const_iterator i = route->begin ();

  CCNxRoute::const_iterator j = i;
  bool isEqual = j == i;
  NS_TEST_EXPECT_MSG_EQ (isEqual, true, "Assigned iterator j should equal original iterator i");
}
EndTest ()

BeginTest (const_iterator_operator_arrow)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  TestVector t (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  CCNxRoute::const_iterator i = route->begin ();
  uint32_t cost = i->GetCost ();
  NS_TEST_EXPECT_MSG_EQ (cost, 10, "Expected 10 cost");
}
EndTest ()

BeginTest (const_iterator_operator_dereference)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  TestVector t (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  CCNxRoute::const_iterator i = route->begin ();

  Ptr<const CCNxRouteEntry> test = *i;
  bool equal = entry->Equals (*test);
  NS_TEST_EXPECT_MSG_EQ (equal, true, "Iterator should dereference to the nexthop, expected " << *entry << ", got " << *test);
}
EndTest ()

BeginTest (const_iterator_operator_increment)
{
  Ptr<const CCNxName> prefix1 = Create<const CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxRoute> route = Create<CCNxRoute> (prefix1);
  TestVector t (prefix1,10,100);
  Ptr<CCNxRouteEntry> entry = CreateRouteEntry (t);
  route->AddNexthop (entry);

  CCNxRoute::const_iterator i = route->begin ();
  ++i;
  bool isEnd = i == route->end ();
  NS_TEST_EXPECT_MSG_EQ (isEnd, true, "incrementing begin with one element should equal end");
}
EndTest ()


/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxTime
 */
static class TestSuiteCCNxRoute : public TestSuite
{
public:
  TestSuiteCCNxRoute () : TestSuite ("ccnx-route", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new Constructor_Copy (), TestCase::QUICK);
    AddTestCase (new AddNexthop_Unique (), TestCase::QUICK);
    AddTestCase (new AddNexthop_Duplicate_SameDistance (), TestCase::QUICK);
    AddTestCase (new AddNexthop_Duplicate_LessDistance (), TestCase::QUICK);
    AddTestCase (new AddRoute (), TestCase::QUICK);
    AddTestCase (new RemoveNexthop_Missing (), TestCase::QUICK);
    AddTestCase (new RemoveNexthop_Exists (), TestCase::QUICK);
    AddTestCase (new GetNexthops (), TestCase::QUICK);

    AddTestCase (new Equals (), TestCase::QUICK);
    AddTestCase (new Operator_Output (), TestCase::QUICK);
    AddTestCase (new size (), TestCase::QUICK);
    AddTestCase (new empty_true (), TestCase::QUICK);
    AddTestCase (new empty_false (), TestCase::QUICK);
    AddTestCase (new clear (), TestCase::QUICK);
    AddTestCase (new const_iterator_empty (), TestCase::QUICK);
    AddTestCase (new const_iterator_not_empty (), TestCase::QUICK);
    AddTestCase (new const_iterator_cbegin_cend (), TestCase::QUICK);
    AddTestCase (new const_iterator_operator_assignmant (), TestCase::QUICK);
    AddTestCase (new const_iterator_operator_arrow (), TestCase::QUICK);
    AddTestCase (new const_iterator_operator_dereference (), TestCase::QUICK);
    AddTestCase (new const_iterator_operator_increment (), TestCase::QUICK);
  }
} g_TestSuiteCCNxRoute;

} // namespace TestSuiteCCNxRoute
