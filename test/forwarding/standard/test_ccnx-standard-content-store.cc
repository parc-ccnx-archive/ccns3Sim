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
#include "ns3/ccnx-standard-content-store.h"
#include "ns3/ccnx-forwarder-message.h"
#include "ns3/ccnx-standard-forwarder-work-item.h"
#include "../../mockups/mockup_ccnx-virtual-connection.h"
#include "../../mockups/ccnx-standard-content-store-with-test-methods.h"
#include "ns3/integer.h"
#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxStandardContentStore {



BeginTest (Constructor)
{
  printf ("TestCCNxStandardContentStoreConstructor DoRun\n");
  LogComponentEnable ("CCNxStandardContentStore", LOG_LEVEL_DEBUG);
  LogComponentEnable ("CCNxStandardContentStoreLruList", LOG_LEVEL_DEBUG);
}
EndTest ()

static Time _layerDelay = MicroSeconds (1);

static Ptr<CCNxPacket> _lookupMatchInterestCallbackPacket;
static bool _lookupMatchInterestCallbackFired;

static Ptr<CCNxPacket> _lookupAddContentObjectCallbackPacket;
static bool _lookupAddContentObjectCallbackFired;
/*
 * used by the CS as the callback function for ReceiveInterest.  After we make
 * a call, use Simulator::Run() for a bit, then check what got put here.
 */
static void
MockupMatchInterestCallback (Ptr<CCNxForwarderMessage> message)
{
  Ptr<CCNxStandardForwarderWorkItem> workItem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);

  _lookupMatchInterestCallbackPacket = workItem->GetContentStorePacket ();
  _lookupMatchInterestCallbackFired = true;
}

static void
MockupAddContentObjectCallback (Ptr<CCNxForwarderMessage> message)
{
  Ptr<CCNxStandardForwarderWorkItem> workItem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);

  _lookupAddContentObjectCallbackPacket = workItem->GetContentStorePacket ();
  _lookupAddContentObjectCallbackFired = true;
}

static void
StepSimulatorMatchInterest ()
{
  unsigned tries = 0;
  _lookupMatchInterestCallbackFired = false;
  while (!_lookupMatchInterestCallbackFired && tries < 4)
    {
      Simulator::Stop (_layerDelay);
      Simulator::Run ();
      tries++;
    }

  NS_ASSERT_MSG (_lookupMatchInterestCallbackFired, "Did not get a call to MatchInterestLookupCallback");
}

static void
StepSimulatorAddContentObject ()
{
  unsigned tries = 0;
  _lookupAddContentObjectCallbackFired = false;
  while (!_lookupAddContentObjectCallbackFired && tries < 4)
    {
      Simulator::Stop (_layerDelay);
      Simulator::Run ();
      tries++;
    }

  NS_ASSERT_MSG (_lookupAddContentObjectCallbackFired, "Did not get a call to AddContentObjectLookupCallback");
}


static Ptr<CCNxStandardContentStoreWithTestMethods>
CreateContentStore ()
{
  Ptr<CCNxStandardContentStoreWithTestMethods> contentStore = CreateObject<CCNxStandardContentStoreWithTestMethods> ();
  contentStore->SetMatchInterestCallback (MakeCallback (&MockupMatchInterestCallback));
  contentStore->SetAddContentObjectCallback (MakeCallback (&MockupAddContentObjectCallback));
  contentStore->Initialize ();
  return contentStore;
}


