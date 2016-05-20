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
#include "ns3/ccnx-standard-pit.h"
#include "../../mockups/mockup_ccnx-virtual-connection.h"
#include "ns3/log.h"
#include "ns3/assert.h"

#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxStandardPit {

  /**
   * True if `entry` is in `connlist`
   *
   * Matched by pointer address
   *
   * @param connlist
   * @param entry
   * @return true if entry is in connlist.
   */
  static bool
  listContains(Ptr<CCNxConnectionList> connlist, Ptr<CCNxConnection> entry)
  {
    for (CCNxConnectionList::iterator i = connlist->begin(); i != connlist->end(); ++i) {
	Ptr<CCNxConnection> test = *i;
	if (test == entry) {
	    return true;
	}
    }
    return false;
  }

//DONE
//BASIC TESTS (no keyid, expiry or hash restrictions)
//b1 one interest one matching content
//b1a one interest one nonmatching content
//b2 two different name interests two matching contents
//b3 two interests same port one matching content
//b4 two interests one content, repeat
//b5 no interest one content

//TODO CCN - KEYID and HASH unit tests as listed below
// KEYID
//1 keyid interest, content key doesnt match
//receiveInterest -  with keyid restriction - verdict forward
//satisfyInterest -  content wrong keyid  - return no ports, not empty
//1 keyid interest, 1 no key id interest, content  matches both
//receiveInterest -  no keyid restriction - verdict forward
//receiveInterest -  with keyid restriction - verdict aggregate
//satisfyInterest - matching content for keyid  - return 2 ports, empty
// 2 keyid interests, content matches keyid 1
//receiveInterest -  with keyid restriction 1- verdict forward
//receiveInterest -  with keyid restriction 2 - verdict aggregate
//satisfyInterest - matching content for keyid 1 - return 1 port, not empty
// 1 keyid interest, 1 no key id interest, content matches no keyid
//receiveInterest -  no keyid restriction
//receiveInterest -  with keyid restriction
//satisfyInterest - matching content for nokeyid  - return 1 port, not empty
// 2 same keyid interests different ports, content matches both
//receiveInterest -  with keyid restriction 1- verdict forward
//receiveInterest -  with keyid restriction 1 - verdict aggregate
//satisfyInterest - matching content for keyid 1 - return 2 port,  empty



//HASH (only affects DoesObjectSatisfyInterest)
//1 interest with name and hash, matching content
//recieveInterest with name and hash restriction  - should return verdict forward
//satisfyInterest with name and hash restriction - should return 1 port, empty
//1 interest with name and hash, content with matching name but non matching hash
//recieveInterest with name and hash restriction  - should return verdict forward
//satisfyInterest with name and hash restriction - should return no port, not empty
//1 interest with no name and hash, matching content
//recieveInterest with no name and hash restriction  - should return verdict forward
//satisfyInterest with no name and hash restriction - should return 1 port, empty
//1 interest with no name and hash, matching hash but different name content
//recieveInterest with no name and hash restriction  - should return verdict forward
//satisfyInterest with different name and hash restriction - should return 1 port, empty




// EXPIRY
//1 interest 1 content but interest expired
//receiveInterest - no key restriction
//satisfyInterest - matching content - but interest has expired
//1 interest 1 content but content expired
//receiveInterest - no key restriction
//satisfyInterest - matching content - but content has expired
//2 interest 1 content but one interest has expired
//receiveInterest - no key restriction
//receiveInterest - no key restriction
//satisfyInterest - matching content - but one interest has expired


static Time _layerDelay = MicroSeconds (1);

static Ptr<CCNxPacket> _receiveInterestCallbackPacket;
static enum CCNxPit::Verdict _receiveInterestCallbackVerdict;
static bool _receiveInterestCallbackFired;

/*
 * used by the PIT as the callback function for ReceiveInterest.  After we make
 * a call, use Simulator::Run() for a bit, then check what got put here.
 */
static void
MockupReceiveInterestCallback (Ptr<CCNxForwarderMessage> message, enum CCNxPit::Verdict verdict)
{
  _receiveInterestCallbackPacket = message->GetPacket ();
  _receiveInterestCallbackVerdict = verdict;
  _receiveInterestCallbackFired = true;
}


static Ptr<CCNxPacket> _satisfyInterestCallbackPacket;
static Ptr<CCNxConnectionList> _satisfyInterestCallbackConnections;
static bool _satisfyInterestCallbackFired;

