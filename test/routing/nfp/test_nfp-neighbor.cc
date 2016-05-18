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
#include "../../TestMacros.h"

#include "ns3/nfp-neighbor.h"
#include "ns3/log.h"
#include "../../mockups/mockup_ccnx-virtual-connection.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace NfpNeighborTests {

/**
 * Counts the number of times the state change handler is called per state.
 */
class StateChangeHandler
{
public:
  StateChangeHandler () :
    m_neighbor (0), m_upCount (0), m_downCount (0), m_deadCount (0)
  {
  }

  void SetNeighbor (Ptr<NfpNeighbor> neighbor)
  {
    m_neighbor = neighbor;
  }

  void Handler (Ptr<NfpNeighborKey> neighborKey)
  {
    if (m_neighbor->IsStateUp ())
      {
        m_upCount++;
      }
    else if (m_neighbor->IsStateDown ())
      {
        m_downCount++;
      }
    else if (m_neighbor->IsStateDead ())
      {
        m_deadCount++;
      }
  }

  unsigned GetUpCount (void) const
  {
    return m_upCount;
  }
  unsigned GetDownCount (void) const
  {
    return m_downCount;
  }
  unsigned GetDeadCount (void) const
  {
    return m_deadCount;
  }

private:
  Ptr<NfpNeighbor> m_neighbor;
  unsigned m_upCount;
  unsigned m_downCount;
  unsigned m_deadCount;
};

Ptr<NfpNeighbor>
CreateNeighbor (const char *uri, CCNxConnection::ConnIdType connid, uint16_t seqnum, Time timeout, StateChangeHandler &handler)
{
  Ptr<CCNxName> name = Create<CCNxName> (uri);
  Ptr<CCNxVirtualConnection> conn = Create<CCNxVirtualConnection> (connid);
  return Create<NfpNeighbor> (name, conn, seqnum, timeout, MakeCallback (&StateChangeHandler::Handler, &handler));
}

BeginTest (TestConstructor)
{
  const char *uri = "ccnx:/name=router_foo";
  Ptr<CCNxName> name = Create<CCNxName> (uri);
  StateChangeHandler handler;
  Ptr<NfpNeighbor> neighbor = CreateNeighbor (uri, 33, 77, Seconds (3), handler);
  handler.SetNeighbor (neighbor);

  NS_TEST_ASSERT_MSG_EQ (name->Equals (*(neighbor->GetName ())), true, "Wrong name");
  NS_TEST_ASSERT_MSG_EQ (77, neighbor->GetMessageSeqnum (), "Wrong seqnum");
  NS_TEST_ASSERT_MSG_EQ (33, neighbor->GetConnection ()->GetConnectionId (), "Wrong connId");
}
EndTest ()

BeginTest (TestTimeout_InitToUp)
{
  Time timeout (Seconds (3));
  StateChangeHandler handler;
  Ptr<NfpNeighbor> neighbor = CreateNeighbor ("ccnx:/name=router_foo", 33, 77, timeout, handler);
  handler.SetNeighbor (neighbor);

  // stop before timeout.
  Simulator::Stop (Seconds (1));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (neighbor->IsStateUp (), true, "Should be in the UP state");
  NS_TEST_ASSERT_MSG_EQ (handler.GetUpCount (), 0, "Wrong value");
  NS_TEST_ASSERT_MSG_EQ (handler.GetDownCount (), 0, "Wrong value");
  NS_TEST_ASSERT_MSG_EQ (handler.GetDeadCount (), 0, "Wrong value");
}
EndTest ()

BeginTest (TestTimeout_UpToDown)
{
  Time timeout (Seconds (3));
  StateChangeHandler handler;
  Ptr<NfpNeighbor> neighbor = CreateNeighbor ("ccnx:/name=router_foo", 33, 77, timeout, handler);
  handler.SetNeighbor (neighbor);

  // stop after timeout
  Simulator::Stop (Seconds (4));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (handler.GetUpCount (), 0, "Wrong UP count");
  NS_TEST_ASSERT_MSG_EQ (handler.GetDownCount (), 1, "Wrong DOWN count");
  NS_TEST_ASSERT_MSG_EQ (handler.GetDeadCount (), 0, "Wrong DEAD count");
}
EndTest ()

BeginTest (TestTimeout_DownToUp)
{
  Time timeout (Seconds (3));
  StateChangeHandler handler;
  Ptr<NfpNeighbor> neighbor = CreateNeighbor ("ccnx:/name=router_foo", 33, 77, timeout, handler);
  handler.SetNeighbor (neighbor);

  // stop after timeout
  Simulator::Stop (Seconds (4));
  Simulator::Run ();

  // We should be in the DOWN state now.  Send neighbor update to go back to UP state.
  NS_TEST_ASSERT_MSG_EQ (neighbor->IsStateDown (), true, "Not in expected state");

  neighbor->ReceiveHello (78);

  NS_TEST_ASSERT_MSG_EQ (handler.GetUpCount (), 1, "Wrong UP count");
  NS_TEST_ASSERT_MSG_EQ (handler.GetDownCount (), 1, "Wrong DOWN count");
  NS_TEST_ASSERT_MSG_EQ (handler.GetDeadCount (), 0, "Wrong DEAD count");
}
EndTest ()

BeginTest (TestTimeout_DownToDead)
{
  Time timeout (Seconds (3));
  StateChangeHandler handler;
  Ptr<NfpNeighbor> neighbor = CreateNeighbor ("ccnx:/name=router_foo", 77, 33, timeout, handler);
  handler.SetNeighbor (neighbor);

  // stop after two timeouts
  Simulator::Stop (Seconds (7) + Simulator::Now ());
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (handler.GetUpCount (), 0, "Wrong UP count");
  NS_TEST_ASSERT_MSG_EQ (handler.GetDownCount (), 1, "Wrong DOWN count");
  NS_TEST_ASSERT_MSG_EQ (handler.GetDeadCount (), 1, "Wrong DEAD count");
}
EndTest ()


/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for NfpRoutingProtocol
 */
static class TestSuiteNfpNeighbor : public TestSuite
{
public:
  TestSuiteNfpNeighbor () :
    TestSuite ("nfp-neighbor", UNIT)
  {
    AddTestCase (new TestConstructor (), TestCase::QUICK);
    AddTestCase (new TestTimeout_InitToUp (), TestCase::QUICK);
    AddTestCase (new TestTimeout_UpToDown (), TestCase::QUICK);
    AddTestCase (new TestTimeout_DownToUp (), TestCase::QUICK);
    AddTestCase (new TestTimeout_DownToDead (), TestCase::QUICK);
  }
} g_TestSuiteNfpNeighbor;
}

