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
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-name-builder.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxName {

BeginTest (Constructor)
{
  std::string s ("ccnx:/NAME=foor/VER=bar/CHUNK=333");
  CCNxName n1 (s);
  CCNxName n2 (n1);
  NS_TEST_EXPECT_MSG_EQ (n1.Equals (n2), true, "not equal");
}
EndTest ()

BeginTest (Equals)
{
  CCNxNameBuilder builder;
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Name, "foor"));
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Version, "bar"));
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Chunk, "333"));
  Ptr<const CCNxName> a = builder.CreateName ();

  builder.clear ();
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Name, "foor"));
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Version, "bar"));
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Chunk, "333"));
  Ptr<const CCNxName> b = builder.CreateName ();

  NS_TEST_EXPECT_MSG_EQ (a->Equals (*b), true, "not equal");

  builder.clear ();
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Name, "foor"));
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Version, "bar"));
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Chunk, "444"));
  Ptr<const CCNxName> x = builder.CreateName ();

  NS_TEST_EXPECT_MSG_NE (a->Equals (*x), true, "equal");
}
EndTest ()

BeginTest (Parse_1)
{
  std::string s ("ccnx:/NAME=foor/VER=bar/CHUNK=333");

  CCNxName n (s);

  // now construct the truth
  CCNxNameBuilder builder;
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Name, "foor"));
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Version, "bar"));
  builder.Append (Create<const CCNxNameSegment> (CCNxNameSegment_Chunk, "333"));
  Ptr<const CCNxName> truth = builder.CreateName ();
  NS_TEST_EXPECT_MSG_EQ (truth->Equals (n), true, "Parsed name not equal to truth");
}
EndTest ()

BeginTest (IsPrefixOf)
{
  struct vectors
  {
    const char *uriA;
    const char *uriB;
    bool isAPrefixOfB;
  } testVectors[] = {
    // FIXME: default route notations is not supported yet
    // { "ccnx:/", "ccnx:/", true },
    // { "ccnx:/", "ccnx:/name=apple", true },
    // { "ccnx:/name=apple", "ccnx:/", false },
    { "ccnx:/name=pear", "ccnx:/name=pear", true },
    { "ccnx:/name=pear", "ccnx:/name=pear/name=tart", true },
    { "ccnx:/name=pear/name=tart", "ccnx:/name=pear/name=tart/ver=sugar", true },
    { "ccnx:/name=pear/name=tart", "ccnx:/name=pear", false },
    { "ccnx:/name=pear", "ccnx:/name=peary/name=tart", false },
    { NULL, NULL, false },
  };

  for (int i = 0; testVectors[i].uriA != NULL; ++i)
    {
      Ptr<const CCNxName> a = Create<CCNxName> (testVectors[i].uriA);
      Ptr<const CCNxName> b = Create<CCNxName> (testVectors[i].uriB);
      bool test = a->IsPrefixOf (*b);
      NS_TEST_EXPECT_MSG_EQ (test, testVectors[i].isAPrefixOfB, "Wrong result vector " << i);
    }
}
EndTest ()

BeginTest (LessThan)
{
  struct vectors
  {
    const char *uriA;
    const char *uriB;
    bool lessThan;
  } testVectors[] = {
    { "ccnx:/NAME=foor/VER=bar", "ccnx:/NAME=foor/VER=bar/CHUNK=333", true },
    { "ccnx:/NAME=foor/VER=bar/CHUNK=333", "ccnx:/NAME=foor/VER=bar", false },
    { "ccnx:/NAME=foor/VER=bar/CHUNK=333", "ccnx:/NAME=foor/VER=bar/CHUNK=334", true },
    { "ccnx:/NAME=foor/VER=bar/CHUNK=334", "ccnx:/NAME=foor/VER=bar/CHUNK=333", false },
    { NULL, NULL, false },
  };

  for (int i = 0; testVectors[i].uriA != NULL; ++i)
    {
      Ptr<const CCNxName> a = Create<CCNxName> (testVectors[i].uriA);
      Ptr<const CCNxName> b = Create<CCNxName> (testVectors[i].uriB);
      bool test = *a < *b;
      NS_TEST_EXPECT_MSG_EQ (test, testVectors[i].lessThan, "Wrong result vector " << i);
    }
}
EndTest ()

BeginTest (PrintName)
{
  std::string s ("ccnx:/NAME=foor/VER=bar");
  CCNxName n (s);

  std::cout << n << std::endl;
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxName
 */
static class TestSuiteCCNxName : public TestSuite
{
public:
  TestSuiteCCNxName () : TestSuite ("ccnx-name", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new Equals (), TestCase::QUICK);
    AddTestCase (new Parse_1 (), TestCase::QUICK);
    AddTestCase (new IsPrefixOf (), TestCase::QUICK);
    AddTestCase (new LessThan (), TestCase::QUICK);
    AddTestCase (new PrintName (), TestCase::QUICK);
  }
} g_TestSuiteCCNxName;

} // namespace TestSuiteCCNxName