/*
 * used by the PIT as the callback function for SatisfyInterest.  After we make
 * a call, use Simulator::Run() for a bit, then check what got put here.
 */
static void
MockupSatisfyInterestCallback (Ptr<CCNxForwarderMessage> message, Ptr<CCNxConnectionList> egress)
{
  _satisfyInterestCallbackPacket = message->GetPacket ();
  _satisfyInterestCallbackConnections = egress;
  _satisfyInterestCallbackFired = true;
}

Ptr<CCNxStandardPit>
CreatePit ()
{
  Ptr<CCNxStandardPit> pit = CreateObject<CCNxStandardPit> ();
  pit->SetReceiveInterestCallback (MakeCallback (&MockupReceiveInterestCallback));
  pit->SetSatisfyInterestCallback (MakeCallback (&MockupSatisfyInterestCallback));
  pit->Initialize ();
  return pit;
}

static void
StepSimulatorReceiveInterest ()
{
  unsigned tries = 0;
  _receiveInterestCallbackFired = false;
  while (!_receiveInterestCallbackFired && tries < 4)
    {
      Simulator::Stop (_layerDelay);
      Simulator::Run ();
      tries++;
    }

  NS_ASSERT_MSG (_receiveInterestCallbackFired, "Did not get a call to ReceiveInterestCallback");
}

static void
StepSimulatorSatisfyInterest ()
{
  unsigned tries = 0;
  _satisfyInterestCallbackFired = false;
  while (!_satisfyInterestCallbackFired && tries < 4)
    {
      Simulator::Stop (_layerDelay);
      Simulator::Run ();
      tries++;
    }

  NS_ASSERT_MSG (_receiveInterestCallbackFired, "Did not get a call to SatisfyInterestCallback");
}


BeginTest (Constructor)
{
  printf ("TestCCNxStandardPitConstructor DoRun\n");
  LogComponentEnable ("CCNxStandardPitEntry", LOG_LEVEL_DEBUG);
  LogComponentEnable ("CCNxStandardPit", LOG_LEVEL_DEBUG);

}
EndTest ()

BeginTest (b1)
{
  //one interest one content
  //receiveInterest - verdict forward
  //satisfyInterest - matching content - should return empty,value

  printf ("TestCCNxStandardPitb1 DoRun\n");

  Ptr<CCNxStandardPit> pit = CreatePit ();

  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is");
  Ptr<CCNxInterest> interest1 = Create<CCNxInterest> (name1);
  Ptr<CCNxContentObject> content1 = Create<CCNxContentObject> (name1);
  Ptr<CCNxPacket> iPacket1 = CCNxPacket::CreateFromMessage (interest1);
  Ptr<CCNxPacket> cPacket1 = CCNxPacket::CreateFromMessage (content1);
  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection2 = Create<CCNxVirtualConnection> ();



  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection1));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacket1, connection2));
  StepSimulatorSatisfyInterest ();

  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 1, "wrong number of connections!");
  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->front (), connection1, "wrong connId!");


  printf ("TestCCNxStandardPitb1 End\n");

}
EndTest ()
BeginTest (b1b)
{
  //one interest one content - no names hash only
  //receiveInterest - verdict forward
  //satisfyInterest - matching content - should return empty,value

  printf ("TestCCNxStandardPitb1b DoRun\n");

  Ptr<CCNxStandardPit> pit = CreatePit ();

  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/");
  Ptr<CCNxInterest> interest1 = Create<CCNxInterest> (name1);
  Ptr<CCNxContentObject> content1 = Create<CCNxContentObject> (name1);
  Ptr<CCNxPacket> iPacket1 = CCNxPacket::CreateFromMessage (interest1);
  Ptr<CCNxPacket> cPacket1 = CCNxPacket::CreateFromMessage (content1);
  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection2 = Create<CCNxVirtualConnection> ();


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection1));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacket1, connection2));
  StepSimulatorSatisfyInterest ();

  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 1, "wrong number of connections!");

  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->front (), connection1, "wrong connId!");

  printf ("TestCCNxStandardPitb1b End\n");

}
EndTest ()

