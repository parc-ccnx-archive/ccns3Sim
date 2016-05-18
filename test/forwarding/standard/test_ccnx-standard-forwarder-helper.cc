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
#include "ns3/ccnx-standard-forwarder-helper.h"
#include "ns3/ccnx-connection.h"
#include "ns3/log.h"
#include "ns3/assert.h"

#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxStandardForwarderHelper {

//DONE
//BASIC TESTS (no keyid, expiry or hash restrictions)
//b1 one interest one content
//b1a one interest one nonmatching content
//b2 two interests different ports one content
//b3 two interests same port one content
//b4 two interests one content, repeat
//b5 no interest one content

//TODO CCN - KEYID and HASH unit tests as listed below
// KEYID
//1 keyid interest, content key doesnt match
//receiveInterest -  with keyid restriction - verdict forward
//satisfyInterest -  content wrong keyid  - return no ports, not empty
//1 keyid interest, 1 no key id interest, content  matches both
//receiveInterest -  no keyid restriction - verdict forward
//receiveInterest -  with keyid restriction - verdict aggregate
//satisfyInterest - matching content for keyid  - return 2 ports, empty
// 2 keyid interests, content matches keyid 1
//receiveInterest -  with keyid restriction 1- verdict forward
//receiveInterest -  with keyid restriction 2 - verdict aggregate
//satisfyInterest - matching content for keyid 1 - return 1 port, not empty
// 1 keyid interest, 1 no key id interest, content matches no keyid
//receiveInterest -  no keyid restriction
//receiveInterest -  with keyid restriction
//satisfyInterest - matching content for nokeyid  - return 1 port, not empty
// 2 same keyid interests different ports, content matches both
//receiveInterest -  with keyid restriction 1- verdict forward
//receiveInterest -  with keyid restriction 1 - verdict aggregate
//satisfyInterest - matching content for keyid 1 - return 2 port,  empty


//HASH (only affects DoesObjectSatisfyInterest)
//1 interest with name and hash, matching content
//recieveInterest with name and hash restriction  - should return verdict forward
//satisfyInterest with name and hash restriction - should return 1 port, empty
//1 interest with name and hash, content with matching name but non matching hash
//recieveInterest with name and hash restriction  - should return verdict forward
//satisfyInterest with name and hash restriction - should return no port, not empty
//1 interest with no name and hash, matching content
//recieveInterest with no name and hash restriction  - should return verdict forward
//satisfyInterest with no name and hash restriction - should return 1 port, empty
//1 interest with no name and hash, matching hash but different name content
//recieveInterest with no name and hash restriction  - should return verdict forward
//satisfyInterest with different name and hash restriction - should return 1 port, empty


// EXPIRY
//1 interest 1 content but interest expired
//receiveInterest - no key restriction
//satisfyInterest - matching content - but interest has expired
//1 interest 1 content but content expired
//receiveInterest - no key restriction
//satisfyInterest - matching content - but content has expired
//2 interest 1 content but one interest has expired
//receiveInterest - no key restriction
//receiveInterest - no key restriction
//satisfyInterest - matching content - but one interest has expired

BeginTest (Constructor)
{
  printf ("TestCCNxStandardForwarderHelperConstructor DoRun\n");
//  LogComponentEnable("CCNxStandardForwarderHelper", LOG_LEVEL_DEBUG);
//  LogComponentEnable("CCNxStandardForwarder", LOG_LEVEL_DEBUG);

}
EndTest ()







/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for CCNxStandardForwarderHelper
 */
static class TestSuiteCCNxStandardForwarderHelper : public TestSuite
{
public:
  TestSuiteCCNxStandardForwarderHelper () : TestSuite ("ccnx-standard-forwarder-helper", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
//   AddTestCase (new b1 (), TestCase::QUICK);
//   AddTestCase (new b1a (), TestCase::QUICK);
//   AddTestCase (new b2 (), TestCase::QUICK);
//   AddTestCase (new b3 (), TestCase::QUICK);
//   AddTestCase (new b4 (), TestCase::QUICK);
//   AddTestCase (new b5 (), TestCase::QUICK);

  }
} g_TestSuiteCCNxStandardForwarderHelper;


} // namespace TestSuiteCCNxStandardForwarderHelper
