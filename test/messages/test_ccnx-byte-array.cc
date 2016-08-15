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
#include "ns3/ccnx-byte-array.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxByteArray {

BeginTest (Constructor_Pointer)
{
  const uint8_t truth[] = { 1, 2, 3, 4, 5 };
  Ptr<CCNxByteArray> array = Create<CCNxByteArray>(sizeof(truth), truth);

  bool exists = (array);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");

  NS_TEST_EXPECT_MSG_EQ(array->size(), sizeof(truth), "Wrong size");
}
EndTest ()

BeginTest (Constructor_Vector)
{
  const uint8_t truth[] = { 1, 2, 3, 4, 5 };
  std::vector<uint8_t> v ( truth, truth + sizeof(truth) );
  Ptr<CCNxByteArray> array = Create<CCNxByteArray>(v);

  bool exists = (array);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");

  NS_TEST_EXPECT_MSG_EQ(array->size(), v.size(), "Wrong size");
}
EndTest ()

BeginTest (size)
{
  const uint8_t truth[] = { 1, 2, 3, 4, 5, 6 };
  Ptr<CCNxByteArray> array = Create<CCNxByteArray>(sizeof(truth), truth);
  NS_TEST_EXPECT_MSG_EQ(array->size(), sizeof(truth), "Wrong size");
}
EndTest ()

BeginTest (Operator_Index)
{
  const uint8_t truth[] = { 1, 2, 3, 4, 5, 6 };
  Ptr<CCNxByteArray> array = Create<CCNxByteArray>(sizeof(truth), truth);
  for (int i = 0; i < sizeof(truth); ++i) {
      uint8_t actual = truth[i];
      uint8_t test = (*array)[i];
      NS_TEST_EXPECT_MSG_EQ(actual, test, "Wrong value");
  }
}
EndTest ()

BeginTest (Operator_Equals)
{
  const uint8_t base[] = { 1, 2, 3, 4, 5, 6 };
  Ptr<CCNxByteArray> a = Create<CCNxByteArray>(sizeof(base), base);
  Ptr<CCNxByteArray> b = Create<CCNxByteArray>(sizeof(base), base);
  Ptr<CCNxByteArray> c = Create<CCNxByteArray>(sizeof(base), base);

  const uint8_t false_x[] = { 1, 2, 3, 4 }; // wrong size
  const uint8_t false_y[] = { 1, 2, 9, 4, 5, 6 };  // wrong value

  Ptr<CCNxByteArray> x = Create<CCNxByteArray>(sizeof(false_x), false_x);
  Ptr<CCNxByteArray> y = Create<CCNxByteArray>(sizeof(false_y), false_y);

  bool equals;

  equals = ((*a) == (*b));
  NS_TEST_EXPECT_MSG_EQ(equals, true, "a != b");
  equals = ((*b) == (*c));
  NS_TEST_EXPECT_MSG_EQ(equals, true, "b != c");
  equals = ((*c) == (*a));
  NS_TEST_EXPECT_MSG_EQ(equals, true, "c != a");

  equals = ((*a) == (*x));
  NS_TEST_EXPECT_MSG_EQ(equals, false, "a == x");
  equals = ((*a) == (*y));
  NS_TEST_EXPECT_MSG_EQ(equals, false, "a == y");
}
EndTest ()

BeginTest (Operator_NotEquals)
{
  const uint8_t base[] = { 1, 2, 3, 4, 5, 6 };
  Ptr<CCNxByteArray> a = Create<CCNxByteArray>(sizeof(base), base);
  Ptr<CCNxByteArray> b = Create<CCNxByteArray>(sizeof(base), base);
  Ptr<CCNxByteArray> c = Create<CCNxByteArray>(sizeof(base), base);

  const uint8_t false_x[] = { 1, 2, 3, 4 }; // wrong size
  const uint8_t false_y[] = { 1, 2, 9, 4, 5, 6 };  // wrong value

  Ptr<CCNxByteArray> x = Create<CCNxByteArray>(sizeof(false_x), false_x);
  Ptr<CCNxByteArray> y = Create<CCNxByteArray>(sizeof(false_y), false_y);

  bool equals;

  equals = ((*a) != (*b));
  NS_TEST_EXPECT_MSG_EQ(equals, false, "a != b");
  equals = ((*b) != (*c));
  NS_TEST_EXPECT_MSG_EQ(equals, false, "b != c");
  equals = ((*c) != (*a));
  NS_TEST_EXPECT_MSG_EQ(equals, false, "c != a");

  equals = ((*a) != (*x));
  NS_TEST_EXPECT_MSG_EQ(equals, true, "a == x");
  equals = ((*a) != (*y));
  NS_TEST_EXPECT_MSG_EQ(equals, true, "a == y");
}
EndTest ()

