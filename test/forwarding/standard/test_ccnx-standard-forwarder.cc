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
#include "ns3/ccnx-standard-forwarder.h"
#include "../../mockups/mockup_ccnx-virtual-connection.h"
#include "ns3/ccnx-connection.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/integer.h"
#include "ns3/object-factory.h"
#include "ns3/ccnx-standard-content-store-factory.h"

#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxStandardForwarder {

//NS_LOG_COMPONENT_DEFINE ("TestCCNxStandardForwarder"); need a typeid defined, not sure how
//NS_OBJECT_ENSURE_REGISTERED (TestCCNxStandardForwarder);

static Ptr<CCNxPacket> _routeCallbackPacket;
static CCNxRoutingError::RoutingErrno _routeCallbackErrno;
static Ptr<CCNxConnectionList> _routeCallbackConnections;
static bool _routeCallbackFired;

static Time _layerDelay = MicroSeconds (1);

/*
 * used by the forwarder as the callback function for RouteIn and RouteOut.  After we make
 * a call, use Simulator::Run() for a bit, then check what got put here.
 */
static void
MockupRouteCallback (Ptr<CCNxPacket> packet, Ptr<CCNxConnection> ingress, enum CCNxRoutingError::RoutingErrno routingErrno,
                     Ptr<CCNxConnectionList> connectionList)
{
//  printf ("MockupRouteCallback(list size %zu)\n", connectionList->size ());
  _routeCallbackPacket = packet;
  _routeCallbackErrno = routingErrno;
  _routeCallbackConnections = connectionList;
  _routeCallbackFired = true;
}

