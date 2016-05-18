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

#include "../../mockups/mockup_ccnx-virtual-connection.h"
#include "ns3/test.h"
#include "ns3/ccnx-standard-fib.h"
#include "ns3/ccnx-standard-fibEntry.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/ccnx-forwarder-message.h"

#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxStandardFib {

static Time _layerDelay = MicroSeconds (10);

static Ptr<CCNxPacket> _lookupCallbackPacket;
static Ptr<CCNxConnectionList> _lookupCallbackConnections;
static bool _lookupCallbackFired;

/*
 * used by the PIT as the callback function for ReceiveInterest.  After we make
 * a call, use Simulator::Run() for a bit, then check what got put here.
 */
static void
MockupReceiveInterestCallback (Ptr<CCNxForwarderMessage> message, Ptr<CCNxConnectionList> egress)
{
  _lookupCallbackPacket = message->GetPacket ();
  _lookupCallbackConnections = egress;
  _lookupCallbackFired = true;
}

static void
StepSimulator ()
{
  unsigned tries = 0;
  _lookupCallbackFired = false;
  while (!_lookupCallbackFired && tries < 4)
    {
      Simulator::Stop (_layerDelay);
      Simulator::Run ();
      tries++;
    }

  NS_ASSERT_MSG (_lookupCallbackFired, "Did not get a call to LookupCallback");
}


static Ptr<CCNxStandardFib>
CreateFib ()
{
  Ptr<CCNxStandardFib> fib = CreateObject<CCNxStandardFib> ();
  fib->SetLookupCallback (MakeCallback (&MockupReceiveInterestCallback));
  fib->Initialize ();
  return fib;
}

static Ptr<CCNxPacket>
PacketFromName (Ptr<const CCNxName> name)
{
  return CCNxPacket::CreateFromMessage (CreateObject<CCNxInterest> (name));
}

static Ptr<CCNxStandardForwarderWorkItem>
CreateWorkItem (Ptr<CCNxPacket> packet, Ptr<CCNxConnection> ingress)
{
  Ptr<CCNxStandardForwarderWorkItem> item = Create<CCNxStandardForwarderWorkItem> (packet, ingress, Ptr<CCNxConnection> (0));
  return item;
}

BeginTest (Constructor)
{
  printf ("TestCCNxStandardFibConstructor DoRun\n");
//  LogComponentEnable("CCNxStandardFibEntry", LOG_LEVEL_DEBUG);
//  LogComponentEnable("CCNxStandardFib", LOG_LEVEL_DEBUG);
}
EndTest ()

BeginTest (Lookup)
{
  /* add 2 routes. lookup both as exact and one as lpm. lookup a route not in fib.
   *
   */
  printf ("TestCCNxStandardLookup DoRun\n");

  Ptr<CCNxStandardFib> fib = CreateFib ();

  Ptr<const CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is");
  Ptr<const CCNxName> name1l = Create<CCNxName> ("ccnx:/name=trump/name=is/name=a/name=bonehead");
  Ptr<const CCNxName> name2 = Create<CCNxName> ("ccnx:/name=kasich");
  Ptr<const CCNxName> name3 = Create<CCNxName> ("ccnx:/name=rubio");
  Ptr<CCNxStandardFibEntry>  fibEntryTest = Create <CCNxStandardFibEntry> ();
  Ptr<CCNxConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxConnection> connection2 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxConnection> connection3 = Create<CCNxVirtualConnection> ();

  fib->AddRoute (name1, connection1);
  fib->AddRoute (name2, connection2);
  fib->PrintRoutes (std::cout);

  NS_TEST_EXPECT_MSG_EQ (fib->CountEntries (), 2, "wrong number of fib entries !");

  fib->Lookup (CreateWorkItem ( PacketFromName (name1), connection3));
  StepSimulator ();

  NS_TEST_EXPECT_MSG_EQ (connection1, _lookupCallbackConnections->front (), "lookup did not return correct connection !");

  fib->Lookup (CreateWorkItem ( PacketFromName (name1l), connection3));
  StepSimulator ();

  NS_TEST_EXPECT_MSG_EQ (connection1, _lookupCallbackConnections->front (), "lookup did not return correct connection !");

  fib->Lookup (CreateWorkItem (PacketFromName (name2), connection3));
  StepSimulator ();

  NS_TEST_EXPECT_MSG_EQ (connection2, _lookupCallbackConnections->front (), "lookup did not return correct connection !");

  fib->Lookup (CreateWorkItem (PacketFromName (name2), connection2));
  StepSimulator ();

  NS_TEST_EXPECT_MSG_EQ (_lookupCallbackConnections->size (), 0, "lookup did not return empty list!");

  fib->Lookup (CreateWorkItem (PacketFromName (name3), connection3));
  StepSimulator ();

  NS_TEST_EXPECT_MSG_EQ (_lookupCallbackConnections->size (), 0, "lookup did not return empty list!");

  printf ("TestCCNxStandardLookup End\n");
}
EndTest ()

