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
#include "ns3/ccnx-codec-contentobject.h"
#include "ns3/ccnx-schema-v1.h"
#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

// =================================

static const uint8_t payloadBytes[] = "an apple a day";

static Ptr<CCNxBuffer>
CreatePayload (size_t length, const uint8_t *bytes)
{
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (0);
  payload->AddAtStart (length);
  payload->Begin ().Write ((uint8_t *) bytes, length);
  return payload;
}

namespace TestCCNxCodecContentObject {

BeginTest (TestGetSerializedSize)
{
  printf ("TestGetSerializedSize DoRun\n");
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=apple/ver=pie/chunk=crust");

  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
  Ptr<CCNxTime> expiryTime  = Create<CCNxTime> (3600);
  Ptr<CCNxBuffer> payload = CreatePayload (sizeof(payloadBytes), payloadBytes);

  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
  CCNxCodecContentObject ci;
  ci.SetHeader (content);

  // expected size = 4 + 29 (name) + 4 (expiryTime TLV) + 8 + 4 (payload TLV) + payloadBytes
  size_t expectedSize = 4 + 29 + 4 + 8 + 4 + sizeof(payloadBytes);

  size_t test = ci.GetSerializedSize ();
  NS_TEST_EXPECT_MSG_EQ (test, expectedSize, "wrong size");
}
EndTest ()

BeginTest (TestSerializeWithDataPayloadType)
{
  printf ("TestSerializeWithDataPayloadType DoRun\n");
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=apple/ver=pie/chunk=crust");

  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
  Ptr<CCNxTime> expiryTime  = Create<CCNxTime> (3600); // hex equivalent E10
  Ptr<CCNxBuffer> payload = CreatePayload (sizeof(payloadBytes), payloadBytes);

  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
  CCNxCodecContentObject ci;
  ci.SetHeader (content);

  Buffer buffer (0);
  buffer.AddAtStart (ci.GetSerializedSize ());
  ci.Serialize (buffer.Begin ());

  NS_TEST_EXPECT_MSG_EQ (buffer.GetSize (), ci.GetSerializedSize (), "Wrong size");

  const uint8_t truth[] = {
    // T_CONTENTOBJECT
    0,  2, 0, 60,
    // T_NAME
    0,  0, 0, 25,
    0,  1, 0,  5, 'a', 'p', 'p', 'l', 'e',
    0, 19, 0,  3, 'p', 'i', 'e',
    0, 16, 0,  5, 'c', 'r', 'u', 's', 't',
    // T_EXPIRY
    0,  6, 0, 8,
    0, 0, 0, 0, 0, 0, 0xE, 0x10,
    // T_PAYLOAD
    0,  1, 0, 15,
    'a', 'n', ' ', 'a', 'p', 'p', 'l', 'e', ' ', 'a', ' ', 'd', 'a', 'y', 0
  };

  uint8_t test[sizeof(truth)];
  buffer.CopyData (test, sizeof(truth));

  hexdump ("truth", sizeof(truth), truth);
  hexdump ("test ", sizeof(test), test);

  NS_TEST_EXPECT_MSG_EQ (memcmp (truth, test, sizeof(truth)), 0, "Data in buffer wrong");
}
EndTest ()

BeginTest (TestSerializeWithNonDataPayloadType)
{
  printf ("TestSerializeWithNonDataPayloadType DoRun\n");
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=apple/ver=pie/chunk=crust");

  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Key;
  Ptr<CCNxTime> expiryTime  = Create<CCNxTime> (3600); // hex equivalent E10
  Ptr<CCNxBuffer> payload = CreatePayload (sizeof(payloadBytes), payloadBytes);

  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
  CCNxCodecContentObject ci;
  ci.SetHeader (content);

  Buffer buffer (0);
  buffer.AddAtStart (ci.GetSerializedSize ());
  ci.Serialize (buffer.Begin ());

  NS_TEST_EXPECT_MSG_EQ (buffer.GetSize (), ci.GetSerializedSize (), "Wrong size");

  const uint8_t truth[] = {
    // T_CONTENTOBJECT
    0,  2, 0, 65,
    // T_NAME
    0,  0, 0, 25,
    0,  1, 0,  5, 'a', 'p', 'p', 'l', 'e',
    0, 19, 0,  3, 'p', 'i', 'e',
    0, 16, 0,  5, 'c', 'r', 'u', 's', 't',
    // T_PAYLDTYPE
    0,  5, 0, 1,
    1,
    // T_EXPIRY
    0,  6, 0, 8,
    0, 0, 0, 0, 0, 0, 0xE, 0x10,
    // T_PAYLOAD
    0,  1, 0, 15,
    'a', 'n', ' ', 'a', 'p', 'p', 'l', 'e', ' ', 'a', ' ', 'd', 'a', 'y', 0
  };

  uint8_t test[sizeof(truth)];
  buffer.CopyData (test, sizeof(truth));

  hexdump ("truth", sizeof(truth), truth);
  hexdump ("test ", sizeof(test), test);

  NS_TEST_EXPECT_MSG_EQ (memcmp (truth, test, sizeof(truth)), 0, "Data in buffer wrong");
}
EndTest ()

BeginTest (TestDeserializeWithDataPayloadType)
{
  printf ("TestDeserializeWithDataPayloadType DoRun\n");

  // serialize it into a buffer
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=apple/ver=pie/chunk=crust");

  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
  Ptr<CCNxTime> expiryTime  = Create<CCNxTime> (3600); // hex equivalent E10
  Ptr<CCNxBuffer> payload = CreatePayload (sizeof(payloadBytes), payloadBytes);

  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
  CCNxCodecContentObject ci;
  ci.SetHeader (content);

  Buffer buffer (0);
  buffer.AddAtStart (ci.GetSerializedSize ());
  ci.Serialize (buffer.Begin ());

  // now de-serialize and compare
  CCNxCodecContentObject citest;
  citest.Deserialize (buffer.Begin ());

  Ptr<CCNxContentObject> test = citest.GetHeader ();
  bool equal = content->Equals (test);

  NS_TEST_EXPECT_MSG_EQ (equal, true, "Data in buffer wrong");
}
EndTest ()

BeginTest (TestDeserializeWithNonDataPayloadType)
{
  printf ("TestDeserializeWithNonDataPayloadType DoRun\n");

  // serialize it into a buffer
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=apple/ver=pie/chunk=crust");

  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Link;
  Ptr<CCNxTime> expiryTime  = Create<CCNxTime> (3600); // hex equivalent E10
  Ptr<CCNxBuffer> payload = CreatePayload (sizeof(payloadBytes), payloadBytes);

  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);