Ptr<CCNxStandardForwarder> CreateForwarder (bool WithContentStore=true)
{
#if 0
  LogComponentEnable ("CCNxStandardForwarder", (LogLevel) (LOG_LEVEL_FUNCTION | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxStandardFib",(LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxStandardPit",(LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxStandardContentStore",(LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
//  LogComponentEnable ("CCNxStandardPitEntry",(LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
#endif

  Ptr<CCNxStandardForwarder> forwarder = CreateObject<CCNxStandardForwarder> ();
  if (WithContentStore)
    {
      static CCNxStandardContentStoreFactory factory;
      forwarder->SetAttribute ("ContentStoreFactory", ObjectFactoryValue (factory));
    }

  forwarder->SetAttribute ("LayerDelayConstant", TimeValue (_layerDelay));
  forwarder->SetAttribute ("LayerDelaySlope", TimeValue (Seconds (0)));
  forwarder->SetAttribute ("LayerDelayServers", IntegerValue (1));
  forwarder->SetRouteCallback (MakeCallback (&MockupRouteCallback));
  forwarder->Initialize ();  //runs the DoInitialize() method

  return forwarder;
}

static void
StepSimulator ()
{
  unsigned tries = 0;
  _routeCallbackFired = false;
  while (!_routeCallbackFired && tries < 4)
    {
      Simulator::Stop (_layerDelay);
      Simulator::Run ();
      //std::cout << "StepSimulator: tries="<<tries<< " simulator time=" << Simulator::Now() << std::endl ;
      tries++;
    }
  //NS3_LOG_DEBUG(std::cout << "callback fired!" << std::endl ;

  NS_ASSERT_MSG (_routeCallbackFired, "Did not get a call to RouteCallback");
}

BeginTest (Constructor)
{
  printf ("TestCCNxStandardForwarderConstructor DoRun\n");

}
EndTest ()

typedef struct
{
  Ptr<const CCNxName> prefix1;
  Ptr<const CCNxName> prefix2;
  Ptr<const CCNxName> prefix3;

  Ptr<CCNxVirtualConnection> ingress1;
  Ptr<CCNxVirtualConnection> ingress2;

  Ptr<CCNxVirtualConnection> nextHop1;
  Ptr<CCNxVirtualConnection> nextHop2;
  Ptr<CCNxVirtualConnection> nextHop3;

  Ptr<const CCNxName> name1;
  Ptr<const CCNxName> name2;
  Ptr<const CCNxName> name3;

  Ptr<CCNxInterest> interest1;
  Ptr<CCNxPacket> iPacket1;

  Ptr<CCNxInterest> interest2;
  Ptr<CCNxPacket> iPacket2;

  Ptr<CCNxInterest> interest3;
  Ptr<CCNxPacket> iPacket3;

  Ptr<CCNxContentObject> content1;
  Ptr<CCNxPacket> cPacket1;

  Ptr<CCNxContentObject> content2;
  Ptr<CCNxPacket> cPacket2;

  Ptr<CCNxContentObject> content3;
  Ptr<CCNxPacket> cPacket3;

  Ptr<CCNxRoute> route3;
} TestData;

static TestData
CreateTestData ()
{
  TestData data;
  data.prefix1 = Create<CCNxName> ("ccnx:/name=trump/name=is/name=a");
  data.prefix1 = Create<CCNxName> ("ccnx:/name=trump/name=is/name=forwarder");
  data.prefix2 = Create<CCNxName> ("ccnx:/name=catfur/name=and/name=mayonnaise");
  data.prefix3 = Create<CCNxName> ("ccnx:/name=my/name=old/name=LPs");

  data.ingress1 = Create<CCNxVirtualConnection> ();
  data.ingress2 = Create<CCNxVirtualConnection> ();

  data.nextHop1 = Create<CCNxVirtualConnection> ();
  data.nextHop2 = Create<CCNxVirtualConnection> ();
  data.nextHop3 = Create<CCNxVirtualConnection> ();

  data.name1 = Create<CCNxName> ("ccnx:/name=trump/name=is/name=forwarder/name=paradox");
  data.name2 = Create<CCNxName> ("ccnx:/name=catfur/name=and/name=mayonnaise/name=feels/name=good");
  data.name3 = Create<CCNxName> ("ccnx:/name=my/name=old/name=LPs/name=includes/name=CCR");

  data.interest1 = Create<CCNxInterest> (data.name1);
  data.iPacket1 = CCNxPacket::CreateFromMessage (data.interest1);

  data.interest2 = Create<CCNxInterest> (data.name2);
  data.iPacket2 = CCNxPacket::CreateFromMessage (data.interest2);

  data.interest3 = Create<CCNxInterest> (data.name3);
  data.iPacket3 = CCNxPacket::CreateFromMessage (data.interest3);

  data.content1 = Create<CCNxContentObject> (data.name1);
  data.cPacket1 = CCNxPacket::CreateFromMessage (data.content1);

  data.content2 = Create<CCNxContentObject> (data.name2);
  data.cPacket2 = CCNxPacket::CreateFromMessage (data.content2);

  data.content3 = Create<CCNxContentObject> (data.name3);
  data.cPacket3 = CCNxPacket::CreateFromMessage (data.content3);

  Ptr<CCNxRouteEntry> routeEntry3 = Create<CCNxRouteEntry> (data.prefix3, 1, data.nextHop3);
  data.route3 = Create<CCNxRoute> (data.prefix3);
  data.route3->AddNexthop (routeEntry3);

  return data;
}

static void
SetupRoutes (Ptr<CCNxStandardForwarder> forwarder, TestData &data)
{
  forwarder->AddRoute (data.nextHop1, data.prefix1);
  forwarder->AddRoute (data.nextHop2, data.prefix2);
  forwarder->AddRoute (data.route3);
}

static void
SetupTwoRoutesPrefix1 (Ptr<CCNxStandardForwarder> forwarder, TestData &data)
{
  //create forwarder route
  Ptr<CCNxRouteEntry> routeEntry3 = Create<CCNxRouteEntry> (data.prefix3, 1, data.nextHop3);
  Ptr<CCNxRoute> route3 = Create<CCNxRoute> (data.prefix3);
  route3->AddNexthop (routeEntry3);

  forwarder->AddRoute (data.nextHop1, data.prefix1);
  forwarder->AddRoute (data.nextHop2, data.prefix1);

  forwarder->AddRoute (route3);
}


BeginTest (OneInterestToOneContent)
{
  //b1 3 route one interest one content

  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder ();
  TestData data = CreateTestData ();
  SetupRoutes (forwarder, data);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 3, "wrong number of fib entries");

  //route an interest
  forwarder->RouteInput (data.iPacket1, data.ingress1); //new interest goes thru 4 delay queues - fwdr, pit, cs and fib before callback occurs
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.nextHop1->GetConnectionId (), "interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 1, "wrong number of pit entries");

  //route content
  forwarder->RouteInput (data.cPacket1, data.nextHop1); //matching content goes thru 3 delay queues - fwdr, pit, cs
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.ingress1->GetConnectionId (), "content not forwarded to interest source");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 0, "wrong number of pit entries");

  Simulator::Destroy ();
}
EndTest ()


