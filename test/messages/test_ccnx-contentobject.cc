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
#include "ns3/ccnx-contentobject.h"

#include "ns3/ccnx-hash-value.h"
#include "ns3/ccnx-buffer.h"
#include "ns3/ccnx-time.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestCCNxContentObject {

BeginTest (Constructor_AllParams)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
  Ptr<CCNxTime> expiryTime = Create<CCNxTime> (3600);
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
  bool exists = (content);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");
  std::cout << *content << std::endl;
}
EndTest ()

BeginTest (Constructor_ShortParams)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload);
  bool exists = (content);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");
}
EndTest ()

BeginTest (Constructor_OneParam)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name);
  bool exists = (content);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");
}
EndTest ()


BeginTest (GetName)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload);
  bool result = name->Equals (*content->GetName ());
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (GetPayload)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload);
  bool result = payload->Equals (content->GetPayload ());
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (GetPayloadType)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
  Ptr<CCNxTime> expiryTime = Create<CCNxTime> (3600);
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
  bool result = payloadType == content->GetPayloadType ();
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (GetExpiryTime)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
  Ptr<CCNxTime> expiryTime = Create<CCNxTime> (3600);
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
  bool result = expiryTime == content->GetExpiryTime ();
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (GetMessageType)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
  Ptr<CCNxTime> expiryTime = Create<CCNxTime> (3600);
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
  CCNxMessage::MessageType result = content->GetMessageType ();
  NS_TEST_EXPECT_MSG_EQ (result, CCNxMessage::ContentObject, "Got wrong value");
}
EndTest ()

static Ptr<CCNxContentObject>
CreateContentObject (const char *uri, size_t payloadSize, const char *payload,
                     CCNxContentObjectPayloadType payloadType, uint64_t expiryTime)
{
  Ptr<CCNxContentObject> content = Create<CCNxContentObject> (Create<CCNxName> (uri), Create<CCNxBuffer> (payloadSize, payload),
                                                              payloadType, Create<CCNxTime> (expiryTime));
  return content;
}

BeginTest (Equals)
{
  printf ("TestCCNxContentObjectEquals DoRun\n");

  Ptr<CCNxContentObject> a = CreateContentObject ("ccnx:/name=foo", 5, "apple", CCNxContentObjectPayloadType_Data, 3600);
  Ptr<CCNxContentObject> b = CreateContentObject ("ccnx:/name=foo", 5, "apple", CCNxContentObjectPayloadType_Data, 3600);
  Ptr<CCNxContentObject> c = CreateContentObject ("ccnx:/name=foo", 5, "apple", CCNxContentObjectPayloadType_Data, 3600);

  Ptr<CCNxContentObject> w = CreateContentObject ("ccnx:/name=bar", 5, "apple", CCNxContentObjectPayloadType_Data, 3600);
  Ptr<CCNxContentObject> x = CreateContentObject ("ccnx:/name=foo", 6, "apples", CCNxContentObjectPayloadType_Data, 3600);
  Ptr<CCNxContentObject> y = CreateContentObject ("ccnx:/name=foo", 5, "apple", CCNxContentObjectPayloadType_Key, 3600);
  Ptr<CCNxContentObject> z = CreateContentObject ("ccnx:/name=foo", 5, "apple", CCNxContentObjectPayloadType_Data, 0);

  // transitivity of equals
  NS_TEST_EXPECT_MSG_EQ (a->Equals (b), true, "not equal");
  NS_TEST_EXPECT_MSG_EQ (b->Equals (c), true, "not equal");
  NS_TEST_EXPECT_MSG_EQ (c->Equals (a), true, "not equal");

  // and the four counter cases
  NS_TEST_EXPECT_MSG_EQ (a->Equals (w), false, "not equal");
  NS_TEST_EXPECT_MSG_EQ (a->Equals (x), false, "not equal");
  NS_TEST_EXPECT_MSG_EQ (a->Equals (y), false, "not equal");
  NS_TEST_EXPECT_MSG_EQ (a->Equals (z), false, "not equal");

  // Negative test
  NS_TEST_EXPECT_MSG_EQ (a->Equals (0), false, "null value");
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxContentObject
 */
static class TestSuiteCCNxContentObject : public TestSuite
{
public:
  TestSuiteCCNxContentObject () : TestSuite ("ccnx-contentobject", UNIT)
  {
    AddTestCase (new Constructor_AllParams (), TestCase::QUICK);
    AddTestCase (new Constructor_ShortParams (), TestCase::QUICK);
    AddTestCase (new Constructor_OneParam (), TestCase::QUICK);
    AddTestCase (new TestCCNxContentObject::GetName (), TestCase::QUICK);
    AddTestCase (new GetPayload (), TestCase::QUICK);
    AddTestCase (new GetPayloadType (), TestCase::QUICK);
    AddTestCase (new GetExpiryTime (), TestCase::QUICK);
    AddTestCase (new GetMessageType (), TestCase::QUICK);
    AddTestCase (new Equals (), TestCase::QUICK);
  }
} g_TestSuiteContentObject;

} // namespace TestCCNxContentObject