  CCNxCodecContentObject ci;
  ci.SetHeader (content);

  Buffer buffer (0);
  buffer.AddAtStart (ci.GetSerializedSize ());
  ci.Serialize (buffer.Begin ());

  // now de-serialize and compare
  CCNxCodecContentObject citest;
  citest.Deserialize (buffer.Begin ());

  Ptr<CCNxContentObject> test = citest.GetHeader ();

  bool equal = content->Equals (test);

  NS_TEST_EXPECT_MSG_EQ (equal, true, "Data in buffer wrong");
}
EndTest ()

BeginTest (TestSerialize_PayloadType)
{
  printf ("TestSerialize_PayloadType DoRun\n");

  uint8_t schemaValue1 = CCNxCodecContentObject::SerializePayloadTypeToSchemaValue (CCNxContentObjectPayloadType_Data);
  NS_TEST_EXPECT_MSG_EQ (schemaValue1, CCNxSchemaV1::T_PAYLOADTYPE_DATA, "wrong parse for CCNxContentObjectPayloadType_Data");

  uint8_t schemaValue2 = CCNxCodecContentObject::SerializePayloadTypeToSchemaValue (CCNxContentObjectPayloadType_Key);
  NS_TEST_EXPECT_MSG_EQ (schemaValue2, CCNxSchemaV1::T_PAYLOADTYPE_KEY, "wrong parse for CCNxContentObjectPayloadType_Key");

  uint8_t schemaValue3 = CCNxCodecContentObject::SerializePayloadTypeToSchemaValue (CCNxContentObjectPayloadType_Link);
  NS_TEST_EXPECT_MSG_EQ (schemaValue3, CCNxSchemaV1::T_PAYLOADTYPE_LINK, "wrong parse for CCNxContentObjectPayloadType_Link");

  uint8_t schemaValue4 = CCNxCodecContentObject::SerializePayloadTypeToSchemaValue (CCNxContentObjectPayloadType_Manifest);
  NS_TEST_EXPECT_MSG_EQ (schemaValue4, CCNxSchemaV1::T_PAYLOADTYPE_MANIFEST, "wrong parse for CCNxContentObjectPayloadType_Manifest");
}
EndTest ()