static Ptr<CCNxStandardForwarderWorkItem>
CreateWorkItem (Ptr<CCNxPacket> packet, Ptr<CCNxConnection> ingress)
{
  Ptr<CCNxStandardForwarderWorkItem> item = Create<CCNxStandardForwarderWorkItem> (packet, ingress, Ptr<CCNxConnection> (0));
  return item;
}
typedef struct
{
  Ptr<const CCNxName> prefix1;
  Ptr<const CCNxName> prefix2;
  Ptr<const CCNxName> prefix3;

  Ptr<CCNxVirtualConnection> ingress1;
  Ptr<CCNxVirtualConnection> ingress2;
  Ptr<CCNxVirtualConnection> ingress3;

  Ptr<CCNxVirtualConnection> nextHop1;
  Ptr<CCNxVirtualConnection> nextHop2;
  Ptr<CCNxVirtualConnection> nextHop3;

  Ptr<CCNxConnectionList> eConnList1;

  Ptr<const CCNxName> name1;
  Ptr<const CCNxName> name2;
  Ptr<const CCNxName> name3;

  Ptr<CCNxInterest> interest1;
  Ptr<CCNxPacket> iPacket1;
  Ptr<CCNxForwarderMessage> iForwarderMessage1;
  Ptr<CCNxStandardForwarderWorkItem> iWorkItem1;

  Ptr<CCNxInterest> interest2;
  Ptr<CCNxPacket> iPacket2;
  Ptr<CCNxForwarderMessage> iForwarderMessage2;
  Ptr<CCNxStandardForwarderWorkItem> iWorkItem2;

  Ptr<CCNxInterest> interest3;
  Ptr<CCNxPacket> iPacket3;
  Ptr<CCNxForwarderMessage> iForwarderMessage3;
  Ptr<CCNxStandardForwarderWorkItem> iWorkItem3;

  Ptr<CCNxContentObject> content1;
  Ptr<CCNxPacket> cPacket1;
  Ptr<CCNxForwarderMessage> cForwarderMessage1;
  Ptr<CCNxStandardForwarderWorkItem> cWorkItem1;

  Ptr<CCNxContentObject> content2;
  Ptr<CCNxPacket> cPacket2;
  Ptr<CCNxForwarderMessage> cForwarderMessage2;
  Ptr<CCNxStandardForwarderWorkItem> cWorkItem2;

  Ptr<CCNxContentObject> content3;
  Ptr<CCNxPacket> cPacket3;
  Ptr<CCNxForwarderMessage> cForwarderMessage3;
  Ptr<CCNxStandardForwarderWorkItem> cWorkItem3;

  Ptr<CCNxHashValue> hash1;
  Ptr<CCNxHashValue> hash2;
  Ptr<CCNxHashValue> hash3;


} TestData;