BeginTest (b1a)
{
  //one interest one content
  //receiveInterest - verdict forward
  //satisfyInterest - nonmatching content - should return no result

  printf ("TestCCNxStandardPitb1a DoRun\n");

  Ptr<CCNxStandardPit> pit = CreatePit ();

  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is");
  Ptr<CCNxInterest> interest1 = Create<CCNxInterest> (name1);
  Ptr<CCNxPacket> iPacket1 = CCNxPacket::CreateFromMessage (interest1);

  Ptr<CCNxName> name2 = Create<CCNxName> ("ccnx:/name=trump/name=is/name=a");
  Ptr<CCNxContentObject> content2 = Create<CCNxContentObject> (name2);
  Ptr<CCNxPacket> cPacket2 = CCNxPacket::CreateFromMessage (content2);

  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection2 = Create<CCNxVirtualConnection> ();


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection1));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacket2, connection2));
  StepSimulatorSatisfyInterest ();

  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 0, "should not have gotten back any connections!");

  printf ("TestCCNxStandardPitb1a End\n");

}
EndTest ()


BeginTest (b2)
{
  //two interests different ports one content
  //receiveInterest - verdict forward
  //receiveInterest -  different port - verdict aggregate
  //satisfyInterest - matching content - should return 2 port list, empty

  printf ("TestCCNxStandardPitb2 DoRun\n");

  Ptr<CCNxStandardPit> pit = CreatePit ();

  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is");
  Ptr<CCNxInterest> interest1 = Create<CCNxInterest> (name1);
  Ptr<CCNxContentObject> content1 = Create<CCNxContentObject> (name1);
  Ptr<CCNxPacket> iPacket1 = CCNxPacket::CreateFromMessage (interest1);
  Ptr<CCNxPacket> cPacket1 = CCNxPacket::CreateFromMessage (content1);
  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection2 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection3 = Create<CCNxVirtualConnection> ();

  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection1));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");

  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection2));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Aggregate, "verdict should be Aggregate !");

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacket1, connection3));
  StepSimulatorSatisfyInterest ();

  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 2, "wrong number of connId returned!");

  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection1), true, "connection1 not in list");
  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection2), true, "connection2 not in list");
}
EndTest ()

BeginTest (b3)
{
  //two interests same port one content
  //receiveInterest - verdict forward
  //receiveInterest -  same port - verdict forward
  //satisfyInterest - matching content = return 1 port, empty

  printf ("TestCCNxStandardPitb3 DoRun\n");

  Ptr<CCNxStandardPit> pit = CreatePit ();

  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is");
  Ptr<CCNxInterest> interest1 = Create<CCNxInterest> (name1);
  Ptr<CCNxContentObject> content1 = Create<CCNxContentObject> (name1);
  Ptr<CCNxPacket> iPacket1 = CCNxPacket::CreateFromMessage (interest1);
  Ptr<CCNxPacket> cPacket1 = CCNxPacket::CreateFromMessage (content1);
  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection3 = Create<CCNxVirtualConnection> ();


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection1));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");

  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection1));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be Forward !");

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacket1, connection3));
  StepSimulatorSatisfyInterest ();

  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 1, "wrong number of connections returned!");
  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection1), true, "connection1 not in list");

  printf ("TestCCNxStandardPitb3 End\n");

}
EndTest ()
BeginTest (b4)
{
  //outstanding interests for two different named contents
  //receiveInterest - verdict forward
  //receiveInterest - verdict aggregate
  //receiveInterest - verdict aggregate
  //receiveInterest - verdict forward
  //receiveInterest - verdict aggregate
  //receiveInterest - verdict aggregate
  //satisfyInterest - matching content 1 = return all ports, empty
  //satisfyInterest - matching content 2 = return all ports, empty

  printf ("TestCCNxStandardPitb4 DoRun\n");

  Ptr<CCNxStandardPit> pit = CreatePit ();

  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is");
  Ptr<CCNxInterest> interest1 = Create<CCNxInterest> (name1);
  Ptr<CCNxPacket> iPacket1 = CCNxPacket::CreateFromMessage (interest1);

  Ptr<CCNxContentObject> content1 = Create<CCNxContentObject> (name1);
  Ptr<CCNxPacket> cPacket1 = CCNxPacket::CreateFromMessage (content1);


//  iPacket1->SetContentObjectHash(0);  meaningless
//  cPacket1->SetContentObjectHash(0);  works but no comparison occurs due to previous line

  Ptr<CCNxVirtualConnection> connection10 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection20 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection30 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection40 = Create<CCNxVirtualConnection> ();

  Ptr<CCNxName> name2 = Create<CCNxName> ("ccnx:/name=trump/name=is/name=scary");
  Ptr<CCNxInterest> interest2 = Create<CCNxInterest> (name2);
  Ptr<CCNxPacket> iPacket2 = CCNxPacket::CreateFromMessage (interest2);

  Ptr<CCNxContentObject> content2 = Create<CCNxContentObject> (name2);
  Ptr<CCNxPacket> cPacket2 = CCNxPacket::CreateFromMessage (content2);


  Ptr<CCNxVirtualConnection> connection50 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection60 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection70 = Create<CCNxVirtualConnection> ();


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection10));
  StepSimulatorReceiveInterest ();
  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");

  printf ("map after 1 interest\n");
  pit->Print (std::cout);


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection20));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Aggregate, "verdict should be Aggregate !");
  printf ("map after 2 interest\n");
  pit->Print (std::cout);

  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket1, connection30));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Aggregate, "verdict should be Aggregate !");
  printf ("map after 3 interest\n");
  pit->Print (std::cout);

  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket2, connection40));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");
  printf ("map after 4 interest\n");
  pit->Print (std::cout);

  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket2, connection50));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Aggregate, "verdict should be Aggregate !");


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacket2, connection60));
  StepSimulatorReceiveInterest ();

  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Aggregate, "verdict should be Aggregate !");

  Ptr<CCNxConnectionList> connections;

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacket1, connection40));
  StepSimulatorSatisfyInterest ();

  printf ("map after 1 content\n");
  pit->Print (std::cout);
  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 3, "wrong number of connections returned!");
  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection10), true, "connection10 not in list");
  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection20), true, "connection20 not in list");
  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection30), true, "connection30 not in list");


  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacket2, connection70));
  StepSimulatorSatisfyInterest ();

  printf ("map after 2 content\n");
  pit->Print (std::cout);
  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 3, "wrong number of connections returned!");

  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection40), true, "connection40 not in list");
  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection50), true, "connection50 not in list");
  NS_TEST_EXPECT_MSG_EQ (listContains(_satisfyInterestCallbackConnections, connection60), true, "connection60 not in list");

  printf ("TestCCNxStandardPitb4 End\n");

}
EndTest ()