BeginTest (Operator_Less)
{

  const uint8_t aArray[] = { 1, 2, 3, 4, 5, 6 };
  Ptr<CCNxByteArray> a = Create<CCNxByteArray>(sizeof(aArray), aArray);
  Ptr<CCNxByteArray> equal = Create<CCNxByteArray>(sizeof(aArray), aArray);

  const uint8_t shorterArray[] = { 1, 2, 3, 4 };
  Ptr<CCNxByteArray> shorter = Create<CCNxByteArray>(sizeof(shorterArray), shorterArray);

  const uint8_t longerArray[] = { 1, 2, 3, 4, 5, 6, 7 };
  Ptr<CCNxByteArray> longer = Create<CCNxByteArray>(sizeof(longerArray), longerArray);

  const uint8_t lessArray[] = { 1, 2, 3, 4, 5, 5 };
  Ptr<CCNxByteArray> less = Create<CCNxByteArray>(sizeof(lessArray), lessArray);

  bool isLess;

  isLess = ((*a) < (*a));
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "a < a");

  isLess = ((*a) < (*equal));
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "a < equal");

  isLess = ((*a) < (*shorter));
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "a < shorter");

  isLess = ((*shorter) < (*a));
  NS_TEST_EXPECT_MSG_EQ(isLess, true, "shorter !< a");

  isLess = ((*a) < (*longer));
  NS_TEST_EXPECT_MSG_EQ(isLess, true, "a !< longer");

  isLess = ((*longer) < (*a));
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "longer < a");

  isLess = ((*a) < (*less));
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "a < less");

  isLess = ((*less) < (*a));
  NS_TEST_EXPECT_MSG_EQ(isLess, true, "less !< a");
}
EndTest ()

BeginTest (Iterator)
{
  const uint8_t aArray[] = { 1, 2, 3, 4, 5, 6 };
  Ptr<CCNxByteArray> a = Create<CCNxByteArray>(sizeof(aArray), aArray);

  int offset = 0;
  for (CCNxByteArray::const_iterator i = a->begin(); i != a->end(); ++i) {
      uint8_t truth = aArray[offset];
      uint8_t test = *i;
      NS_TEST_EXPECT_MSG_EQ(truth, test, "Incorrect value from iterator");
      ++offset;
  }
}
EndTest ()

BeginTest (CreateBuffer)
{
  const uint8_t aArray[] = { 1, 2, 3, 4, 5, 6 };
  Ptr<CCNxByteArray> a = Create<CCNxByteArray>(sizeof(aArray), aArray);

  Ptr<CCNxBuffer> buffer = a->CreateBuffer();
  bool exists = (buffer);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer from CreateBuffer");

  NS_TEST_EXPECT_MSG_EQ(a->size(), buffer->GetSize(), "Wrong buffer size");

  int offset = 0;

  Buffer::Iterator i = buffer->Begin();
  while (! i.IsEnd() ) {
      uint8_t truth = aArray[offset];
      uint8_t test = i.ReadU8();
      NS_TEST_EXPECT_MSG_EQ(truth, test, "Incorrect value from iterator");
      ++offset;
  }
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxByteArray
 */
static class TestSuiteCCNxByteArray : public TestSuite
{
public:
  TestSuiteCCNxByteArray () : TestSuite ("ccnx-byte-array", UNIT)
  {
    AddTestCase (new Constructor_Pointer (), TestCase::QUICK);
    AddTestCase (new Constructor_Vector (), TestCase::QUICK);
    AddTestCase (new size (), TestCase::QUICK);
    AddTestCase (new Operator_Index (), TestCase::QUICK);
    AddTestCase (new Operator_Equals (), TestCase::QUICK);
    AddTestCase (new Operator_NotEquals (), TestCase::QUICK);
    AddTestCase (new Operator_Less (), TestCase::QUICK);
    AddTestCase (new Iterator (), TestCase::QUICK);
    AddTestCase (new CreateBuffer (), TestCase::QUICK);
  }
} g_TestSuiteCCNxByteArray;

} // namespace TestSuiteCCNxByteArray
