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
//       n0    n1
//       |     |
//       =======
//         LAN
//

#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/ccns3Sim-module.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("ccnx-multi-producer-consumer");

#define DEBUG_HIGH 0
#define DEBUG_LOW 0
typedef std::vector < Ptr <CCNxContentRepository> > contentRepoList;

static void
RunSimulation (uint32_t nPrefixes)
{
  NS_LOG_INFO ("Number of Producers are = " << nPrefixes );

#if DEBUG_HIGH
  LogComponentEnable ("CCNxStandardLayer3", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
  LogComponentEnable ("NfpRoutingProtocol", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxStandardForwarder", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxPacket", (LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxConnectionDevice", (LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxCodecFixedHeader", (LogLevel) (LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxForwarder", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxMessagePortal", (LogLevel) (LOG_LEVEL_INFO | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxProducer", (LogLevel)( LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
  LogComponentEnable ("CCNxConsumer", (LogLevel)(LOG_LEVEL_DEBUG | LOG_PREFIX_ALL));
#endif
#if DEBUG_LOW
  LogComponentEnable ("CCNxStandardLayer3", (LogLevel) (LOG_LEVEL_ERROR | LOG_LEVEL_WARN ));
  LogComponentEnable ("NfpRoutingProtocol", (LogLevel) (LOG_LEVEL_ERROR | LOG_LEVEL_WARN ));
  LogComponentEnable ("CCNxStandardForwarder", (LogLevel) (LOG_LEVEL_ERROR | LOG_LEVEL_WARN ));
  LogComponentEnable ("CCNxPacket", (LogLevel) (LOG_LEVEL_ERROR | LOG_LEVEL_WARN ));
  LogComponentEnable ("CCNxConnectionDevice", (LogLevel) (LOG_LEVEL_ERROR | LOG_LEVEL_WARN));
  LogComponentEnable ("CCNxCodecFixedHeader", (LogLevel) (LOG_LEVEL_ERROR | LOG_LEVEL_WARN));
  LogComponentEnable ("CCNxForwarder", (LogLevel) (LOG_LEVEL_ERROR | LOG_LEVEL_WARN));
  LogComponentEnable ("CCNxMessagePortal", (LogLevel) (LOG_LEVEL_ERROR | LOG_LEVEL_WARN));
  LogComponentEnable ("CCNxProducer", (LogLevel)( LOG_LEVEL_ERROR | LOG_LEVEL_WARN));
  LogComponentEnable ("CCNxConsumer", (LogLevel)(LOG_LEVEL_ERROR | LOG_LEVEL_WARN));
#endif

  Time::SetResolution (Time::NS);
  NodeContainer nodes;
  nodes.Create (2);

  CsmaHelper etherNet;
  etherNet.SetChannelAttribute ("DataRate", DataRateValue (DataRate (1000000000))); // 1Gbps
  etherNet.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (1)));
  etherNet.SetDeviceAttribute ("Mtu", UintegerValue (1504));
  NetDeviceContainer devices = etherNet.Install (nodes);

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
  ccnxStack.AddInterfaces (devices);

  const std::string pre = "ccnx:/name=multi/name=producer";
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
      sprintf (fix, "name=size%d-count%d", size, count);
      globalRepoList.push_back (Create <CCNxContentRepository> (Create <CCNxName> (pre + fix),size,count));
    }

  for (contentRepoList::iterator ii = globalRepoList.begin (); ii != globalRepoList.end (); ii++)
    {
      CCNxProducerHelper producerHelper (*ii);
      ApplicationContainer producerApps = producerHelper.Install (nodes.Get (0));
      producerApps.Start (Seconds (0.0));
      producerApps.Stop (Seconds (105.0));
    }
  uint32_t offset = 0;
  for (contentRepoList::iterator ii = globalRepoList.begin (); ii != globalRepoList.end (); ii++)
  {
      CCNxConsumerHelper consumerHelper (*ii);
      consumerHelper.SetAttribute ("RequestInterval", TimeValue (MilliSeconds (250)));
      ApplicationContainer consumerApps = consumerHelper.Install (nodes.Get (1));
      consumerApps.Start (Seconds (1.0 + offset));
      consumerApps.Stop (Seconds (100.0));
      offset++;
      offset %= globalRepoList.size() ;
    }

  Simulator::Stop (Seconds (110));

  etherNet.EnablePcapAll ("ccnx-multi-producer-consumer", false);
  // Run the simulator and execute all the events
  Simulator::Run ();
  Simulator::Destroy ();

}

int
main (int argc, char *argv[])
{
  uint32_t nPrefixes = 1;
  CommandLine cmd;
  cmd.AddValue ("nPrefixes", "Number of Prefixes to simulate", nPrefixes);
  cmd.Parse (argc, argv);

  RunSimulation (nPrefixes);
  return 0;
}
