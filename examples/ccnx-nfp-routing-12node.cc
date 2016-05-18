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

/*
 * Uses a point-to-poit topology.  Sink is on n0, source is n10 for ccnx:/name=foo/name=sink.
 *
 *                             -----------n11
 *                             |           |
 *                n4 --  n5 -- n6 --- n7 \ |
 *                |      |     |   x  |   n9 -- n10
 * n0 ----------- n1     n2    n3 --- n8 /
 *     5Mbps      |      |     |
 *     2ms      |------ LAN -----|
 *                    100 Mbps / 1 usec
 */

#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"

#include "ns3/ccns3Sim-module.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxNfpRouting");

static const char * sinkNameString = "ccnx:/name=foo/name=sink";

static Ptr<CCNxPacket>
CreatePacket (uint32_t size, Ptr<CCNxName> name)
{
  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name);
  Ptr<CCNxPacket> packet = CCNxPacket::CreateFromMessage (interest);

  return packet;
}

static void
GenerateTraffic (Ptr<CCNxPortal> source, uint32_t size, Ptr<CCNxName> name)
{
  std::cout << "Client Send    at=" << Simulator::Now ().GetSeconds () << "s, tx bytes=" << size << std::endl;

  Ptr<CCNxPacket> packet = CreatePacket (size, name);
  source->Send (packet);
  if (size > 0)
    {
      // Every 1/2 second, send a packet by calling GenerateTraffic.
      // Keep doing this until (size - 50) is not positive
      Simulator::Schedule (Seconds (0.5), &GenerateTraffic, source, size - 50, name);
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
      std::cout << "Client Receive at=" << Simulator::Now ().GetSeconds () << "s, name=" << packet->GetMessage ()->GetName () << std::endl;
    }
}

static void
PrintTraffic (Ptr<CCNxPortal> sink)
{
  sink->SetRecvCallback (MakeCallback (&PortalPrinter));
}

static void
RunSimulation (void)
{
//    LogComponentEnable ("CCNxStandardLayer3", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
//    LogComponentEnable ("NfpRoutingProtocol", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
//    LogComponentEnable ("CCNxStandardForwarder", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
//    LogComponentEnable ("CCNxPacket", (LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
//    LogComponentEnable ("CCNxConnectionDevice", (LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
//    LogComponentEnable ("CCNxCodecFixedHeader", (LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
//    LogComponentEnable ("CCNxForwarder", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
//    LogComponentEnable ("CCNxMessagePortal", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));

  Time::SetResolution (Time::NS);

  NodeContainer nodes;
  nodes.Create (12);

  Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);

  // ==== Point To Point
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
//    pointToPoint.EnableAsciiAll(trace);

  /*
   * Uses a point-to-poit topology.  Sink is on n0, source is n10 for ccnx:/name=foo/name=sink.
   *
   *                             -----------n11
   *                             |           |
   *                n4 --  n5 -- n6 --- n7 \ |
   *                |      |     |   x  |   n9 -- n10
   * n0 ----------- n1     n2    n3 --- n8 /
   *     5Mbps      |      |     |
   *     2ms      |------ LAN -----|
   *                    100 Mbps / 1 usec
   */

  NetDeviceContainer pppDevices;

  pppDevices.Add (pointToPoint.Install (nodes.Get (0), nodes.Get (1)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (1), nodes.Get (4)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (2), nodes.Get (5)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (3), nodes.Get (6)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (3), nodes.Get (8)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (3), nodes.Get (7)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (4), nodes.Get (5)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (5), nodes.Get (6)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (6), nodes.Get (7)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (6), nodes.Get (8)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (6), nodes.Get (11)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (7), nodes.Get (8)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (7), nodes.Get (9)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (8), nodes.Get (9)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (9), nodes.Get (10)));
  pppDevices.Add (pointToPoint.Install (nodes.Get (9), nodes.Get (11)));

  // ==== CSMA
  NodeContainer csmaNodes;
  csmaNodes.Add (nodes.Get (1));
  csmaNodes.Add (nodes.Get (2));
  csmaNodes.Add (nodes.Get (3));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (1000000000));    // 1 Gbps
  csma.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (1)));
  NetDeviceContainer csmaDevices = csma.Install (csmaNodes);

  NfpRoutingHelper nfpHelper;
  nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
  nfpHelper.PrintNeighborTableAllNodesWithInterval (Time (Seconds (5)), trace);
  nfpHelper.PrintRoutingTableAllNodesWithInterval (Time (Seconds (5)), trace);

  // Setup a CCNxL3Protocol on all the nodes
  CCNxStackHelper ccnxStack;

  ccnxStack.SetRoutingHelper (nfpHelper);
  ccnxStack.Install (nodes);
//    ccnxStack.EnableAsciiCCNx(trace);
  ccnxStack.AddInterfaces (pppDevices);
  ccnxStack.AddInterfaces (csmaDevices);

  // Now send packets and run simulation

  // Create a CCNxPortal on the sink and have it register a Name
  TypeId tid = TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory");
  Ptr<CCNxName> sinkName = Create<CCNxName> (sinkNameString);

  Ptr<CCNxPortal> sink = CCNxPortal::CreatePortal (nodes.Get (0), tid);
  PrintTraffic (sink);

  // cause the routing protocol on Sink to advertise the name
  sink->RegisterAnchor (sinkName);

  // Create a CCNxPortal on the source and have it send Interests
  Ptr<CCNxPortal> source = CCNxPortal::CreatePortal (nodes.Get (10), tid);
  Simulator::Schedule (Seconds (0.5), &GenerateTraffic, source, 500, sinkName);

  // Run the simulator and execute all the events
  Simulator::Stop (Seconds (10));
  Simulator::Run ();
  Simulator::Destroy ();
}

int
main (int argc, char *argv[])
{
  RunSimulation ();
  return 0;
}
