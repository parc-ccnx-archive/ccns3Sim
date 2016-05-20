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
#include <iomanip>
#include "ns3/log.h"
#include "ns3/ccnx-consumer.h"

using namespace ns3;
using namespace ns3::ccnx;
NS_LOG_COMPONENT_DEFINE ("CCNxConsumer");
NS_OBJECT_ENSURE_REGISTERED (CCNxConsumer);

static bool printConsStatsHeader = 1;
TypeId
CCNxConsumer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxApplication::CCNxConsumer")
    .SetParent<CCNxApplication> ()
    .SetGroupName ("CCNx")
    .AddConstructor < CCNxConsumer > ()
    .AddAttribute ("RequestInterval",
                   "delay between successive Interests",
                   TimeValue (MilliSeconds (0)),
                   MakeTimeAccessor (&CCNxConsumer::m_requestInterval),
                   MakeTimeChecker ());
  return tid;
}

CCNxConsumer::CCNxConsumer ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_consumerPortal = Ptr<CCNxPortal> (0);
  m_goodInterestsSent = 0;
  m_goodContentReceived = 0;
  m_interestProcessFails = 0;
  m_contentProcessFails = 0;
  m_count = 0;
  m_sum = 0;
  m_sumSquare = 0;
}

CCNxConsumer::~CCNxConsumer ()
{
  // empty
}


void
CCNxConsumer::StartApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_consumerPortal = CCNxPortal::CreatePortal (
      GetNode (), TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory"));
  m_consumerPortal->SetRecvCallback (
    MakeCallback (&CCNxConsumer::ReceiveCallback, this));

  m_requestIntervalTimer = Timer (Timer::REMOVE_ON_DESTROY);
  m_requestIntervalTimer.SetFunction (&CCNxConsumer::GenerateTraffic, this);
  m_requestIntervalTimer.SetDelay (m_requestInterval);
  m_requestIntervalTimer.Schedule ();
}

void
CCNxConsumer::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_requestIntervalTimer.IsRunning ())
    {
      m_requestIntervalTimer.Cancel ();
    }
  m_consumerPortal->Close ();
  if (m_outstandingRequests.size ())
    {
      NS_LOG_ERROR (
        "All the outstanding requests not honored by forwarder " << m_outstandingRequests.size () << " more left behind");
    }
  else
    {
      NS_LOG_INFO (
        "All interest expressed by consumer have been honored by the network");
    }
  CCNxConsumer::ShowStatistics ();
}

void
CCNxConsumer::SetContentRepository (
  Ptr<CCNxContentRepository> repositoryPtr)
{
  NS_LOG_FUNCTION (this << repositoryPtr);
  m_globalContentRepositoryPrefix = repositoryPtr;
}

void
CCNxConsumer::InsertOutStandingInterest (Ptr<const CCNxName> interest)
{
  NS_LOG_FUNCTION (this << interest);
#if 0
  m_outstandingRequests.insert (interest);
#else
  m_outstandingRequests[interest].txTime = (uint64_t) Simulator::Now ().GetMilliSeconds ();
#endif
}

void
CCNxConsumer::RemoveOutStandingInterest (Ptr<const CCNxName> interest)
{
  NS_LOG_FUNCTION (this << interest);
#if 0
  m_outstandingRequests.erase (interest);
#else
  uint64_t packet_latency = (uint64_t) Simulator::Now ().GetMilliSeconds () - m_outstandingRequests[interest].txTime ;
  m_sum += packet_latency;
  m_sumSquare += packet_latency * packet_latency;
  m_outstandingRequests.erase (interest);
#endif
}

bool
CCNxConsumer::FindOutStandingInterest (Ptr<const CCNxName> interest)
{
  NS_LOG_FUNCTION (this << interest);
  return m_outstandingRequests.find (interest) != m_outstandingRequests.end ();
}

void
CCNxConsumer::ReceiveCallback (Ptr<CCNxPortal> portal)
{
  NS_LOG_FUNCTION (this << portal);
  Ptr<CCNxPacket> packet;
  while ((packet = portal->Recv ()))
    {
      NS_LOG_DEBUG (
        "CCNxConsumer:Received content response " << *packet << " packet dump");
      m_goodContentReceived++;
      if (packet->GetMessage ()->GetMessageType ()
          == CCNxMessage::ContentObject)
        {
          Ptr<const CCNxName> name = packet->GetMessage ()->GetName ();
          if (FindOutStandingInterest (name))
            {
              NS_LOG_INFO (
                "CCNxConsumer:Received content back for Node " << GetNode ()->GetId () << *name);
              RemoveOutStandingInterest (name);
            }
          else
            {
              m_contentProcessFails++;
              NS_LOG_ERROR (
                "CCNxConsumer:Received wrong content back for Node " << GetNode ()->GetId () << *name);
            }
        }
      else
        {
          m_contentProcessFails++;
          NS_LOG_ERROR ("CCNxConsumer:Bad packet type received " << *packet);
        }
    }
}

void
CCNxConsumer::GenerateTraffic ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<const CCNxName> name = m_globalContentRepositoryPrefix->GetRandomName ();
  m_count++;
  if (name)
    {
      Ptr<CCNxInterest> interest = Create<CCNxInterest> (name);
      Ptr<CCNxPacket> packet = CCNxPacket::CreateFromMessage (interest);
      m_consumerPortal->Send (packet);
      InsertOutStandingInterest (name);
      m_requestIntervalTimer.Schedule (m_requestInterval);
      NS_LOG_DEBUG (
        "CCNxConsumer:Sending interest request" << *packet << " packet dump");
      m_goodInterestsSent++;
    }
  else
    {
      NS_LOG_ERROR ("Bad Interest Generated");
      m_interestProcessFails++;
    }
}


void
CCNxConsumer::ShowStatistics ()
{

  Ptr <Node> node = CCNxConsumer::GetNode ();
  if (printConsStatsHeader)
    {
      std::cout << std::endl <<  "Consumer " << " Interest " << "Content   " << \
    		  " Missing   " << " Bad        " << "Average   " << "Std Dev    " \
			  << "Total    " <<" Repository" << std::endl;
      std::cout << "Node Id :" << " Sent    :" << "Received  :" \
    		  << "Interests :" << "Packets   :" << "Delay(Ms):" <<  "Delay(Ms) :" \
			  << "Count     :" << "Prefix   " << std::endl;
      printConsStatsHeader = 0;
    }
  double average = 0.0;
  double stdev = 0.0;

  if (m_count > 0) {
	  average = m_sum / m_count;

	  // This comes from https://en.wikipedia.org/wiki/Standard_deviation#Identities_and_mathematical_properties
	  if (m_count > 2) {
		  double k = sqrt(m_count / (m_count - 1));

		  double variance = (m_sumSquare / m_count) - (average * average);
		  stdev = k * sqrt(variance);
	  }
  }
  std::cout << std::setw (10) << std::left << node->GetId ();
  std::cout << std::setw (10) << std::left  << m_goodInterestsSent;
  std::cout << std::setw (12) << std::left << m_goodContentReceived;
  std::cout << std::setw (12) << std::left << m_goodInterestsSent - m_goodContentReceived;
  std::cout << std::setw (8) << std::left << m_contentProcessFails + m_interestProcessFails;
  std::cout << std::setw (10) << std::left << average;
  std::cout << std::setw (11) << std::left << stdev;
  std::cout << std::setw (10) << std::left << m_count;
  std::cout << *m_globalContentRepositoryPrefix->GetRepositoryPrefix () << std::endl;

}