BeginTest (OneInterestNonMatchingContent)
{
  //b2 3 route one interest one non-matching content

  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder ();
  TestData data = CreateTestData ();
  SetupRoutes (forwarder, data);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 3, "wrong number of fib entries");

  //route an interest
  forwarder->RouteInput (data.iPacket1, data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.nextHop1->GetConnectionId (),"interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 1, "wrong number of pit entries");

  //route content
  forwarder->RouteInput (data.cPacket2, data.nextHop1);
  StepSimulator ();
//  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoRoute,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 0, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 1, "wrong number of pit entries");
}
EndTest ()

BeginTest (TwoInterestOneContent)
{
  //b3 one route 2 interests one content

  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder ();
  TestData data = CreateTestData ();
  SetupRoutes (forwarder, data);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 3, "wrong number of fib entries");

  //route an interest
  forwarder->RouteInput (data.iPacket1, data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno, CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.nextHop1->GetConnectionId (), "interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 1, "wrong number of pit entries");

  //route same interest another port - should be aggregated
  forwarder->RouteInput (data.iPacket1, data.ingress2);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (),0,"wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 1, "wrong number of pit entries");

  //route content - connections should include both sources
  forwarder->RouteInput (data.cPacket1, data.nextHop1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (),2,"wrong number of connections");

  //order of connections in list is indeterminate, so check using sets
  typedef std::set <CCNxConnection::ConnIdType> connIdSet;
  connIdSet expConnIds,actConnIds;
  expConnIds.insert(data.ingress2->GetConnectionId ());
  expConnIds.insert(data.ingress1->GetConnectionId ());
  actConnIds.insert(_routeCallbackConnections->front ()->GetConnectionId ());
  actConnIds.insert(_routeCallbackConnections->back ()->GetConnectionId ());
  if (actConnIds != expConnIds)
    {
      NS_TEST_EXPECT_MSG_EQ (true,false, "expected and actual nextHop ConnectionIds dont match");
    }

  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 0, "wrong number of pit entries");
}
EndTest ()

/*
 * one route with multiple destinations. interest forwarded to 2 dests. content forwarded to sources list,
 * but with content source removed.
 */
BeginTest (InterestToTwo)
{
  //one interest to  multiple destinations

  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder ();
  TestData data = CreateTestData ();
  SetupTwoRoutesPrefix1 (forwarder, data);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 2, "wrong number of fib entries");

  //route  interest1 from ingress1 - should get 2 dests
  forwarder->RouteInput (data.iPacket1, data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (),2,"wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),1,"wrong number of pit entries");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.nextHop1->GetConnectionId (),"interest not forwarded to correct nextHop");
  _routeCallbackConnections->pop_front ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.nextHop2->GetConnectionId (),"interest not forwarded to correct nextHop");

  //route  interest1 from ingress2 - should be aggregated
  forwarder->RouteInput (data.iPacket1, data.ingress2);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (),0,"wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),1,"wrong number of pit entries");

  //route content from ingress1 - should be sent to only ingress2 because it arrived on ingress1
  // TODO CCN: make a test case where content object goes to both ingress1 and ingress2
  forwarder->RouteInput (data.cPacket1, data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.ingress2->GetConnectionId (),"interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 0, "wrong number of pit entries");
}
EndTest ()


/*
 * try to add the same route twice. add route all methods, remove Route all methods
 *
 */