static TestData
CreateTestData ()
{
  TestData data;
  data.prefix1 = Create<CCNxName> ("ccnx:/name=trump/name=is/name=forwarder");
  data.prefix2 = Create<CCNxName> ("ccnx:/name=catfur/name=and/name=mayonnaise");
  data.prefix3 = Create<CCNxName> ("ccnx:/name=my/name=old/name=LPs");

  data.ingress1 = Create<CCNxVirtualConnection> ();
  data.ingress2 = Create<CCNxVirtualConnection> ();
  data.ingress3 = Create<CCNxVirtualConnection> ();

  data.nextHop1 = Create<CCNxVirtualConnection> ();
  data.nextHop2 = Create<CCNxVirtualConnection> ();
  data.nextHop3 = Create<CCNxVirtualConnection> ();

  data.eConnList1 = Create<CCNxConnectionList>(); data.eConnList1->push_back(data.nextHop1);


  //interest and content #1 - content has hash, name, and keyid. interest name matches
  data.name1 = Create<CCNxName> ("ccnx:/name=trump/name=is/name=forwarder/name=paradox");

  data.interest1 = Create<CCNxInterest> (data.name1);
  data.iPacket1 = CCNxPacket::CreateFromMessage (data.interest1);
  data.iForwarderMessage1 = Create<CCNxForwarderMessage> (data.iPacket1,data.ingress1);
  data.iWorkItem1 = CreateWorkItem(data.iPacket1,data.ingress1);

  data.content1 = Create<CCNxContentObject> (data.name1);
  data.cPacket1 = CCNxPacket::CreateFromMessage (data.content1);
  data.hash1 = Create<CCNxHashValue>(1);
  data.cPacket1->SetContentObjectHash(data.hash1);
  data.cForwarderMessage1 = Create<CCNxForwarderMessage> (data.cPacket1,data.ingress1);
  data.cWorkItem1 = CreateWorkItem(data.cPacket1,data.ingress1);

  //interest and content #2 - content has hash, name, and keyid. interest has name and hash matches
  //TODO CCN make interest and content nameless
  data.name2 = Create<CCNxName> ("ccnx:/name=catfur/name=and/name=mayonnaise/name=feels/name=good");

  Ptr<CCNxHashValue> keyidRest2;
  Ptr<CCNxHashValue> hashRest2 = Create<CCNxHashValue> (2); //iPacket2 hashRest matches cPacket2 hash
  Ptr<CCNxBuffer> payload2 = Create<CCNxBuffer> (0);
  data.interest2 = Create<CCNxInterest> (data.name2,payload2,keyidRest2,hashRest2);
  data.iPacket2 = CCNxPacket::CreateFromMessage (data.interest2);
  data.iForwarderMessage2 = Create<CCNxForwarderMessage> (data.iPacket2,data.ingress2);
  data.iWorkItem2 = CreateWorkItem(data.iPacket2,data.ingress2);

  data.content2 = Create<CCNxContentObject> (data.name2);
  data.cPacket2 = CCNxPacket::CreateFromMessage (data.content2);
  data.hash2 = Create<CCNxHashValue>(2);
  data.cPacket2->SetContentObjectHash(data.hash2);
  data.cForwarderMessage2 = Create<CCNxForwarderMessage> (data.cPacket2,data.ingress2);
  data.cWorkItem2 = CreateWorkItem(data.cPacket2,data.ingress2);

  //interest and content #3 - content has hash, name, and keyid. interest has name and keyid matches
  data.name3 = Create<CCNxName> ("ccnx:/name=my/name=old/name=LPs/name=includes/name=CCR/name=yeah");

  Ptr<CCNxHashValue> keyidRest3 = Create<CCNxHashValue> (55); //iPacket3 keyidRest matches cPacket3 keyid (KEYIDHACK)
  Ptr<CCNxHashValue> hashRest3;
  Ptr<CCNxBuffer> payload3 = Create<CCNxBuffer> (0);
  data.interest3 = Create<CCNxInterest> (data.name3,payload3,keyidRest3,hashRest3);
  data.iPacket3 = CCNxPacket::CreateFromMessage (data.interest3);
  data.iForwarderMessage3 = Create<CCNxForwarderMessage> (data.iPacket3,data.ingress3);
  data.iWorkItem3 = CreateWorkItem(data.iPacket3,data.ingress3);

  data.content3 = Create<CCNxContentObject> (data.name3);
  data.cPacket3 = CCNxPacket::CreateFromMessage (data.content3);
  data.hash3 = Create<CCNxHashValue>(3);
  data.cPacket3->SetContentObjectHash(data.hash3);
  data.cForwarderMessage3 = Create<CCNxForwarderMessage> (data.cPacket3,data.ingress3);
  data.cWorkItem3 = CreateWorkItem(data.cPacket3,data.ingress3);

  return data;
}




BeginTest (MatchInterest)
{
  printf ("TestCCNxStandardContentStore_matchInterest DoRun\n");
  //AddContentObject with name,  verify MatchInterest returns same packet
  //TODO CCN add method to allow us to verify entry->GetUseCount
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();

  a->MatchInterest(data.iWorkItem1); StepSimulatorMatchInterest ();

  NS_TEST_EXPECT_MSG_EQ (_lookupMatchInterestCallbackPacket, data.cPacket1, "wrong Packet returned !");


}
EndTest ()

BeginTest (MatchInterestHash)  //TODO CCN try this with nameless object - doesnt test hash if there is a name
{
  printf ("TestCCNxStandardContentStore_matchInterestHash DoRun\n");
  //AddContentObject with name,  verify MatchInterest returns same packet
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem2,data.eConnList1); StepSimulatorAddContentObject();

  a->MatchInterest(data.iWorkItem2); StepSimulatorMatchInterest ();

  NS_TEST_EXPECT_MSG_EQ (_lookupMatchInterestCallbackPacket, data.cPacket2, "wrong Packet returned !");



}
EndTest ()