BeginTest (LookupBestMatch)
{
  /* add 4 routes. verify lookup chooses correct one.
   *
   */
  printf ("TestCCNxStandardLookupBestMatch DoRun\n");

  Ptr<CCNxStandardFib> fib = CreateFib ();

  Ptr<CCNxStandardFibEntry>  fibEntryTest = Create <CCNxStandardFibEntry> ();
  // std::vector<CCNxVirtualConnection::ConnIdType> nextHops[ ] = {(CCNxVirtualConnection::ConnIdType) 1,2,3,4,5,6,7};

  Ptr<CCNxVirtualConnection> nextHop0 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop2 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop3 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection4 = Create<CCNxVirtualConnection> ();

  Ptr<const CCNxName>  name0 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb");
  Ptr<const CCNxName>  name1 = Create<CCNxName> ("ccnx:/name=aaab/name=bbbb/name=cccc/name=dddd");
  Ptr<const CCNxName>  name2 = Create<CCNxName> ("ccnx:/name=aaab/name=bbbb/name=eeee");
  Ptr<const CCNxName>  name3 = Create<CCNxName> ("ccnx:/name=aaab/name=bbbb/name=eeee/name=ffff");
  Ptr<const CCNxName>  name4 = Create<CCNxName> ("ccnx:/name=aaab/name=bbbb/name=eeee/name=fffg"); //should match name2
  Ptr<const CCNxName>  name5 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=cccc/name=zzzz");//should match name0
  Ptr<const CCNxName>  name6 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb"); //should match name0

  fib->AddRoute (name0,nextHop0);
  fib->AddRoute (name1,nextHop1);
  fib->AddRoute (name2,nextHop2);
  fib->AddRoute (name3,nextHop3);
  NS_TEST_EXPECT_MSG_EQ (fib->CountEntries (), 4, "wrong number of fib entries !");

  fib->PrintRoutes (std::cout);

  fib->Lookup (CreateWorkItem (PacketFromName (name4), connection4));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_lookupCallbackConnections->front (), nextHop2,  "lookup did not return correct connection !");

  fib->Lookup (CreateWorkItem (PacketFromName (name5), connection4));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_lookupCallbackConnections->front (), nextHop0, "lookup did not return correct connection !");

  fib->Lookup (CreateWorkItem (PacketFromName (name6), connection4));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_lookupCallbackConnections->front (), nextHop0, "lookup did not return correct connection !");

  printf ("TestCCNxStandardLookupBestMatch End\n");
}
EndTest ()

BeginTest (DeleteRoute)
{
  /* add 2 routes. delete one. lookup existing and nonexisting routes.
   *
   */
  printf ("TestCCNxStandardDeleteRoute DoRun\n");

  Ptr<CCNxStandardFib> fib = CreateFib ();

  Ptr<const CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump");
  Ptr<const CCNxName> name1l = Create<CCNxName> ("ccnx:/name=trump/name=is/name=a/name=bonehead");
  Ptr<const CCNxName> name2 = Create<CCNxName> ("ccnx:/name=kasich");
  Ptr<CCNxStandardFibEntry>  fibEntryTest = Create <CCNxStandardFibEntry> ();
  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection2 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection3 = Create<CCNxVirtualConnection> ();

  fib->AddRoute (name1, connection1);
  fib->AddRoute (name2, connection2);
  NS_TEST_EXPECT_MSG_EQ (fib->CountEntries (), 2, "wrong number of fib entries !");

  fib->DeleteRoute (name2,connection2);
  NS_TEST_EXPECT_MSG_EQ (fib->CountEntries (), 1, "wrong number of fib entries !");

  fib->Lookup (CreateWorkItem (PacketFromName (name2), connection3));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (_lookupCallbackConnections->size (), 0, "lookup did not return empty connection list!");

  fib->Lookup (CreateWorkItem (PacketFromName (name1l), connection3));
  StepSimulator ();

  NS_TEST_EXPECT_MSG_EQ (connection1, _lookupCallbackConnections->front (), "lookup did not return correct connection !");

  fib->PrintRoutes (std::cout);
  fib->PrintRoute (std::cout, name1);

  printf ("TestCCNxStandardDeleteRoute End\n");
}
EndTest ()

