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
 * This is the same as ccnx-consumer-producer, except we illustrate how to
 * set the processing time delays (layer delays) in all the different components.
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
RunSimulation (void)
{
  Time::SetResolution (Time::NS);

  /*
   * Uses a point-to-point topology.
   *
   * producer  consumer
   *  |         |
   * n0 ------ n1
   *     5Mbps
   *     2ms
   */

  /*
   * Step-1 Create Nodes per above topology.
   */
  NodeContainer nodes;
  nodes.Create (2);

/*
 * Step-2 Create a L1 topology and install them on nodes created from Step-1
 */
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes.Get (0), nodes.Get (1));

  /*
   * Setup a Forwarder and associate it with the nodes created.
   */
  CCNxStackHelper ccnxStack;

  // Set the processing time in the CCNxStandardLayer3
  // Not supported yet


  CCNxStandardForwarderHelper forwarderHelper;

  Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
  forwarderHelper.PrintForwardingStatisticsAllNodesWithInterval(Seconds(5), trace);

  // processing time at the forwarder is 10 usec + 1 nsec per byte.
  // It has a parallism of 2, so you will get 2 packets through per service time.
  forwarderHelper.SetLayerDelayConstant (MicroSeconds (10));
  forwarderHelper.SetLayerDelaySlope (NanoSeconds (1));
  forwarderHelper.SetLayerDelayServers (2);

  // Set the PIT processing times (linear function of name bytes)
  CCNxStandardPitFactory pitFactory;
  pitFactory.SetLayerDelayConstant (MicroSeconds (10));
  pitFactory.SetLayerDelaySlope (NanoSeconds (3));
  pitFactory.SetLayerDelayServers (4);
  forwarderHelper.SetPitFactory (pitFactory);

  // Set the FIB processing times (linear function of name components until match found)
  CCNxStandardFibFactory fibFactory;
  fibFactory.SetLayerDelayConstant (MicroSeconds (8));
  fibFactory.SetLayerDelaySlope (NanoSeconds (7));
  fibFactory.SetLayerDelayServers (2);
  forwarderHelper.SetFibFactory (fibFactory);

  // Set the CS processing time (linear function of ContentObject bytes returned)


  ccnxStack.SetForwardingHelper (forwarderHelper);


  NfpRoutingHelper nfpHelper;
  nfpHelper.Set ("HelloInterval", TimeValue (Seconds (5)));
  ccnxStack.SetRoutingHelper (nfpHelper);

  ccnxStack.Install (nodes);
  ccnxStack.AddInterfaces (devices);

  /*
   * Setup a global content repository. This is the repository that the producer will server off from
   * and the consumer will randomly request out of.
   */

  Ptr <const CCNxName> prefix = Create <CCNxName> ("ccnx:/name=simple/name=producer/name=consumer");
  uint32_t size = 124;
  uint32_t count = 10;
  Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository> (prefix,size,count);

  /*
   * Setup a producer and consumer. In this example they are very identical set of instructions.
   */
  CCNxProducerHelper producerHelper (globalContentRepository);
  ApplicationContainer producerApps = producerHelper.Install (nodes.Get (0));
  producerApps.Start (Seconds (1.0));

/*
 * The consumer will generate interest request at a 250 millisecond interval.
 */

  CCNxConsumerHelper consumerHelper (globalContentRepository);
  consumerHelper.SetAttribute ("RequestInterval", TimeValue (MilliSeconds (250)));
  ApplicationContainer consumerApps = consumerHelper.Install (nodes.Get (1));
  consumerApps.Start (Seconds (2.0));

  producerApps.Stop (Seconds (10.0));
  consumerApps.Stop (Seconds (10.0));

  // Run the simulator and execute all the events
  producerApps.Stop (Seconds (10.0));
  consumerApps.Stop (Seconds (10.0));
  Simulator::Stop (Seconds (12));


  Simulator::Run ();
  Simulator::Destroy ();
}

int
main (int argc, char *argv[])
{

  RunSimulation ();
  return 0;
}