BeginTest (TestDeserialize_PayloadType)
{
  printf ("TestDeserialize_PayloadType DoRun\n");

  CCNxContentObjectPayloadType payloadType1 = CCNxCodecContentObject::DeserializeSchemaValueToPayloadType (CCNxSchemaV1::T_PAYLOADTYPE_DATA);
  NS_TEST_EXPECT_MSG_EQ (payloadType1, CCNxContentObjectPayloadType_Data, "wrong parse for T_PAYLOADTYPE_DATA");

  CCNxContentObjectPayloadType payloadType2 = CCNxCodecContentObject::DeserializeSchemaValueToPayloadType (CCNxSchemaV1::T_PAYLOADTYPE_KEY);
  NS_TEST_EXPECT_MSG_EQ (payloadType2, CCNxContentObjectPayloadType_Key, "wrong parse for T_PAYLOADTYPE_KEY");

  CCNxContentObjectPayloadType payloadType3 = CCNxCodecContentObject::DeserializeSchemaValueToPayloadType (CCNxSchemaV1::T_PAYLOADTYPE_LINK);
  NS_TEST_EXPECT_MSG_EQ (payloadType3, CCNxContentObjectPayloadType_Link, "wrong parse for T_PAYLOADTYPE_LINK");

  CCNxContentObjectPayloadType payloadType4 = CCNxCodecContentObject::DeserializeSchemaValueToPayloadType (CCNxSchemaV1::T_PAYLOADTYPE_MANIFEST);
  NS_TEST_EXPECT_MSG_EQ (payloadType4, CCNxContentObjectPayloadType_Manifest, "wrong parse for T_PAYLOADTYPE_MANIFEST");

}
EndTest ()
}

// =================================
// Finally, define the TestSuite

/**
 * #ingroup ccnx-test
 *
 * Test Suite for CCNxCodecContentObject
 */
static class TestSuiteCCNxCodecContentObject : public TestSuite
{
public:
  TestSuiteCCNxCodecContentObject () : TestSuite ("ccnx-codec-contentobject", UNIT)
  {
    AddTestCase (new TestCCNxCodecContentObject::TestGetSerializedSize (), TestCase::QUICK);
    AddTestCase (new TestCCNxCodecContentObject::TestSerializeWithDataPayloadType (), TestCase::QUICK);
    AddTestCase (new TestCCNxCodecContentObject::TestSerializeWithNonDataPayloadType (), TestCase::QUICK);
    AddTestCase (new TestCCNxCodecContentObject::TestDeserializeWithDataPayloadType (), TestCase::QUICK);
    AddTestCase (new TestCCNxCodecContentObject::TestDeserializeWithNonDataPayloadType (), TestCase::QUICK);
    AddTestCase (new TestCCNxCodecContentObject::TestSerialize_PayloadType (), TestCase::QUICK);
    AddTestCase (new TestCCNxCodecContentObject::TestDeserialize_PayloadType (), TestCase::QUICK);
  }
} g_TestSuiteCCNxCodecContentObject;

