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


// Network topology
//
//  n0                                  n4
//     \ 5 Mb/s, 2ms                   /
//      \          1.5Mb/s, 10ms      /
//       n2 -------------------------n3
//      /                             \
//     / 5 Mb/s, 2ms                    \
//   n1                                   n5
//

// This example is to demonstrate the protocol ability to aggregate interests.
// Nodes n0 and n1 are hosting nProducers
// Node n4 and n5 are expressing interests and expecting content.
// Node n2 will route the requests to both n0 and n1. n0 and n1 will respond but the PIT table on n2 should only forward 1/2 of these responses.
// Node n3 will aggregate the interests if the timers are tweaked right.


#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ccns3Sim-module.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("ccnx-6node-topo");

typedef std::vector < Ptr <CCNxContentRepository> > contentRepoList;

static void
RunSimulation (uint32_t nPrefixes)
{
  NS_LOG_INFO ("Number of Prefixes to simulate are = " << nPrefixes );
  Time::SetResolution (Time::NS);
  NodeContainer nodes;
  nodes.Create (6);

  NodeContainer n0n2 = NodeContainer (nodes.Get (0), nodes.Get (2));
  NodeContainer n1n2 = NodeContainer (nodes.Get (1), nodes.Get (2));
  NodeContainer n3n2 = NodeContainer (nodes.Get (3), nodes.Get (2));
  NodeContainer n4n3 = NodeContainer (nodes.Get (4), nodes.Get (3));
  NodeContainer n5n3 = NodeContainer (nodes.Get (5), nodes.Get (3));
  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  p2p.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  NetDeviceContainer d0d2 = p2p.Install (n0n2);
  NetDeviceContainer d1d2 = p2p.Install (n1n2);
  NetDeviceContainer d4d3 = p2p.Install (n4n3);
  NetDeviceContainer d5d3 = p2p.Install (n5n3);

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (1500000)));
  p2p.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer d3d2 = p2p.Install (n3n2);

  /*
   * Setup a Forwarder and associate it with the nodes created.
   */
  CCNxStackHelper ccnxStack;
  CCNxStandardForwarderHelper standardHelper;
  ccnxStack.SetForwardingHelper (standardHelper);

  NfpRoutingHelper nfpHelper;
  nfpHelper.Set ("HelloInterval", TimeValue (Seconds (5)));
#if DEBUG
  Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
  nfpHelper.PrintNeighborTableAllEvery (Time (Seconds (5)), trace);
  nfpHelper.PrintRoutingTableAllEvery (Time (Seconds (5)), trace);
#endif
  ccnxStack.SetRoutingHelper (nfpHelper);

  ccnxStack.Install (nodes);
  ccnxStack.AddInterfaces (d0d2);
  ccnxStack.AddInterfaces (d1d2);
  ccnxStack.AddInterfaces (d3d2);
  ccnxStack.AddInterfaces (d4d3);
  ccnxStack.AddInterfaces (d5d3);

  const std::string pre = "ccnx:/name=6nodetopo/name=producer/";
  contentRepoList globalRepoList;
  char fix[64];
  for (int ii = 0; ii < nPrefixes; ii++)
    {
      uint32_t size = (64 * (ii + 1)) % 1504;
      if (size < 64)
        {
          size = 64;
        }
      uint32_t count = 10 * (ii + 1);
      sprintf (fix, "name=size%dcount%d", size, count);
      globalRepoList.push_back (Create <CCNxContentRepository> (Create <CCNxName> (pre + fix),size,count));
    }

  for (contentRepoList::iterator ii = globalRepoList.begin (); ii != globalRepoList.end (); ii++)
    {
      CCNxProducerHelper producerHelper (*ii);
      ApplicationContainer producern0 = producerHelper.Install (nodes.Get (0));
      ApplicationContainer producern1 = producerHelper.Install (nodes.Get (1));
      producern0.Start (Seconds (0.0));
      producern0.Stop (Seconds (120.0));
      producern1.Start (Seconds (0.0));
      producern1.Stop (Seconds (120.0));
    }

  for (contentRepoList::iterator ii = globalRepoList.begin (); ii != globalRepoList.end (); ii++)
    {
      CCNxConsumerHelper consumerHelper (*ii);
      consumerHelper.SetAttribute ("RequestInterval", TimeValue (MilliSeconds (250)));
      ApplicationContainer cAppsn4 = consumerHelper.Install (nodes.Get (4));
      ApplicationContainer cAppsn5 = consumerHelper.Install (nodes.Get (5));
      cAppsn4.Start (Seconds (5.0));
      cAppsn5.Start (Seconds (2.0));
      cAppsn4.Stop (Seconds (105.0));
      cAppsn5.Stop (Seconds (102.0));
    }

  Simulator::Stop (Seconds (122));

  // Run the simulator and execute all the events
  Simulator::Run ();
  Simulator::Destroy ();

}

int
main (int argc, char *argv[])
{
  uint32_t nPrefixes = 5;
  CommandLine cmd;
  cmd.AddValue ("nPrefixes", "Number of Prefixes to simulate", nPrefixes);
  cmd.Parse (argc, argv);

  RunSimulation (nPrefixes);
  return 0;
}
