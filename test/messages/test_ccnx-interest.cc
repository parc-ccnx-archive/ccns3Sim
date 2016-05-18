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
#include "ns3/ccnx-interest.h"

#include "ns3/ccnx-hash-value.h"
#include "ns3/ccnx-buffer.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestCCNxInterest {

BeginTest (Constructor_AllParams)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (77);
  Ptr<CCNxHashValue> hash = Create<CCNxHashValue> (234235);
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  bool exists = (interest);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");

  std::cout << *interest << std::endl;
}
EndTest ()

BeginTest (Constructor_ShortParams)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload);
  bool exists = (interest);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");

  std::cout << *interest << std::endl;
}
EndTest ()

BeginTest (Constructor_OneParam)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name);
  bool exists = (interest);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");

  std::cout << *interest << std::endl;
}
EndTest ()

BeginTest (GetName)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload);
  bool result = name->Equals (*interest->GetName ());
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (GetPayload)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload);
  bool result = payload->Equals (interest->GetPayload ());
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (GetKeyIdRestriction)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (77);
  Ptr<CCNxHashValue> hash = Create<CCNxHashValue> (234235);
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  bool result = *keyid == *interest->GetKeyidRestriction ();
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (GetHashRestriction)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (77);
  Ptr<CCNxHashValue> hash = Create<CCNxHashValue> (234235);
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  bool result = *hash == *interest->GetHashRestriction ();
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (HasKeyIdRestriction)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (77);
  Ptr<CCNxHashValue> hash = Create<CCNxHashValue> (234235);
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  bool result = interest->HasKeyidRestriction ();
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (HasNoKeyIdRestriction)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload);
  bool result = interest->HasKeyidRestriction ();
  NS_TEST_EXPECT_MSG_EQ (result, false, "Got wrong value");
}
EndTest ()

BeginTest (HasHashRestriction)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (77);
  Ptr<CCNxHashValue> hash = Create<CCNxHashValue> (234235);
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  bool result = interest->HasHashRestriction ();
  NS_TEST_EXPECT_MSG_EQ (result, true, "Got wrong value");
}
EndTest ()

BeginTest (HasNoHashRestriction)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload);
  bool result = interest->HasHashRestriction ();
  NS_TEST_EXPECT_MSG_EQ (result, false, "Got wrong value");
}
EndTest ()

BeginTest (GetMessageType)
{
  Ptr<CCNxName> name = Create<CCNxName> ("ccnx:/name=foo");
  Ptr<CCNxHashValue> keyid = Create<CCNxHashValue> (77);
  Ptr<CCNxHashValue> hash = Create<CCNxHashValue> (234235);
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload, keyid, hash);
  CCNxMessage::MessageType result = interest->GetMessageType ();
  NS_TEST_EXPECT_MSG_EQ (result, CCNxMessage::Interest, "Got wrong value");
}
EndTest ()

static Ptr<CCNxInterest>
CreateInterest (const char *uri, size_t payloadSize, const char *payload, unsigned keyid, unsigned hash)
{
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (Create<CCNxName> (uri), Create<CCNxBuffer> (payloadSize, payload),
                                                     Create<CCNxHashValue> (keyid), Create<CCNxHashValue> (hash));
  return interest;
}

BeginTest (Equals)
{
  printf ("TestCCNxInterestEquals DoRun\n");

  Ptr<CCNxInterest> a = CreateInterest ("ccnx:/name=foo", 5, "apple", 77, 234235);
  Ptr<CCNxInterest> b = CreateInterest ("ccnx:/name=foo", 5, "apple", 77, 234235);
  Ptr<CCNxInterest> c = CreateInterest ("ccnx:/name=foo", 5, "apple", 77, 234235);

  Ptr<CCNxInterest> w = CreateInterest ("ccnx:/name=bar", 5, "apple", 77, 234235);
  Ptr<CCNxInterest> x = CreateInterest ("ccnx:/name=foo", 6, "apples", 77, 234235);
  Ptr<CCNxInterest> y = CreateInterest ("ccnx:/name=foo", 5, "apple", 88, 234235);
  Ptr<CCNxInterest> z = CreateInterest ("ccnx:/name=foo", 5, "apple", 77, 0);

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
 * Test Suite for CCNxInterest
 */
static class TestSuiteCCNxInterest : public TestSuite
{
public:
  TestSuiteCCNxInterest () : TestSuite ("ccnx-interest", UNIT)
  {
    AddTestCase (new Constructor_AllParams (), TestCase::QUICK);
    AddTestCase (new Constructor_ShortParams (), TestCase::QUICK);
    AddTestCase (new Constructor_OneParam (), TestCase::QUICK);
    AddTestCase (new TestCCNxInterest::GetName (), TestCase::QUICK);
    AddTestCase (new GetPayload (), TestCase::QUICK);
    AddTestCase (new GetKeyIdRestriction (), TestCase::QUICK);
    AddTestCase (new GetHashRestriction (), TestCase::QUICK);
    AddTestCase (new HasKeyIdRestriction (), TestCase::QUICK);
    AddTestCase (new HasNoKeyIdRestriction (), TestCase::QUICK);
    AddTestCase (new HasHashRestriction (), TestCase::QUICK);
    AddTestCase (new HasNoHashRestriction (), TestCase::QUICK);
    AddTestCase (new GetMessageType (), TestCase::QUICK);
    AddTestCase (new Equals (), TestCase::QUICK);
  }
} g_TestSuiteCCNxInterest;

} // namespace TestSuiteCCNxInterest