BeginTest (b5)
{
  //no interest one content
  //satisfyInterest - should return empty,no port

  printf ("TestCCNxStandardPitb5 DoRun\n");

  Ptr<CCNxStandardPit> pit = CreatePit ();

  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is");
  Ptr<CCNxInterest> interest1 = Create<CCNxInterest> (name1);
  Ptr<CCNxContentObject> content1 = Create<CCNxContentObject> (name1);
  Ptr<CCNxPacket> iPacket1 = CCNxPacket::CreateFromMessage (interest1);
  Ptr<CCNxPacket> cPacket1 = CCNxPacket::CreateFromMessage (content1);

  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacket1, connection1));
  StepSimulatorSatisfyInterest ();

  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 0, "wrong number of connections returned!");

  printf ("TestCCNxStandardPitb5 End\n");
}
EndTest ()




BeginTest (b6)
{

  //1 interest with no restriction, from two source ports
  //1 interest with hash and keyid restriction  from two source ports
  //1 content with no hash or keyid restriction

  printf ("TestCCNxStandardPitb6 DoRun\n");

  Ptr<CCNxStandardPit> pit = CreatePit ();

  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is");
  Ptr<CCNxBuffer> payload1  = Create<CCNxBuffer> (64);
  Ptr<CCNxHashValue> keyIdRest1 = Create<CCNxHashValue> (55);  //magic number to match KEYIDHACK
  Ptr<CCNxHashValue> hashValRest1 = Create<CCNxHashValue> (33);
  Ptr<CCNxHashValue> hashValRest2 = Create<CCNxHashValue> (44);


//  Ptr<CCNxName> name0 = Create<CCNxName> ("ccnx:/");;
//  Ptr<CCNxInterest> interestNoNameOrHash = Create<CCNxInterest> (name0);
//  Ptr<CCNxPacket> iPacketNoNameOrHash = CCNxPacket::CreateFromMessage (interestNoNameOrHash);

  Ptr<CCNxInterest> interestNoRestrictions = Create<CCNxInterest> (name1);
  Ptr<CCNxPacket> iPacketNoRestrictions = CCNxPacket::CreateFromMessage (interestNoRestrictions);

  Ptr<CCNxInterest> interestHashKeyRestrictions = Create<CCNxInterest> (name1,payload1,keyIdRest1,hashValRest1);
  Ptr<CCNxPacket> iPacketHashKeyRestrictions = CCNxPacket::CreateFromMessage (interestHashKeyRestrictions);

  Ptr<CCNxContentObject> contentNoRestrictions = Create<CCNxContentObject> (name1);
  Ptr<CCNxPacket> cPacketNoRestrictions = CCNxPacket::CreateFromMessage (contentNoRestrictions);

  Ptr<CCNxPacket> cPacketHashKeyRestrictions = CCNxPacket::CreateFromMessage (contentNoRestrictions);
  cPacketHashKeyRestrictions->SetContentObjectHash (hashValRest1);

  Ptr<CCNxPacket> cPacketNoMatchRestrictions = CCNxPacket::CreateFromMessage (contentNoRestrictions);
  cPacketNoMatchRestrictions->SetContentObjectHash (hashValRest2);

  Ptr<CCNxVirtualConnection> connection1 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection2 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection3 = Create<CCNxVirtualConnection> ();
  Ptr<CCNxVirtualConnection> connection4 = Create<CCNxVirtualConnection> ();

  //* actions start here

//  pit->ReceiveInterest (iPacketNoNameOrHash, connection1); // no way to process, should return error
//  StepSimulatorReceiveInterest();
//  NS_TEST_EXPECT_MSG_EQ (verdict, CCNxPit::Error, "verdict should be Error !");


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacketHashKeyRestrictions, connection1)); // will go into both name and hash maps
  StepSimulatorReceiveInterest ();
  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacketNoRestrictions, connection2)); // will go into only name map, but there is already entry with this name
  StepSimulatorReceiveInterest ();
  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");

  pit->Print (std::cout);

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacketNoMatchRestrictions, connection4)); //matches iPacketNoRestrictions
  StepSimulatorSatisfyInterest ();
  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 1, "wrong number of connections returned!");
  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->front ()->GetConnectionId (), connection2->GetConnectionId (), "wrong connection returned!");


  NS_TEST_EXPECT_MSG_EQ (pit->CountEntries (), 1, "wrong number of table entries!");


  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacketHashKeyRestrictions, connection4)); //matches iPacketHashKeyRestrictions
  StepSimulatorSatisfyInterest ();
  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 1, "wrong number of connections returned!");
  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->front ()->GetConnectionId (), connection1->GetConnectionId (), "wrong connection returned!");

  NS_TEST_EXPECT_MSG_EQ (pit->CountEntries (), 0, "wrong number of table entries!");

  //RemoveDuplicates test

  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacketHashKeyRestrictions, connection1)); // will go into  hash map
  StepSimulatorReceiveInterest ();
  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");


  pit->ReceiveInterest (Create<CCNxForwarderMessage> (iPacketNoRestrictions, connection1)); // will go into only name map
  StepSimulatorReceiveInterest ();
  NS_TEST_EXPECT_MSG_EQ (_receiveInterestCallbackVerdict, CCNxPit::Forward, "verdict should be forward !");

  pit->Print (std::cout);

  NS_TEST_EXPECT_MSG_EQ (pit->CountEntries (), 2, "wrong number of table entries!");

  pit->SatisfyInterest (Create<CCNxForwarderMessage> (cPacketHashKeyRestrictions, connection4)); //matches both maps
  StepSimulatorSatisfyInterest ();

  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->size (), 1, "wrong number of connections returned!");
  NS_TEST_EXPECT_MSG_EQ (_satisfyInterestCallbackConnections->front ()->GetConnectionId (), connection1->GetConnectionId (), "wrong connection returned!");

  NS_TEST_EXPECT_MSG_EQ (pit->CountEntries (), 0, "wrong number of table entries!");


  printf ("TestCCNxStandardPitEntryb6 End\n");

}
EndTest ()

/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for CCNxStandardPit
 */
static class TestSuiteCCNxStandardPit : public TestSuite
{
public:
  TestSuiteCCNxStandardPit () : TestSuite ("ccnx-standard-pit", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new b1 (), TestCase::QUICK);
    AddTestCase (new b1a (), TestCase::QUICK);
    //TODO CCN HASH ONLY PACKETS - HOW TO CREATE?    AddTestCase (new b1b (), TestCase::QUICK);
    AddTestCase (new b2 (), TestCase::QUICK);
    AddTestCase (new b3 (), TestCase::QUICK);
    AddTestCase (new b4 (), TestCase::QUICK);
    AddTestCase (new b5 (), TestCase::QUICK);
    AddTestCase (new b6 (), TestCase::QUICK);

  }
} g_TestSuiteCCNxStandardPit;


} // namespace TestSuiteCCNxStandardPit
