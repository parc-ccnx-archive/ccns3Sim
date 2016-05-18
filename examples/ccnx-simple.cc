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

#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ccns3Sim-module.h"

using namespace ns3;
using namespace ns3::ccnx;

static Ptr<CCNxPacket>
CreatePacket (uint32_t size)
{
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (size, true);
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (Create<CCNxName> ("ccnx:/name=foo/name=sink"), payload);
  Ptr<CCNxPacket> packet = CCNxPacket::CreateFromMessage (interest);

  return packet;
}

static void
GenerateTraffic (Ptr<CCNxPortal> source, uint32_t size)
{
  std::cout << "Client Send   at=" << Simulator::Now ().GetSeconds () << "s, tx bytes=" << size << std::endl;

  Ptr<CCNxPacket> packet = CreatePacket (size);
  source->Send (packet);
  if (size > 0)
    {
      // Every 1/2 second, send a packet by calling GenerateTraffic.
      // Keep doing this until (size - 50) is not positive
      Simulator::Schedule (Seconds (0.5), &GenerateTraffic, source, size - 50);
    }
  else
    {
      source->Close ();
    }
}

static void
PortalPrinter (Ptr<CCNxPortal> portal)
{
  Ptr<CCNxPacket> packet;
  while ((packet = portal->Recv ()))
    {
      std::cout << "Client Receive at=" << Simulator::Now ().GetSeconds () << "s, name=" << packet->GetMessage ()->GetName ()
                << std::endl;
    }
}

void
RunSimulation (void)
{
  LogComponentEnable ("CCNxL3Protocol", LOG_LEVEL_INFO);
  LogComponentEnable ("CCNxStandardForwarder", LOG_LEVEL_INFO);
  LogComponentEnable ("CCNxMessagePortal", LOG_LEVEL_ALL);

  /*
   * Create one node and put a sink and a source on it and send a message
   * from the source to the sink.
   */
  NodeContainer c;
  c.Create (1);

  CCNxStackHelper ccnx;

  ccnx.Install (c);

  // Create a CCNxPortal on the sink and have it register a Name
  TypeId tid = TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory");
  Ptr<Node> node = c.Get (0);

  Ptr<CCNxPortal> sink = CCNxPortal::CreatePortal (node, tid);
  sink->SetRecvCallback (MakeCallback (&PortalPrinter));
  Ptr<CCNxName> sinkName = Create<CCNxName> ("ccnx:/name=foo/name=sink");
  sink->RegisterPrefix (sinkName);

  // Create a CCNxPortal on the source and have it send Interests
  Ptr<CCNxPortal> source = CCNxPortal::CreatePortal (node, tid);
  GenerateTraffic (source, 500);

  // Run the simulator and execute all the events
  Simulator::Run ();
  Simulator::Destroy ();
}

int main (int argc, char *argv[])
{
  RunSimulation ();

  return 0;
}
