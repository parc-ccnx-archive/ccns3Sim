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
 * Uses a single broadcast domain with a single Producer and a single Consumer.
 *
 * Producer      Consumer
 *  |              |
 * n0 ----------- n1
 *       1Gbps
 *       10 millisecond
 *       THe Delay is amount of time it takes to enter and leave a hub that is connecting these devices.
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

static void
RunSimulation (void)
{
  Time::SetResolution (Time::NS);

  NodeContainer nodes;
  nodes.Create (2);

  CsmaHelper etherNet;
  etherNet.SetChannelAttribute ("DataRate", DataRateValue (DataRate (1000000000)));
  etherNet.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  etherNet.SetDeviceAttribute ("Mtu", UintegerValue (1500));

  NetDeviceContainer devices = etherNet.Install (nodes);

  /*
   * Setup a Forwarder and associate it with the nodes created.
   */
  CCNxStackHelper ccnxStack;
  CCNxStandardForwarderHelper standardHelper;
  ccnxStack.SetForwardingHelper (standardHelper);

  NfpRoutingHelper nfpHelper;
  nfpHelper.Set ("HelloInterval", TimeValue (Seconds (5)));
  ccnxStack.SetRoutingHelper (nfpHelper);
  ccnxStack.Install (nodes);
  ccnxStack.AddInterfaces (devices);

  /*
   * Setup a global content repository. This is the repository that the producer will server off from
   * and the consumer will randomly request out of.
   */

  Ptr<const CCNxName> prefix = Create<CCNxName> (
      "ccnx:/name=test/name=packet/name=generator");
  uint32_t size = 124;
  uint32_t count = 100;
  Ptr<CCNxContentRepository> globalContentRepository = Create<
      CCNxContentRepository> (prefix, size, count);

  /*
   * Setup a producer and consumer. In this example they are very identical set of instructions.
   */
  CCNxProducerHelper producerHelper (globalContentRepository);
  ApplicationContainer producerApps = producerHelper.Install (nodes.Get (0));
  producerApps.Start (Seconds (0.0));

  /*
   * The consumer will generate interest request at a 250 millisecond interval.
   */

  CCNxConsumerHelper consumerHelper (globalContentRepository);
  consumerHelper.SetAttribute ("RequestInterval",
                               TimeValue (MilliSeconds (5)));
  ApplicationContainer consumerApps = consumerHelper.Install (nodes.Get (1));
  consumerApps.Start (Seconds (2.0));


  etherNet.EnablePcapAll ("ccnx-tp-generator");

  // Run the simulator and execute all the events
  producerApps.Stop (Seconds (12.0));
  consumerApps.Stop (Seconds (10.0));

  Simulator::Stop (Seconds (13));

  Simulator::Run ();
  Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
  standardHelper.PrintForwardingTable(trace, nodes.Get(0));
  standardHelper.PrintForwardingTable(trace, nodes.Get(1));
  standardHelper.PrintForwardingStatistics(trace, nodes.Get(0));
   standardHelper.PrintForwardingStatistics(trace, nodes.Get(1));

  Simulator::Destroy ();
}

int
main (int argc, char *argv[])
{
  RunSimulation ();
  return 0;
}