BeginTest (AddRoutes)
{
  /* add 2 routes.
   *
   */
  printf ("TestCCNxStandardAddRoutes DoRun\n");

  CCNxStandardFib a;

  std::vector< Ptr<const CCNxName> > myNames;

  Ptr<CCNxVirtualConnection> nextHop0 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop1 = Create<CCNxVirtualConnection> ();


  Ptr<const CCNxName>  name0 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb");
  Ptr<const CCNxName>  name1 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=cccc/name=dddd");


  a.AddRoute (name0,nextHop0);
  a.AddRoute (name1,nextHop1);

  NS_TEST_EXPECT_MSG_EQ (a.CountEntries (), 2, "wrong number of fib entries !");

  a.PrintRoutes (std::cout);


  printf ("TestCCNxStandardAddRoutes End\n");


}
EndTest ()


BeginTest (DeleteNonExistentRoute)
{
  /* add several routes. delete nonexistent ones.
   *
   */
  printf ("TestCCNxStandardFibDeleteNonExistentRoute DoRun\n");

  CCNxStandardFib a;

  std::vector< Ptr<const CCNxName> > myNames;

  Ptr<CCNxVirtualConnection> nextHop0 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop2 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop3 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop4 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop5 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop6 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop7 = Create<CCNxVirtualConnection> ();

  Ptr<const CCNxName>  name0 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb");
  Ptr<const CCNxName>  name1 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=cccc/name=dddd");
  Ptr<const CCNxName>  name2 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=eeee");
  Ptr<const CCNxName>  name3 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=eeee/name=ffff");

  Ptr<const CCNxName>  name4 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=eeee/name=fffg");  //should match name2
  Ptr<const CCNxName>  name5 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=cccd"); //should match name0
  Ptr<const CCNxName>  name6 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb");  //should match name0?

  Ptr<const CCNxName>  name7 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=cccc");

  a.AddRoute (name0,nextHop0);
  a.AddRoute (name1,nextHop1);
  a.AddRoute (name2,nextHop2);
  a.AddRoute (name3,nextHop3);
  a.AddRoute (name4,nextHop4);
  a.AddRoute (name5,nextHop5);
  a.AddRoute (name6,nextHop6);  //this is a duplicate so should not be added

  a.PrintRoutes (std::cout);
  NS_TEST_EXPECT_MSG_EQ (a.CountEntries (), 6, "wrong number of fib entries !");


  bool ok = a.DeleteRoute (name7,nextHop7); //try to delete nonmatching name
  NS_TEST_EXPECT_MSG_EQ (ok, false, "attempt to delete nonexistent route did not return false !");

  ok = a.DeleteRoute (name0,nextHop1);      //try to delete nonmatching connection
  NS_TEST_EXPECT_MSG_EQ (ok, false, "attempt to delete nonexistent route did not return false !");

  a.PrintRoutes (std::cout);


  printf ("TestCCNxStandardFibDeleteNonExistentRoute End\n");


}
EndTest ()