BeginTest (MatchInterestKeyid)
{
  printf ("TestCCNxStandardContentStore_matchInterestKeyid DoRun\n");
  //* 	AddContentObject with name/keyid,   verify MatchInterest returns same packet,
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem3,data.eConnList1); StepSimulatorAddContentObject();
  a->MatchInterest(data.iWorkItem3); StepSimulatorMatchInterest ();

  NS_TEST_EXPECT_MSG_EQ (_lookupMatchInterestCallbackPacket, data.cPacket3, "wrong Packet returned !");

}
EndTest ()

BeginTest (MatchInterestTwoObjects)
{
  printf ("TestCCNxStandardContentStore_MatchInterestTwoObjects DoRun\n");
  //* 	AddContentObject 1, AddContentObject 2, MatchInterest 2, verify GetObjectCount and GetLru
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();
  a->AddContentObject(data.cWorkItem2,data.eConnList1); StepSimulatorAddContentObject();

  a->MatchInterest(data.iWorkItem2); StepSimulatorMatchInterest ();

  NS_TEST_EXPECT_MSG_EQ (_lookupMatchInterestCallbackPacket, data.cPacket2, "wrong Packet returned !");

  NS_TEST_EXPECT_MSG_EQ(a->GetMapByHashCount(),2,"map by hash size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameCount(),2,"map by name size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameKeyidCount(),2, "Wrong namekeyid map size");
  NS_TEST_EXPECT_MSG_EQ(a->GetObjectCount(),2,"LRU list size wrong");



}
EndTest ()



BeginTest (MatchInterestButExpired)
{
  printf ("TestCCNxStandardContentStore_MatchInterestButExpired DoRun\n");
  // 	AddContentObject with name but expired,  verify MatchInterest returns null packet
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();
  // TODO CCN make content object expire
  a->MatchInterest(data.iWorkItem1); StepSimulatorMatchInterest ();

  NS_TEST_EXPECT_MSG_EQ(_lookupMatchInterestCallbackPacket,Ptr<CCNxPacket>(0),"Null packet should be returned");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByHashCount(),0,"map by hash size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameCount(),0,"map by name size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameKeyidCount(),0, "Wrong namekeyid map size");
  NS_TEST_EXPECT_MSG_EQ(a->GetObjectCount(),0,"LRU list size wrong");

}
EndTest ()


BeginTest (AddContentObject)
{
  printf ("TestCCNxStandardContentStore_AddContentObject DoRun\n");
  //AddContentObject,  verify  Lrulist and Maps
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem1,data.eConnList1);  StepSimulatorAddContentObject();

  NS_TEST_EXPECT_MSG_EQ(a->GetObjectCount(),1,"Lru list length wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByHashCount(),1,"map by hash size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameCount(),1,"map by name size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameKeyidCount(),1, "Wrong namekeyid map size"); //if no KEYIDHACK, chg the exp to 0

}
EndTest ()