BeginTest (DuplicateRoutes)
{
  //b6 try to add the same route twice. add route all methods, remove Route all methods

  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder ();
  TestData data = CreateTestData ();
  SetupRoutes (forwarder, data);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 3, "wrong number of fib entries");

  //repeat a route add
  forwarder->AddRoute (data.nextHop1, data.prefix1);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 3, "wrong number of fib entries");

  //route an interest for prefix1
  forwarder->RouteInput (data.iPacket1, data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (),1,"wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),1,"wrong number of pit entries");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (),data.nextHop1->GetConnectionId (),"interest not forwarded to correct nextHop");
  _routeCallbackConnections->pop_front ();

//delete a dest from a fib entry - should not delete entry since there is another dest
  forwarder->RemoveRoute (data.nextHop1,data.prefix2);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable),3,"wrong number of fib entries");


  //delete a dest from a fib entry - should  delete entry
  forwarder->RemoveRoute (data.nextHop3,data.prefix3);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable),2,"wrong number of fib entries");

  //route an interest for prefix3 - not forwarded - but is retained in pit
  forwarder->RouteInput (data.iPacket3,data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (),0,"wrong number of connections");
//  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoRoute,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),2,"wrong number of pit entries");

  forwarder->AddRoute (data.route3);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable),3,"wrong number of fib entries");

  forwarder->RemoveRoute (data.route3);

  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable),2,"wrong number of fib entries");

  //route content1
  forwarder->RouteInput (data.cPacket1, data.nextHop1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (),1,"wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.ingress1->GetConnectionId (),"interest not forwarded to correct nextHop");
  _routeCallbackConnections->pop_front ();
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),1,"wrong number of pit entries");


  printf ("TestCCNxStandardForwarder b6 End\n");

}
EndTest ()


/*
 * RouteOutput test of one interest to one content object
 */
BeginTest (RouteOutputOneInterestOneContent)
{
  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder ();
  TestData data = CreateTestData ();
  SetupRoutes (forwarder, data);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 3, "wrong number of fib entries");

  //route an interest

  forwarder->RouteOutput (data.iPacket1, data.ingress1, Ptr<CCNxConnection> (0));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (),data.nextHop1->GetConnectionId (),"interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),1,"wrong number of pit entries");

  //route content
  forwarder->RouteOutput (data.cPacket1,data.nextHop1, Ptr<CCNxConnection> (0));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (),data.ingress1->GetConnectionId (),"content not forwarded to interest source");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),0,"wrong number of pit entries");

  //explicit routing of interest
  Ptr<CCNxVirtualConnection> egressFromUser = Create<CCNxVirtualConnection> ();
  forwarder->RouteOutput (data.iPacket2,data.ingress1, egressFromUser);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (),egressFromUser->GetConnectionId (),"interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),1,"wrong number of pit entries");

  //route content
  forwarder->RouteOutput (data.cPacket2,data.nextHop1, Ptr<CCNxConnection> (0));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (),data.ingress1->GetConnectionId (),"content not forwarded to interest source");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),0,"wrong number of pit entries");


}
EndTest ()

BeginTest (RouteExactName)
{
  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder ();
  TestData data = CreateTestData ();
  SetupRoutes (forwarder, data);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 3, "wrong number of fib entries");


  //exact name routing
  Ptr<CCNxInterest> interest4 = Create<CCNxInterest> (data.name1);
  Ptr<CCNxPacket> iPacket4 = CCNxPacket::CreateFromMessage (interest4);

  Ptr<CCNxContentObject> content4 = Create<CCNxContentObject> (data.name1);
  Ptr<CCNxPacket> cPacket4 = CCNxPacket::CreateFromMessage (content4);

  forwarder->RouteOutput (iPacket4, data.ingress1, Ptr<CCNxConnection> (0));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (),data.nextHop1->GetConnectionId (),"interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),1,"wrong number of pit entries");

  forwarder->RouteOutput (cPacket4, data.nextHop1, Ptr<CCNxConnection> (0));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (),data.ingress1->GetConnectionId (),"content not forwarded to interest source");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable),0,"wrong number of pit entries");
}
EndTest ()

