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
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"

#include "ns3/ccns3Sim-module.h"


using namespace ns3;
using namespace ns3::ccnx;

#define DEBUG 0
#define DEBUG_TRACE 0

void
RunSimulation (std::string uri, uint32_t nSize, uint32_t nCount, double nSecsToRun)
{
  LogComponentEnable ("CCNxStandardPit", (LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxStandardPitEntry", (LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));

  Time::SetResolution (Time::NS);

  /*
   * Uses a point-to-point topology.
   *
   * producer  relay     consumer
   *  |         |        |
   * n0 ------ n1 ------ n2
   *   Lossy        Lossless
   *   link         Link
   *   1Gbps        100Mbps
   *   20ms          10ms
   *
   */

  /*
   * Step-1 Create Nodes per above topology.
   */
  NodeContainer nodes;
  nodes.Create (3);
  NodeContainer lossyNode = NodeContainer (nodes.Get (0), nodes.Get (1));
  NodeContainer losslessNode = NodeContainer (nodes.Get (1), nodes.Get (2));

  PointToPointHelper p2pLossyLink;
  p2pLossyLink.SetDeviceAttribute ("DataRate", StringValue ("1Gbps"));
  p2pLossyLink.SetChannelAttribute ("Delay", StringValue ("20ms"));


  PointToPointHelper p2pLosslessLink;
  p2pLosslessLink.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  p2pLosslessLink.SetChannelAttribute ("Delay", StringValue ("10ms"));

  NetDeviceContainer pppDevices;
  pppDevices.Add(p2pLossyLink.Install(lossyNode));
  pppDevices.Add(p2pLosslessLink.Install(losslessNode));


  /*
   * Setup a Forwarder and associate it with the nodes created.
   */
  CCNxStandardForwarderHelper standardHelper;

  CCNxStackHelper ccnxStack;
  ccnxStack.SetForwardingHelper (standardHelper);

  NfpRoutingHelper nfpHelper;
  nfpHelper.Set ("HelloInterval", TimeValue (Seconds (5)));
  ccnxStack.SetRoutingHelper (nfpHelper);

  /*
   * THis is important sequence, that u install the nodes first and then add interfaces to it.
   */
  ccnxStack.Install (nodes);
  ccnxStack.AddInterfaces (pppDevices);

  /*
   * Setup a global content repository. This is the repository that the producer will server off from
   * and the consumer will randomly request out of.
   */

  Ptr <const CCNxName> prefix = Create <CCNxName> (uri);
  Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository> (prefix,nSize,nCount);

  /*
   * Setup a producer and consumer. In this example they are very identical set of instructions.
   */
  CCNxProducerHelper producerHelper (globalContentRepository);
  ApplicationContainer producerApps = producerHelper.Install (nodes.Get(0));
  producerApps.Start (Seconds (0.0));

/*
 * The consumer will generate interest request at a 250 millisecond interval.
 */

  CCNxConsumerHelper consumerHelper (globalContentRepository);
  consumerHelper.SetAttribute ("RequestInterval", TimeValue (MilliSeconds (5)));
  ApplicationContainer consumerApps = consumerHelper.Install (nodes.Get(2));
  consumerApps.Start (Seconds (1.0));

  // Run the simulator and execute all the events
  consumerApps.Stop (Seconds (nSecsToRun+1)); // Consumer runs for nSecsToRun seconds.
  producerApps.Stop (Seconds (nSecsToRun+2)); // Producer runs for an extra second
  Simulator::Stop (Seconds (nSecsToRun+3)); // Allow an extra second for simulator to complete shutdown.


  Simulator::Run ();

  Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
  standardHelper.PrintForwardingStatistics(trace, nodes.Get(0));
  standardHelper.PrintForwardingStatistics(trace, nodes.Get(1));
  standardHelper.PrintForwardingStatistics(trace, nodes.Get(2));
  standardHelper.PrintForwardingTable(trace, nodes.Get(0));
  standardHelper.PrintForwardingTable(trace, nodes.Get(1));
  standardHelper.PrintForwardingTable(trace, nodes.Get(2));

  Simulator::Destroy ();
}

int
main (int argc, char *argv[])
{
  uint32_t nSize = 124;
  uint32_t nChunks = 1000;
  double nSecsToRun = 10.0;
  std::string repoPrefix = "ccnx:/name=ccnx/name=link/name=loss";

   CommandLine cmd;
   cmd.AddValue ("nSize", "Size of the chunks that the producer will provide", nSize);
   cmd.AddValue ("nChunks", "Number of Chunks that the producer will provide", nChunks);
   cmd.AddValue ("repoPrefix", "uri://<string> representing the repository", repoPrefix);
   cmd.AddValue ("nSecsToRun", "Number of seconds to simulate", nSecsToRun);
   cmd.Parse (argc, argv);

  RunSimulation (repoPrefix,nSize,nChunks,nSecsToRun);
  return 0;
}
