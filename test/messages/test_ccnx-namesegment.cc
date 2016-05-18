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

#include <iomanip>
#include <sstream>

#include "ns3/log.h"
#include "ns3/test.h"

#include "ns3/ccnx-namesegment.h"
#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxNameSegment {

BeginTest (Constructor)
{
  std::string s ("hello");
  Ptr<CCNxNameSegment> seg = Create<CCNxNameSegment> (CCNxNameSegment_Name, s);

  NS_TEST_EXPECT_MSG_NE (seg, NULL, "Constructor returned NULL");
  NS_TEST_EXPECT_MSG_EQ (s, seg->GetValue (), "Value wrong");
}
EndTest ()

BeginTest (Constructor_Binary)
{
  uint8_t data[] = { 0, 1, 2, 3, 0 };

  std::string s ((const char *) data, sizeof(data));
  NS_TEST_EXPECT_MSG_EQ (s.size (), sizeof(data), "String wrong size");

  Ptr<CCNxNameSegment> seg = Create<CCNxNameSegment> (CCNxNameSegment_Name, s);

  std::stringstream ss;

  ss << seg->GetValue ();
  std::string test = ss.str ();
  NS_TEST_EXPECT_MSG_EQ (test.size (), sizeof(data), "String wrong size");
  for (int i = 0; i < sizeof(data); i++)
    {
      char c = test[i];
      NS_TEST_EXPECT_MSG_EQ (data[i], c, "Character wrong");
    }
}
EndTest ()

BeginTest (Operator_Output)
{
  uint8_t data[] = { 0, 1, 2, 3, 0 };
  std::string expected ("NAME=0x0001020300");

  std::string s ((const char *) data, sizeof(data));
  NS_TEST_EXPECT_MSG_EQ (s.size (), sizeof(data), "String wrong size");

  Ptr<CCNxNameSegment> seg = Create<CCNxNameSegment> (CCNxNameSegment_Name, s);

  std::stringstream ss;
  ss << *seg;
  std::string test = ss.str ();
  NS_TEST_EXPECT_MSG_EQ (expected, test, "Hex output does not match");
}
EndTest ()

BeginTest (Copy)
{
  std::string s ("hello");
  CCNxNameSegment seg1 (CCNxNameSegment_Name, s);
  CCNxNameSegment seg2 (seg1);

  NS_TEST_EXPECT_MSG_EQ (seg1.Equals (seg2), true, "Value wrong");
}
EndTest ()

BeginTest (Equals_TypeMismatch)
{
  std::string s1 ("hello");

  CCNxNameSegment a (CCNxNameSegment_App0, s1);
  CCNxNameSegment b (CCNxNameSegment_App1, s1);

  bool result = a.Equals (b);

  NS_TEST_EXPECT_MSG_EQ (result, false, "Value wrong");
}
EndTest ()

BeginTest (Equals_ValueMismatch)
{
  std::string s1 ("hello");
  std::string s2 ("bye");

  CCNxNameSegment a (CCNxNameSegment_App0, s1);
  CCNxNameSegment b (CCNxNameSegment_App0, s2);

  bool result = a.Equals (b);

  NS_TEST_EXPECT_MSG_EQ (result, false, "Value wrong");
}
EndTest ()

BeginTest (Equals_EqualPointers)
{
  std::string s1 ("hello");

  CCNxNameSegment a (CCNxNameSegment_App0, s1);
  CCNxNameSegment b (CCNxNameSegment_App0, s1);

  bool result = a.Equals (b);

  NS_TEST_EXPECT_MSG_EQ (result, true, "Value wrong");
}
EndTest ()

BeginTest (Equals_EqualValues)
{
  std::string s1 ("hello");
  std::string s2 ("hello");

  CCNxNameSegment a (CCNxNameSegment_App0, s1);
  CCNxNameSegment b (CCNxNameSegment_App0, s2);

  bool result = a.Equals (b);

  NS_TEST_EXPECT_MSG_EQ (result, true, "Value wrong");
}
EndTest ()

BeginTest (FromString)
{
  CCNxNameSegmentType t1 = CCNxNameSegment::TypeFromString (std::string ("NAME"));
  NS_TEST_EXPECT_MSG_EQ (t1, CCNxNameSegment_Name, "wrong parse for NAME");

  CCNxNameSegmentType t2 = CCNxNameSegment::TypeFromString (std::string ("APP2"));
  NS_TEST_EXPECT_MSG_EQ (t2, CCNxNameSegment_App2, "wrong parse for APP2");

  CCNxNameSegmentType t3 = CCNxNameSegment::TypeFromString (std::string ("Name"));
  NS_TEST_EXPECT_MSG_EQ (t3, CCNxNameSegment_Name, "Could not match mixed-case string");
}
EndTest ()

BeginTest (FromEnum)
{
  std::string s1 = CCNxNameSegment::TypeFromEnum (CCNxNameSegment_Name);
  NS_TEST_EXPECT_MSG_EQ (s1, std::string ("NAME"), "wrong parse for CCNxNameSegment_Name");
}
EndTest ()

BeginTest (Parse_1)
{
  std::string s ("NAME=foo");

  CCNxNameSegment test (s);
  CCNxNameSegment truth (CCNxNameSegment_Name, "foo");

  NS_TEST_EXPECT_MSG_EQ (truth.Equals (test), true, "Error in parse");
}
EndTest ()

BeginTest (Compare)
{
  Ptr<CCNxNameSegment> base = Create<CCNxNameSegment> ("VER=abcd");

  struct
  {
    std::string s;
    int result;
    bool terminal;
  } vectors[] = {
    {std::string ("NAME=abcd"), 1, false},
    {std::string ("VER=abc"), 1, false},
    {std::string ("VER=abcc"), 1, false},
    {std::string ("VER=abc"), 1, false},
    {std::string ("VER=abcd"), 0, false},
    {std::string ("VER=abce"), -1, false},
    {std::string ("VER=abcda"), -1, false},
    {std::string ("APP0=abcd"), -1, false},
    {std::string (), 0, true}
  };

  for (int i = 0; !vectors[i].terminal; i++)
    {
      Ptr<CCNxNameSegment> segment = Create<CCNxNameSegment> (vectors[i].s);
      int test = base->Compare (*segment);
      NS_TEST_EXPECT_MSG_EQ (test, vectors[i].result, "Error in compare");
    }
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxNameSegment
 */
static class TestSuiteCCNxNameSegment : public TestSuite
{
public:
  TestSuiteCCNxNameSegment () : TestSuite ("ccnx-namesegment", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new Constructor_Binary (), TestCase::QUICK);
    AddTestCase (new Operator_Output (), TestCase::QUICK);
    AddTestCase (new Copy (), TestCase::QUICK);

    AddTestCase (new Equals_TypeMismatch (), TestCase::QUICK);
    AddTestCase (new Equals_ValueMismatch (), TestCase::QUICK);
    AddTestCase (new Equals_EqualPointers (), TestCase::QUICK);
    AddTestCase (new Equals_EqualValues (), TestCase::QUICK);

    AddTestCase (new FromString (), TestCase::QUICK);
    AddTestCase (new FromEnum (), TestCase::QUICK);
    AddTestCase (new Parse_1 (), TestCase::QUICK);
    AddTestCase (new Compare (), TestCase::QUICK);
  }
} g_TestSuiteCCNxNameSegment;

}