BeginTest (ContentStore)
{
  //route one interest one content then same interest which returns content from CS

  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder ();
  TestData data = CreateTestData ();
  SetupRoutes (forwarder, data);
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::FibTable), 3, "wrong number of fib entries");

  //route an interest
  forwarder->RouteInput (data.iPacket1, data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.nextHop1->GetConnectionId (), "interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 1, "wrong number of pit entries");

  //route content
  forwarder->RouteInput (data.cPacket1, data.nextHop1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.ingress1->GetConnectionId (), "content not forwarded to interest source");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 0, "wrong number of pit entries");

  //route same interest
   forwarder->RouteInput (data.iPacket1, data.ingress1);
   StepSimulator ();
   NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
   NS_TEST_EXPECT_MSG_EQ (_routeCallbackPacket, data.cPacket1, "wrong packet returned");
   NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
   NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.ingress1->GetConnectionId (), "content from CS not forwarded to correct nextHop");
   NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 0, "wrong number of pit entries");

  Simulator::Destroy ();
}
EndTest ()

BeginTest (NoContentStore)
{
  //route one interest one content then same interest which returns content from CS

  Ptr<CCNxStandardForwarder> forwarder = CreateForwarder (false);  //false => no content store
  TestData data = CreateTestData ();
  SetupRoutes (forwarder, data);


 // forwarder->SetAttribute("ContentStoreFactory",ObjectFactory());

  forwarder->Initialize();

  //route an interest
  forwarder->RouteInput (data.iPacket1, data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.nextHop1->GetConnectionId (), "interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 1, "wrong number of pit entries");

  //route content
  forwarder->RouteInput (data.cPacket1, data.nextHop1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.ingress1->GetConnectionId (), "content not forwarded to interest source");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 0, "wrong number of pit entries");

  //route same interest
  forwarder->RouteInput (data.iPacket1, data.ingress1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.nextHop1->GetConnectionId (), "interest not forwarded to correct nextHop");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 1, "wrong number of pit entries");

  //route content
  forwarder->RouteInput (data.cPacket1, data.nextHop1);
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackErrno,CCNxRoutingError::CCNxRoutingError_NoError,"wrong errorno");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->size (), 1, "wrong number of connections");
  NS_TEST_EXPECT_MSG_EQ (_routeCallbackConnections->front ()->GetConnectionId (), data.ingress1->GetConnectionId (), "content not forwarded to interest source");
  NS_TEST_EXPECT_MSG_EQ (forwarder->CountEntries (CCNxStandardForwarder::PitTable), 0, "wrong number of pit entries");

  Simulator::Destroy ();
}
EndTest ()



//#TODO CCN test forwarder stats
//#TODO CCN test no content store option set by attribute
//#TODO CCN fix delay model for content store
//#TODO CCN get rid of KEYIDHACK
//#TODO CCN get line coverage to 100%


/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for CCNxStandardForwarder
 */
static class TestSuiteCCNxStandardForwarder : public TestSuite
{
public:
  TestSuiteCCNxStandardForwarder () : TestSuite ("ccnx-standard-forwarder", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new OneInterestToOneContent (), TestCase::QUICK);
    AddTestCase (new OneInterestNonMatchingContent (), TestCase::QUICK);
    AddTestCase (new TwoInterestOneContent (), TestCase::QUICK);
//   AddTestCase (new InterestReturnPacket, TestCase::QUICK); TODO CCN write this case
    AddTestCase (new InterestToTwo (), TestCase::QUICK);
    AddTestCase (new DuplicateRoutes (), TestCase::QUICK);
    AddTestCase (new RouteOutputOneInterestOneContent (), TestCase::QUICK);
    AddTestCase (new RouteExactName (), TestCase::QUICK);
    AddTestCase (new ContentStore (), TestCase::QUICK);
    AddTestCase (new NoContentStore (), TestCase::QUICK);

  }
} g_TestSuiteCCNxStandardForwarder;


} // namespace TestSuiteCCNxStandardForwarder
