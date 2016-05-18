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
 * Uses a single broadcast domain.  Sink will RegisterPrefix() on n0 which creates a route from n0 to the sink
 * Portal.  We add a static route on node n1 -> n0 for ccnx:/name=foo/name=sink.
 *
 * sink      source
 *  |         |
 * n0 ------ n1
 *     10Mbps
 *     2ms
 */

#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/ccns3Sim-module.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("ccnx-csma-1bcast");

static const char * sinkNameString = "ccnx:/name=foo/name=sink";

static Ptr<CCNxPacket>
CreatePacket (uint32_t size, Ptr<CCNxName> name)
{
  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (size, true);

  Ptr<CCNxInterest> interest = Create<CCNxInterest> (name, payload);
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
  Ptr<const CCNxName>   namePtr;
  while ((packet = portal->Recv ()))
    {
      namePtr = packet->GetMessage ()->GetName ();
      std::cout << "Client Receive at=" << Simulator::Now ().GetSeconds () << "s Number of Segments = " << namePtr->GetSegmentCount () << std::endl;
    }
}

static void
RunSimulation (void)
{
#if 0
  LogComponentEnable ("CCNxL3Protocol", LOG_LEVEL_ALL);
  LogComponentEnable ("CCNxForwarder", LOG_LEVEL_ALL);
  LogComponentEnable ("CCNxMessagePortal", LOG_LEVEL_ALL);
#endif
  LogComponentEnable ("ccnx-csma-1bcast", LOG_LEVEL_ALL);

  Time::SetResolution (Time::NS);

  NS_LOG_INFO ("Create Nodes.");
  NodeContainer nodes;
  nodes.Create (2);

  NS_LOG_INFO ("Create Channel.");
  CsmaHelper etherNet;
  etherNet.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  etherNet.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  etherNet.SetDeviceAttribute ("Mtu", UintegerValue (1400));

  NetDeviceContainer devices = etherNet.Install (nodes);

  NS_LOG_INFO ("Create Forwarder Stack.");
  // Setup a CCNxL3Protocol on all the nodes
  CCNxStackHelper ccnx;

  ccnx.Install (nodes);
  ccnx.AddInterfaces (devices);

  NS_LOG_INFO ("Setup Static Route");
  // Add a route from n1 to n0
  Ptr<Node> node0 = nodes.Get (0);
  Ptr<Node> node1 = nodes.Get (1);
  Ptr<NetDevice> node0If0 = node0->GetDevice (0);
  Ptr<NetDevice> node1If0 = node1->GetDevice (0);

  Ptr<CCNxL3Protocol> node1_ccnx = node1->GetObject<CCNxL3Protocol> ();

  // Tell the forwarder on node 1 that node0 if0's MAC address is reachable via node1 if 0.
  // We need to create the neighbor adjacency on node1 so we can then add a route
  // via that adjacency next.
  Ptr<CCNxConnectionDevice> node1ToNode0Connection = node1_ccnx->AddNeighbor (node0If0->GetAddress (), node1If0);

  Ptr<CCNxName> sinkName = Create<CCNxName> (sinkNameString);
  node1_ccnx->GetForwarder ()->AddRoute (node1ToNode0Connection, sinkName);

  // Now send packets and run simulation

  // Create a CCNxPortal on the sink and have it register a Name
  TypeId tid = TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory");

  Ptr<CCNxPortal> node0Portal = CCNxPortal::CreatePortal (node0, tid);
  // the order of these does not matter.  Nothing is going to call the callback until we start
  // the simulator with Simulator::Run().
  node0Portal->SetRecvCallback (MakeCallback (&PortalPrinter));
  node0Portal->RegisterPrefix (sinkName);

  // Create a CCNxPortal on the source and have it send Interests
  Ptr<CCNxPortal> node1Portal = CCNxPortal::CreatePortal (node1, tid);
  GenerateTraffic (node1Portal, 500, sinkName);

#if 0
  // Configure ascii tracing of all enqueue, dequeue, and NetDevice receive
  // events on all devices.  Trace output will be sent to the file
  // "csma-one-subnet.tr"
  AsciiTraceHelper ascii;
  etherNet.EnableAsciiAll (ascii.CreateFileStream ("ccnx-csma-1bcast.tr"));
#endif

  // Also configure some tcpdump traces; each interface will be traced
  // The output files will be named
  // csma-broadcast-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -tt -r" command
  etherNet.EnablePcapAll ("ccnx-csma-broadcast", false);


  // Run the simulator and execute all the events
  Simulator::Run ();
  Simulator::Destroy ();

}

int
main (int argc, char *argv[])
{
  RunSimulation ();
  return 0;
}
