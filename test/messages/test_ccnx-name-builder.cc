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

namespace TestSuiteCCNxNameBuilder {

BeginTest (Constructor_Empty)
{
  CCNxNameBuilder builder;
  NS_TEST_EXPECT_MSG_EQ (builder.empty (), true, "Empty constructor did not return an empty object");
}
EndTest ()

bool NameSegmentEqual (Ptr<const CCNxNameSegment> a, Ptr<const CCNxNameSegment> b)
{
  return a->Equals (*b);
}

BeginTest (Constructor_Uri)
{
  std::string s ("ccnx:/name=foo/name=bar");
  CCNxNameBuilder builder (s);

  NS_TEST_EXPECT_MSG_EQ (builder.size (), 2, "Name builder URI did not return right object size");

  std::vector< Ptr<const CCNxNameSegment> > truth;
  truth.push_back (Create<const CCNxNameSegment> ("name=foo"));
  truth.push_back (Create<const CCNxNameSegment> ("name=bar"));

  bool isEqual = std::equal (truth.begin (), truth.end (), builder.begin (), NameSegmentEqual);
  NS_TEST_EXPECT_MSG_EQ (isEqual, true, "Name builder URI did not return right object");
}
EndTest ()

BeginTest (Constructor_CCNxName)
{
  std::string s ("ccnx:/name=foo/name=bar");
  CCNxNameBuilder truth (s);
  Ptr<const CCNxName> name = Create<const CCNxName> (s);

  CCNxNameBuilder test (*name);

  NS_TEST_EXPECT_MSG_EQ (test.size (), truth.size (), "Name builder URI did not return right object size");
  for (int i = 0; i < truth.size (); ++i)
    {
      NS_TEST_EXPECT_MSG_EQ (truth[i]->Equals (*test[i]), true, "Name component does not match: " << i);
    }

}
EndTest ()

BeginTest (Constructor_Copy)
{
  std::string s ("ccnx:/name=foo/name=bar/name=car");
  CCNxNameBuilder truth (s);
  CCNxNameBuilder test (truth);

  NS_TEST_EXPECT_MSG_EQ (test.size (), truth.size (), "Name builder URI did not return right object size");
  for (int i = 0; i < truth.size (); ++i)
    {
      NS_TEST_EXPECT_MSG_EQ (truth[i]->Equals (*test[i]), true, "Name component does not match: " << i);
    }
}
EndTest ()

BeginTest (CreateName)
{
  std::string s ("ccnx:/name=foo/name=bar");
  CCNxName n (s);

  CCNxNameBuilder builder (s);
  Ptr<const CCNxName> name = builder.CreateName ();
  NS_TEST_EXPECT_MSG_EQ (name->Equals (n), true, "Names are not same");
}
EndTest ()

BeginTest (Append)
{
  std::string s ("ccnx:/name=foo/name=bar/name=car");
  CCNxNameBuilder truth (s);
  CCNxNameBuilder test;
  test.Append (Create<const CCNxNameSegment> ("name=foo"));
  test.Append (Create<const CCNxNameSegment> ("name=bar"));
  test.Append (Create<const CCNxNameSegment> ("name=car"));
  NS_TEST_EXPECT_MSG_EQ (test.size (), truth.size (), "Name builder URI did not return right object size");
  for (int i = 0; i < truth.size (); ++i)
    {
      NS_TEST_EXPECT_MSG_EQ (truth[i]->Equals (*test[i]), true, "Name component does not match: " << i);
    }
}
EndTest ()

BeginTest (Trim_IsEmpty)
{
  CCNxNameBuilder test;
  test.Trim ();
  NS_TEST_EXPECT_MSG_EQ (test.size (), 0, "Trim() on empty CCNxNameBuilder failed");
}
EndTest ()

BeginTest (Trim_IsNotEmpty)
{
  std::string s1 ("ccnx:/name=foo/name=bar");
  CCNxNameBuilder truth (s1);
  std::string s2 ("ccnx:/name=foo/name=bar/name=car");
  CCNxNameBuilder test (s2);
  test.Trim ();

  NS_TEST_EXPECT_MSG_EQ ((test == truth), true, "Trimed builder does not equal right thing");
}
EndTest ()

BeginTest (Iterator_IsEmpty)
{
  CCNxNameBuilder builder;
  bool isEnd = builder.cbegin () == builder.cend ();

  NS_TEST_EXPECT_MSG_EQ (isEnd, true, "An empty builder should have begin == end");
}
EndTest ()

BeginTest (Iterator_IsNotEmpty)
{
  CCNxNameBuilder builder ("ccnx:/name=foo/name=bar");

  std::vector< Ptr<const CCNxNameSegment> > truth;
  truth.push_back (Create<const CCNxNameSegment> ("name=foo"));
  truth.push_back (Create<const CCNxNameSegment> ("name=bar"));

  bool isEqual = std::equal (truth.begin (), truth.end (), builder.begin (), NameSegmentEqual);
  NS_TEST_EXPECT_MSG_EQ (isEqual, true, "Name builder URI did not return right object");
}
EndTest ()

BeginTest (clear)
{
  CCNxNameBuilder builder ("ccnx:/name=foo/name=bar");
  builder.clear ();
  NS_TEST_EXPECT_MSG_EQ (builder.size (), 0, "Name builder size should be 0 after clear");
}
EndTest ()

BeginTest (PointerOperator)
{
  CCNxNameBuilder builder ("ccnx:/name=foo/name=bar");

  CCNxNameBuilder::const_iterator i = builder.begin ();
  const std::string s = i->GetValue ();
  NS_TEST_EXPECT_MSG_EQ (s, "foo", "First name segment should be 'foo'");
}
EndTest ()

BeginTest (IteratorCopy)
{
  CCNxNameBuilder builder ("ccnx:/name=foo/name=bar");

  CCNxNameBuilder::const_iterator i = builder.begin ();
  CCNxNameBuilder::const_iterator j = i;

  NS_TEST_EXPECT_MSG_EQ (j->GetValue (), "foo", "First name segment should be 'foo'");
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxNameBuilder
 */
static class TestSuiteCCNxNameBuilder : public TestSuite
{
public:
  TestSuiteCCNxNameBuilder () : TestSuite ("ccnx-name-builder", UNIT)
  {
    AddTestCase (new Constructor_Empty (), TestCase::QUICK);
    AddTestCase (new Constructor_Uri (), TestCase::QUICK);
    AddTestCase (new Constructor_CCNxName (), TestCase::QUICK);
    AddTestCase (new Constructor_Copy (), TestCase::QUICK);
    AddTestCase (new CreateName (), TestCase::QUICK);

    AddTestCase (new Append (), TestCase::QUICK);
    AddTestCase (new Trim_IsEmpty (), TestCase::QUICK);
    AddTestCase (new Trim_IsNotEmpty (), TestCase::QUICK);
    AddTestCase (new Iterator_IsEmpty (), TestCase::QUICK);
    AddTestCase (new Iterator_IsNotEmpty (), TestCase::QUICK);
    AddTestCase (new clear (), TestCase::QUICK);
    AddTestCase (new IteratorCopy (), TestCase::QUICK);
    AddTestCase (new PointerOperator (), TestCase::QUICK);
  }
} g_TestSuiteCCNxNameBuilder;

} // namespace TestSuiteCCNxName



