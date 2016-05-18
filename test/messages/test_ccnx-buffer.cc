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
#include "ns3/ccnx-buffer.h"

#include "../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxBuffer {

BeginTest (Constructor_DataParam)
{
  size_t bufLen = 5;
  Ptr<CCNxBuffer> buf = Create<CCNxBuffer> (bufLen, "apple");
  bool exists = (buf);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");
}
EndTest ()

BeginTest (Constructor_Initialize)
{
  size_t bufLen = 5;
  Ptr<CCNxBuffer> buf = Create<CCNxBuffer> (bufLen, true);
  bool exists = (buf);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");
}
EndTest ()

BeginTest (Constructor_Length)
{
  size_t bufLen = 5;
  Ptr<CCNxBuffer> buf = Create<CCNxBuffer> (bufLen);
  bool exists = (buf);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Gut null pointer");

  Ptr<CCNxBuffer> copybuf = Create<CCNxBuffer> (buf->GetBuffer ());
  bool copyexists = (copybuf);
  NS_TEST_EXPECT_MSG_EQ (copyexists, true, "Gut null pointer");
}
EndTest ()

BeginTest (GetSize)
{
  size_t bufLen = 5;
  Ptr<CCNxBuffer> buf = Create<CCNxBuffer> (bufLen);
  size_t test = buf->GetSize ();
  NS_TEST_EXPECT_MSG_EQ (test, bufLen, "Got wrong value");
}
EndTest ()

BeginTest (Equals)
{
  printf ("TestSuiteCCNxBuffer DoRun\n");

  Ptr<CCNxBuffer> a = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxBuffer> b = Create<CCNxBuffer> (5, "apple");
  Ptr<CCNxBuffer> c = Create<CCNxBuffer> (5, "apple");

  Ptr<CCNxBuffer> x = Create<CCNxBuffer> (6, "apples");

  // transitivity of equals
  NS_TEST_EXPECT_MSG_EQ (a->Equals (b), true, "not equal");
  NS_TEST_EXPECT_MSG_EQ (b->Equals (c), true, "not equal");
  NS_TEST_EXPECT_MSG_EQ (c->Equals (a), true, "not equal");

  // and the counter case
  NS_TEST_EXPECT_MSG_EQ (a->Equals (x), false, "not equal");

  // Negative test
  Ptr<CCNxBuffer> test = 0;
  NS_TEST_EXPECT_MSG_EQ (a->Equals (test), false, "not equal");
}
EndTest ()

BeginTest (IteratorBeginEnd)
{
  Ptr<CCNxBuffer> buffer = Create<CCNxBuffer> (5, "apple");

  Buffer::Iterator start = buffer->Begin ();
  while (!start.IsEnd ())
    {
      uint8_t byte = start.ReadU8 ();
      std::cout << (uint32_t) byte << ",";
    }
  std::cout << "end" << std::endl;
}
EndTest ()

BeginTest (AddAtStart)
{
  size_t bufLen = 5;
  Ptr<CCNxBuffer> buf = Create<CCNxBuffer> (0);
  buf->AddAtStart (bufLen);
  size_t test = buf->GetSize ();
  NS_TEST_EXPECT_MSG_EQ (test, bufLen, "Got wrong value");
}
EndTest ()

/**
 * @ingroup ccnx-test
 *
 * Test Suite for CCNxBuffer
 */
static class TestSuiteCCNxBuffer : public TestSuite
{
public:
  TestSuiteCCNxBuffer () : TestSuite ("ccnx-buffer", UNIT)
  {
    AddTestCase (new Constructor_DataParam (), TestCase::QUICK);
    AddTestCase (new Constructor_Initialize (), TestCase::QUICK);
    AddTestCase (new Constructor_Length (), TestCase::QUICK);
    AddTestCase (new GetSize (), TestCase::QUICK);
    AddTestCase (new Equals (), TestCase::QUICK);
    AddTestCase (new AddAtStart (), TestCase::QUICK);
    AddTestCase (new IteratorBeginEnd (), TestCase::QUICK);
  }
} g_TestSuiteCCNxBuffer;

} // namespace TestSuiteCCNxBuffer