BeginTest (AddContentObject2x)
{
  printf ("TestCCNxStandardContentStore_AddContentObject DoRun\n");
  //AddContentObject 2x, and verify no change to Lrulist and Maps
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();
  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();

  NS_TEST_EXPECT_MSG_EQ(a->GetObjectCount(),1,"Lru list length wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByHashCount(),1,"map by hash size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameCount(),1,"map by name size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameKeyidCount(),1, "Wrong namekeyid map size");

}
EndTest ()

BeginTest (AddContentObject_ObjectCapacity)
{
  printf ("TestCCNxStandardContentStore_AddContentObject DoRun\n");
  //SetObjectCapacity=1, AddContentObject 2x and see 1st object deleted
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  //set object capacity
   a->SetAttribute("ObjectCapacity",IntegerValue (1));
   a->Initialize();

  NS_TEST_EXPECT_MSG_EQ(a->GetObjectCapacity(),1,"wrong object capacity");

  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();
  a->AddContentObject(data.cWorkItem2,data.eConnList1); StepSimulatorAddContentObject();

  NS_TEST_EXPECT_MSG_EQ(a->GetObjectCount(),1,"Lru list length wrong");

}
EndTest ()


BeginTest (FindEntryInHashMap)
{
  printf ("TestCCNxStandardContentStore_FindEntryInHashMap DoRun\n");
  //  * 	AddContentObject, verify FindEntryInHashMap same name different hash returns false
  //  * 	AddContentObject, AddContentObject with same hash, verify FindEntryInHashMap returns true

  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();


  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();
//  std::cout <<"data.cForwarderMessage1 stored hash value exp=1 act="<< *data.cForwarderMessage1->GetContentObjectHash().GetValue() << std::endl;
  a->AddContentObject(data.cWorkItem3,data.eConnList1);  StepSimulatorAddContentObject();
//  std::cout <<"data.cForwarderMessage3 stored hash value exp=3 act="<< *data.cForwarderMessage3->GetContentObjectHash().GetValue() << std::endl;


  //compare with new packet - same as content1 but with different hash - should be no match
  Ptr<CCNxName> name1 = Create<CCNxName> ("ccnx:/name=trump/name=is/name=forwarder/name=paradox");
  Ptr<CCNxContentObject> content1 = Create<CCNxContentObject> (name1);
  Ptr<CCNxPacket> cPacket1 = CCNxPacket::CreateFromMessage (content1);
  Ptr<CCNxHashValue> hash2 = Create<CCNxHashValue>(2);
  cPacket1->SetContentObjectHash(hash2);
  std::cout <<"new cPacket1 new hash value exp=2, act="<< *cPacket1->GetContentObjectHash()->GetValue() << std::endl;
  NS_TEST_EXPECT_MSG_EQ(a->FindEntryInHashMap(cPacket1), Ptr<CCNxStandardContentStoreEntry>(0),"packet should not have matched");

  //compare with new packet - same as content2 but with same hash as content3 - should match content3
  Ptr<CCNxName> name2 = Create<CCNxName> ("ccnx:/name=catfur/name=and/name=mayonnaise/name=feels/name=good");
  Ptr<CCNxContentObject> content2 = Create<CCNxContentObject> (name2);
  Ptr<CCNxPacket> cPacket2 = CCNxPacket::CreateFromMessage (content2);
  Ptr<CCNxHashValue> hash3 = Create<CCNxHashValue>(3);
  cPacket2->SetContentObjectHash(hash3);
  std::cout<< "new cPacket2 new hash value exp=3, act="<< *cPacket2->GetContentObjectHash()->GetValue() << std::endl;
  Ptr<CCNxStandardContentStoreEntry> newEntry = a->FindEntryInHashMap(cPacket2);
  NS_TEST_EXPECT_MSG_EQ(newEntry->GetPacket(),data.cPacket3,"packet should  have matched");


}
EndTest ()





BeginTest (DeleteContentObject)
{
  printf ("TestCCNxStandardContentStore_DeleteContentObject DoRun\n");

//  * 	AddContentObject with hash/keyid/name, verify DeleteContentObject returns true, verify lrulist and maps=0,0,0
//  * 	AddContentObject, verify DeleteContentObject of different object returns false, verify lrulist and maps

  TestData data = CreateTestData ();
  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();
  NS_TEST_EXPECT_MSG_EQ(a->DeleteContentObject(data.cPacket1),true,"could not remove content object");
  NS_TEST_EXPECT_MSG_EQ(a->GetObjectCount(),0,"Lru list length wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByHashCount(),0,"map by hash size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameCount(),0,"map by name size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameKeyidCount(),0, "map by namekeyid size wrong");

  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();
  NS_TEST_EXPECT_MSG_EQ(a->DeleteContentObject(data.cPacket2),false,"could remove content object");
  NS_TEST_EXPECT_MSG_EQ(a->GetObjectCount(),1,"Lru list length wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByHashCount(),1,"map by hash size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameCount(),1,"map by name size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameKeyidCount(),1, "map by namekeyid size wrong");


}
EndTest ()


BeginTest (AddMapEntry)
{
  printf ("TestCCNxStandardContentStore_AddMapEntry DoRun\n");

  //  * TODO CCN	AddMapEntry with cPacket hash only, verify maps
  //  * TODO CCN	AddMapEntry with cPacket hash + name, verify maps



  //AddMapEntry with cPacket hash + name + keyid, verify maps
  TestData data = CreateTestData ();
  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();
  Ptr<CCNxStandardContentStoreEntry> newEntry = Create<CCNxStandardContentStoreEntry> (data.cPacket1);

  NS_TEST_EXPECT_MSG_EQ(a->AddMapEntry(data.cPacket1,newEntry),true,"could not add map entry");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByHashCount(),1,"map by hash size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameCount(),1,"map by name size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameKeyidCount(),1, "map by namekeyid size wrong");

}
EndTest ()






BeginTest (IsEntryValid)
{
  printf ("TestCCNxStandardContentStore_IsEntryValid DoRun\n");

  //  *  verify IsEntryValid returns true

  TestData data = CreateTestData ();
  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();
  Ptr<CCNxStandardContentStoreEntry> newEntry = Create<CCNxStandardContentStoreEntry> (data.cPacket1);
  a->AddContentObject(data.cWorkItem1,data.eConnList1);  StepSimulatorAddContentObject();

  NS_TEST_EXPECT_MSG_EQ(a->IsEntryValid(a->FindEntryInHashMap(data.cPacket1)),true,"entry should be valid");

  // TODO CCN wait until IsStale, verify IsEntryValid returns false
  // TODO CCN wait until IsExpired, verify IsEntryValid returns false

}
EndTest ()


BeginTest (GetMapCounts)
{
  printf ("TestCCNxStandardContentStore_GetMapCounts DoRun\n");
  //AddContentObject with name, verify GetMapCounts = 1,1,0
  TestData data = CreateTestData ();

  Ptr<CCNxStandardContentStoreWithTestMethods> a = CreateContentStore ();

  a->AddContentObject(data.cWorkItem1,data.eConnList1); StepSimulatorAddContentObject();

  NS_TEST_EXPECT_MSG_EQ(a->GetMapByHashCount(),1,"map by hash size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameCount(),1,"map by name size wrong");
  NS_TEST_EXPECT_MSG_EQ(a->GetMapByNameKeyidCount(),1, "Wrong namekeyid map size"); //if no KEYIDHACK, chg the exp to 0
;

  //TODO CCN test add of nameless objects and objects without Keyid when possible

}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * \brief Test Suite for CCNxStandardContentStore
 */
static class TestSuiteCCNxStandardContentStore : public TestSuite
{
public:
  TestSuiteCCNxStandardContentStore () : TestSuite ("ccnx-standard-content-store", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new MatchInterest (), TestCase::QUICK);
    AddTestCase (new MatchInterestHash (), TestCase::QUICK);
    AddTestCase (new MatchInterestKeyid (), TestCase::QUICK);

    AddTestCase (new AddContentObject (), TestCase::QUICK);
    AddTestCase (new AddContentObject_ObjectCapacity (), TestCase::QUICK);
    AddTestCase (new GetMapCounts (), TestCase::QUICK);
    AddTestCase (new MatchInterestTwoObjects (), TestCase::QUICK);
    AddTestCase (new AddContentObject2x (), TestCase::QUICK);
    AddTestCase (new FindEntryInHashMap (), TestCase::QUICK);
    AddTestCase (new DeleteContentObject (), TestCase::QUICK);
    AddTestCase (new AddMapEntry (), TestCase::QUICK);
    // TODO CCN FINISH THIS AddTestCase (new MatchInterestButExpired (), TestCase::QUICK);
    AddTestCase (new IsEntryValid (), TestCase::QUICK);


  }
} g_TestSuiteCCNxStandardContentStore;


} // namespace TestSuiteCCNxStandardContentStore
