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
#include "ns3/assert.h"
#include "ns3/ccnx-producer.h"

using namespace ns3;
using namespace ns3::ccnx;
NS_LOG_COMPONENT_DEFINE ("CCNxProducer");
NS_OBJECT_ENSURE_REGISTERED (CCNxProducer);

static bool printProdStatsHeader = 1;
TypeId CCNxProducer::GetTypeId (void)
{
  static TypeId tid =
    TypeId ("ns3::ccnx::CCNxApplication::CCNxProducer")
    .SetParent<CCNxApplication> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxProducer> ();
  return tid;
}


CCNxProducer::CCNxProducer ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_goodInterestsReceived = 0;
  m_goodContentServed = 0;
  m_interestProcessFails = 0;
  m_contentProcessFails = 0;
}

CCNxProducer::~CCNxProducer ()
{
  // empty
}


void CCNxProducer::StartApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT_MSG ( m_globalContentRepositoryPrefix, "Must setup a repository first" );
  Ptr<const CCNxName> repositoryPrefix =
    m_globalContentRepositoryPrefix->GetRepositoryPrefix ();
  NS_LOG_DEBUG ("Starting Producer on  " << *repositoryPrefix);

  m_producerPortal = CCNxPortal::CreatePortal (GetNode (),
                                               TypeId::LookupByName ("ns3::ccnx::CCNxMessagePortalFactory"));
  m_producerPortal->RegisterAnchor (repositoryPrefix);
  m_producerPortal->SetRecvCallback (
    MakeCallback (&CCNxProducer::ReceiveCallback, this));
}

void CCNxProducer::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<const CCNxName> repositoryPrefix =
    m_globalContentRepositoryPrefix->GetRepositoryPrefix ();
  NS_LOG_DEBUG ("Stopping Producer on prefix " << *repositoryPrefix);
  m_producerPortal->UnregisterAnchor (repositoryPrefix);
  m_producerPortal->Close ();
  m_producerPortal = Ptr<CCNxPortal> (0);
  CCNxProducer::ShowStatistics ();
}

void
CCNxProducer::SetContentRepository (
  Ptr<CCNxContentRepository> repositoryPtr)
{
  NS_LOG_FUNCTION (this << repositoryPtr);
  m_globalContentRepositoryPrefix = repositoryPtr;
}

void
CCNxProducer::ReceiveCallback (Ptr<CCNxPortal> portal)
{
  NS_LOG_FUNCTION (this << portal);
  Ptr<CCNxPacket> packet;

  while ((packet = portal->Recv ()))
    {
      if (packet->GetMessage ()->GetMessageType () == CCNxMessage::Interest)
        {
          m_goodInterestsReceived++;
          NS_LOG_DEBUG (
            "CCNxProducer::Received request" << *packet << " packet dump");
          Ptr<const CCNxName> name = packet->GetMessage ()->GetName ();
          Ptr<CCNxContentObject> contentResponse =
            m_globalContentRepositoryPrefix->GetContentObject (name);
          if (contentResponse)
            {
              m_goodContentServed++;
              Ptr<CCNxPacket> response = CCNxPacket::CreateFromMessage (
                  contentResponse);
              portal->Send (response);
              NS_LOG_INFO (
                "CCNxProducer::Sending Response for request " << *name);
            }
          else
            {
              m_contentProcessFails++;
              NS_LOG_ERROR (
                "CCNxProducer::Producer on node" << GetNode ()->GetId () << "got wrong prefix request " << *name);
            }
        }
      else
        {
          // Cant deal with packet not an interest
          m_interestProcessFails++;
          NS_LOG_ERROR ("CCNxProducer::Bad packet type received " << *packet);
        }
    }
}

void
CCNxProducer::ShowStatistics ()
{
  Ptr <Node> node = CCNxProducer::GetNode ();
  if (printProdStatsHeader)
    {
      std::cout << std::endl <<  "Producer " << " Interest  " << "Content  " << "Missing    " << " Bad        " << "Repository" << std::endl;
      std::cout << "Node Id :" << " Received :" << " Sent   :" << "Content   :" << "Packets    :" << " Prefix   " << std::endl;
      printProdStatsHeader = 0;
    }
  std::cout << std::setw (10) << std::left << node->GetId ();
  std::cout << std::setw (10) << std::left  << m_goodInterestsReceived;
  std::cout << std::setw (10) << std::left << m_goodContentServed;
  std::cout << std::setw (12) << std::left << m_goodInterestsReceived - m_goodContentServed;
  std::cout << std::setw (10) << std::left << m_contentProcessFails + m_interestProcessFails;
  std::cout << *m_globalContentRepositoryPrefix->GetRepositoryPrefix () << std::endl;
}