BeginTest (DeleteAllRoutes)
{
  /* add several routes. delete them all.
   *
   */
  printf ("TestCCNxStandardFibDeleteAllRoutes DoRun\n");

  CCNxStandardFib a;

  std::vector< Ptr<const CCNxName> > myNames;

  Ptr<CCNxVirtualConnection> nextHop0 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop2 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop3 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop4 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop5 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop6 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> nextHop7 = Create<CCNxVirtualConnection> ();

  Ptr<const CCNxName>  name0 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb");
  Ptr<const CCNxName>  name1 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=cccc/name=dddd");
  Ptr<const CCNxName>  name2 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=eeee");
  Ptr<const CCNxName>  name3 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=eeee/name=ffff");
  Ptr<const CCNxName>  name4 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=eeee/name=fffg");
  Ptr<const CCNxName>  name5 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=cccd");
  Ptr<const CCNxName>  name6 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=zzzz");
  Ptr<const CCNxName>  name7 = Create<CCNxName> ("ccnx:/name=aaaa/name=bbbb/name=cccc");

  a.AddRoute (name0,nextHop0);
  a.AddRoute (name1,nextHop1);
  a.AddRoute (name2,nextHop2);
  a.AddRoute (name3,nextHop3);
  a.AddRoute (name4,nextHop4);
  a.AddRoute (name5,nextHop5);
  a.AddRoute (name6,nextHop6);
  a.AddRoute (name7,nextHop7);

  a.PrintRoutes (std::cout);

  NS_TEST_EXPECT_MSG_EQ (a.CountEntries (), 8, "wrong number of fib entries !");


  bool ok;
  ok = a.DeleteRoute (name0,nextHop0);
  ok = a.DeleteRoute (name1,nextHop1);
  ok = a.DeleteRoute (name2,nextHop2);
  ok = a.DeleteRoute (name3,nextHop3);
  ok = a.DeleteRoute (name4,nextHop4);
  ok = a.DeleteRoute (name5,nextHop5);
  ok = a.DeleteRoute (name6,nextHop6);
  ok = a.DeleteRoute (name7,nextHop7);

  NS_TEST_EXPECT_MSG_EQ (a.CountEntries (), 0, "wrong number of fib entries !");

  a.PrintRoutes (std::cout);


  printf ("TestCCNxStandardFibDeleteAllRoutes End\n");


}
EndTest ()

BeginTest (RemoveConnection)
{
  /* add 2 routes sharing some ConnIds. remove common ConnIds. verify unshared routes remain.
   *
   */
  printf ("TestCCNxStandardRemoveConnection DoRun\n");

  Ptr<CCNxStandardFib> fib = CreateFib ();

  Ptr<const CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump");
  Ptr<const CCNxName> name2 = Create<CCNxName> ("ccnx:/name=kasich");
  Ptr<const CCNxName> name1l = Create<CCNxName> ("ccnx:/name=trump/name=is/name=a/name=bonehead");

  Ptr<CCNxStandardFibEntry>  fibEntryTest = Create <CCNxStandardFibEntry> ();
  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection2 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection3 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection4 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection5 = Create<CCNxVirtualConnection> ();

  fib->AddRoute (name1, connection1);
  fib->AddRoute (name1, connection2);
  fib->AddRoute (name1, connection3);
  fib->AddRoute (name2, connection1);
  fib->AddRoute (name2, connection2);
  fib->AddRoute (name2, connection4);

  fib->PrintRoutes (std::cout);

  fib->RemoveConnection (connection1);
  fib->RemoveConnection (connection2);

  NS_TEST_EXPECT_MSG_EQ (fib->CountEntries (), 2, "wrong number of fib entries !");

  fib->PrintRoutes (std::cout);

  fib->Lookup (CreateWorkItem (PacketFromName (name1l), connection5));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (connection3, _lookupCallbackConnections->front (), "lookup did not return correct connection!");

  fib->Lookup (CreateWorkItem (PacketFromName (name2), connection5));
  StepSimulator ();
  NS_TEST_EXPECT_MSG_EQ (connection4, _lookupCallbackConnections->front (), "lookup did not return correct connection !");

  //remove all connections from an entry and it should be deleted
  NS_TEST_EXPECT_MSG_EQ (fib->CountEntries (), 2, "wrong number of fib entries");

  fib->RemoveConnection (connection3);
  NS_TEST_EXPECT_MSG_EQ (fib->CountEntries (), 1, "wrong number of fib entries");

  printf ("TestCCNxStandardRemoveConnection End\n");
}
EndTest ()




/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for CCNxStandardFib
 */
static class TestSuiteCCNxStandardFib : public TestSuite
{
public:
  TestSuiteCCNxStandardFib () : TestSuite ("ccnx-standard-fib", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new AddRoutes (), TestCase::QUICK);
    AddTestCase (new Lookup (), TestCase::QUICK);
    AddTestCase (new DeleteRoute (), TestCase::QUICK);
    AddTestCase (new RemoveConnection (), TestCase::QUICK);
    AddTestCase (new DeleteNonExistentRoute (), TestCase::QUICK );
    AddTestCase (new DeleteAllRoutes (), TestCase::QUICK );
    AddTestCase (new LookupBestMatch (), TestCase::QUICK );
  }
} g_TestSuiteCCNxStandardFib;

} // namespace TestSuiteCCNxStandardFib
