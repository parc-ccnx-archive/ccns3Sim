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
#include "ns3/ccnx-hash-value.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxHashValue {


BeginTest (Constructor_u64)
{
  uint64_t value = 0x0102030405060708LL;
  size_t length = 32; // the default

  Ptr<CCNxHashValue> hv = Create<CCNxHashValue>(value);

  bool exists = (hv);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");

  Ptr<const CCNxByteArray> test = hv->GetValue();
  exists = (test);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null value");

  NS_TEST_EXPECT_MSG_EQ (length, hv->size(), "Got wrong value length");
}
EndTest ()

BeginTest (Constructor_u64_length)
{
  uint64_t value = 0x0102030405060708LL;
  size_t length = 16;

  Ptr<CCNxHashValue> hv = Create<CCNxHashValue>(value, length);

  bool exists = (hv);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");

  Ptr<const CCNxByteArray> test = hv->GetValue();
  exists = (test);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null value");

  NS_TEST_EXPECT_MSG_EQ (length, hv->size(), "Got wrong value length");
}
EndTest ()

BeginTest (Constructor_bytearray)
{
  const uint8_t array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
  const size_t length = sizeof(array);
  Ptr<CCNxByteArray> bytearray = Create<CCNxByteArray>(length, array);
  Ptr<CCNxHashValue> hv = Create<CCNxHashValue>(bytearray);
  bool exists = (hv);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");

  Ptr<const CCNxByteArray> test = hv->GetValue();
  exists = (test);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null value");

  NS_TEST_EXPECT_MSG_EQ (length, hv->size(), "Got wrong value length");
}
EndTest ()

BeginTest (GetValue)
{
  const uint8_t array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
  const size_t length = sizeof(array);
  Ptr<CCNxByteArray> bytearray = Create<CCNxByteArray>(length, array);
  Ptr<CCNxHashValue> hv = Create<CCNxHashValue>(bytearray);
  Ptr<const CCNxByteArray> test = hv->GetValue();

  bool equals = *bytearray == *test;
  NS_TEST_EXPECT_MSG_EQ (equals, true, "Value not equal to input");
}
EndTest ()

BeginTest (CreateBuffer)
{
  const uint8_t aArray[] = { 1, 2, 3, 4, 5, 6 };
  Ptr<CCNxByteArray> ba = Create<CCNxByteArray>(sizeof(aArray), aArray);
  Ptr<CCNxHashValue> hv = Create<CCNxHashValue>(ba);

  Ptr<CCNxBuffer> buffer = hv->CreateBuffer();
  bool exists = (buffer);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer from CreateBuffer");

  NS_TEST_EXPECT_MSG_EQ(hv->size(), buffer->GetSize(), "Wrong buffer size");

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

BeginTest (Operator_Equals)
{
  Ptr<CCNxHashValue> a = Create<CCNxHashValue>(77);
  Ptr<CCNxHashValue> b = Create<CCNxHashValue>(77);
  Ptr<CCNxHashValue> c = Create<CCNxHashValue>(77);

  Ptr<CCNxHashValue> x = Create<CCNxHashValue>(44); // unequal in value
  Ptr<CCNxHashValue> y = Create<CCNxHashValue>(77, 16);  // unequal in length

  bool equals;

  equals = *a == *a;
  NS_TEST_EXPECT_MSG_EQ(equals, true, "a != a");

  equals = *a == *b;
  NS_TEST_EXPECT_MSG_EQ(equals, true, "a != b");

  equals = *b == *c;
  NS_TEST_EXPECT_MSG_EQ(equals, true, "b != c");

  equals = *c == *a;
  NS_TEST_EXPECT_MSG_EQ(equals, true, "c != a");

  equals = *a == *x;
  NS_TEST_EXPECT_MSG_EQ(equals, false, "a == x");

  equals = *a == *y;
  NS_TEST_EXPECT_MSG_EQ(equals, false, "a == y");
}
EndTest ()

BeginTest (Operator_Less)
{
  Ptr<CCNxHashValue> a = Create<CCNxHashValue>(77, 16);
  Ptr<CCNxHashValue> less = Create<CCNxHashValue>(50, 16);
  Ptr<CCNxHashValue> greater = Create<CCNxHashValue>(88, 16);
  Ptr<CCNxHashValue> shorter = Create<CCNxHashValue>(77, 7);
  Ptr<CCNxHashValue> longer = Create<CCNxHashValue>(77, 24);

  bool isLess;

  isLess = *a < *a;
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "a < a");

  isLess = *a < *less;
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "a < less");

  isLess = *less < *a;
  NS_TEST_EXPECT_MSG_EQ(isLess, true, "less !< a");

  isLess = *a < *greater;
  NS_TEST_EXPECT_MSG_EQ(isLess, true, "a !< greater");

  isLess = *greater < *a;
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "greater < a");

  isLess = *a < *shorter;
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "a < shorter");

  isLess = *shorter < *a;
  NS_TEST_EXPECT_MSG_EQ(isLess, true, "shorter !< a");

  isLess = *a < *longer;
  NS_TEST_EXPECT_MSG_EQ(isLess, true, "a !< longer");

  isLess = *longer < *a;
  NS_TEST_EXPECT_MSG_EQ(isLess, false, "longer < a");

}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxHashValue
 */
static class TestSuiteCCNxHashValue : public TestSuite
{
public:
  TestSuiteCCNxHashValue () : TestSuite ("ccnx-hash-value", UNIT)
  {
    AddTestCase (new Constructor_u64 (), TestCase::QUICK);
    AddTestCase (new Constructor_u64_length (), TestCase::QUICK);
    AddTestCase (new Constructor_bytearray (), TestCase::QUICK);
    AddTestCase (new GetValue (), TestCase::QUICK);
    AddTestCase (new CreateBuffer (), TestCase::QUICK);
    AddTestCase (new Operator_Equals (), TestCase::QUICK);
    AddTestCase (new Operator_Less (), TestCase::QUICK);
  }
} g_TestSuiteCCNxHashValue;

} // namespace TestSuiteCCNxHashValue
