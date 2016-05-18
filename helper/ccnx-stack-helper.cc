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

#include <ns3/ccnx-message-portalfactory.h>
#include "ns3/log.h"
#include "ccnx-stack-helper.h"
#include "ns3/ccnx-forwarder.h"
#include "ns3/node-list.h"
#include "ns3/config.h"

#include "ns3/ccnx-standard-layer3-helper.h"
#include "ns3/ccnx-standard-forwarder-helper.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxStackHelper");
NS_OBJECT_ENSURE_REGISTERED (CCNxStackHelper);

CCNxStackHelper::TraceFilter::TraceFilter (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> namePrefix)
{
  m_stream = stream;
  m_namePrefix = namePrefix;
}

bool
CCNxStackHelper::TraceFilter::Match (Ptr<const CCNxName> name) const
{
  NS_LOG_FUNCTION (this << name);

  bool match = true;
  if (m_namePrefix && !m_namePrefix->IsPrefixOf (*name))
    {
      match = false;
    }

  return match;
}

Ptr<OutputStreamWrapper>
CCNxStackHelper::TraceFilter::GetStream (void) const
{
  return m_stream;
}


CCNxStackHelper::CCNxStackHelper (void)
{
  NS_LOG_FUNCTION (this);

  m_defaultForwardingHelper = new CCNxStandardForwarderHelper ();
  m_defaultLayer3Helper = new CCNxStandardLayer3Helper ();

  m_routingHelper = NULL;
  m_forwardingHelper = m_defaultForwardingHelper;
  m_layer3Helper = m_defaultLayer3Helper;
  m_ccnxEnabled = true;

  m_traceFilters.reserve (NodeList::GetNNodes ());
  for (int i = 0; i < NodeList::GetNNodes (); ++i)
    {
      m_traceFilters[i] = new TraceFilterListType ();
    }
}

CCNxStackHelper::~CCNxStackHelper (void)
{
  NS_LOG_FUNCTION (this);
  for (TraceFilterVectorType::iterator i = m_traceFilters.begin (); i != m_traceFilters.end (); ++i)
    {
      delete *i;
    }

  delete m_defaultForwardingHelper;
  delete m_defaultLayer3Helper;
}

void
CCNxStackHelper::SetRoutingHelper (const CCNxRoutingHelper & routingHelper)
{
  m_routingHelper = &routingHelper;
}

void
CCNxStackHelper::SetLayer3Helper (const CCNxLayer3Helper & layer3Helper)
{
  m_layer3Helper = &layer3Helper;
}

void
CCNxStackHelper::SetForwardingHelper (const CCNxForwardingHelper & forwardingHelper)
{
  m_forwardingHelper = &forwardingHelper;
}


void CCNxStackHelper::Install (NodeContainer c) const
{
  NS_LOG_FUNCTION (this);

  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install ( (Ptr<Node>) * i );
    }
}

void
CCNxStackHelper::Install (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);

  m_layer3Helper->Install (node);
  m_forwardingHelper->Install (node);

  CreateAndAggregateObjectFromTypeId (node, "ns3::ccnx::CCNxMessagePortalFactory");

  Ptr<CCNxL3Protocol> ccnx = node->GetObject<CCNxL3Protocol> ();

  Ptr<CCNxMessagePortalFactory> mpf = node->GetObject<CCNxMessagePortalFactory> ();
  NS_ASSERT_MSG (mpf, "Got null CCNxMessagePortalFactory from node " << node);
  mpf->SetCCNx (ccnx);
  NS_LOG_DEBUG ("Node " << node << " Installed CCNxMessagePortalFactory " << mpf);

  // Install the routing protocol on the node
  if (m_routingHelper)
    {
      Ptr<CCNxRoutingProtocol> routing = m_routingHelper->Create (node);
      ccnx->SetRoutingProtocol (routing);
    }
}

void
CCNxStackHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
  NS_LOG_FUNCTION (node << typeId);

  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> protocol = factory.Create<Object> ();
  node->AggregateObject (protocol);
}

void
CCNxStackHelper::AddInterfaces (const NetDeviceContainer &c)
{
  NS_LOG_FUNCTION_NOARGS ();
  for (uint32_t i = 0; i < c.GetN (); ++i)
    {
      Ptr<NetDevice> device = c.Get (i);

      Ptr<Node> node = device->GetNode ();
      NS_ASSERT_MSG (node, "NetDevice is not not associated with any node");

      Ptr<CCNxL3Protocol> ccnx = node->GetObject<CCNxL3Protocol> ();
      NS_ASSERT_MSG (ccnx, "CCNxL3Protocol not found on node " << node->GetSystemId ());

      int32_t interface = ccnx->AddInterface (device);

      Ptr<CCNxL3Interface> ccnxIf = ccnx->GetInterface (interface);
      ccnxIf->SetUp ();

      NS_LOG_INFO ("CCNx added device " << device->GetIfIndex ());
    }
}

void
CCNxStackHelper::EnableAsciiCCNxInternal (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> namePrefix, Ptr<Node> node)
{
  Packet::EnablePrinting ();
  m_traceFilters[node->GetId ()]->push_back (TraceFilter (stream, namePrefix));


  // We use a string representation of the attribute to bind to.  Because the user has set the string
  // name of the L3 protocol they want to use (or it defaulted to "ns3::ccnx::CCNxStandardLayer3"), this
  // will magically call the right Attribute and set the function to use for tracing.
  std::ostringstream oss;

  oss.str ("");
  oss << "/NodeList/" << node->GetId () << "/$" << m_layer3Helper->GetLayer3TypeId ().GetName () << "/Drop";
  Config::Connect (oss.str (), MakeCallback (&CCNxStackHelper::AsciiDropTraceCallback, this));
  oss.str ("");
  oss << "/NodeList/" << node->GetId () << "/$" << m_layer3Helper->GetLayer3TypeId ().GetName () << "/Tx";
  Config::Connect (oss.str (), MakeCallback (&CCNxStackHelper::AsciiTxTraceCallback, this));
  oss.str ("");
  oss << "/NodeList/" << node->GetId () << "/$" << m_layer3Helper->GetLayer3TypeId ().GetName () << "/Rx";
  Config::Connect (oss.str (), MakeCallback (&CCNxStackHelper::AsciiRxTraceCallback, this));
}

static void
AsciiTrace (const char *label, std::string &context, Ptr<OutputStreamWrapper> stream, const CCNxL3Protocol *ccnx, Ptr<const Packet> ns3Packet,
            Ptr<const CCNxPacket> ccnxPacket, uint32_t interface)
{
  // The packet trace will print the CCNx headers, so don't seem to need to do anything with ccnxPacket parameter
  *stream->GetStream () << label << Simulator::Now ().GetSeconds () << " " << context << " " << ccnx->GetNode ()->GetId ()
                        << "(" << interface << ") "
                        << *ns3Packet << std::endl;
}

void
CCNxStackHelper::AsciiTxTraceCallback (std::string context, const CCNxL3Protocol *ccnx, Ptr<const Packet> ns3Packet,
                                       Ptr<const CCNxPacket> ccnxPacket, uint32_t interface)
{
  TraceFilterListType *list = m_traceFilters[ccnx->GetNode ()->GetId ()];
  for (std::list<TraceFilter>::const_iterator i = list->cbegin (); i != list->cend (); ++i)
    {
      if (i->Match (ccnxPacket->GetMessage ()->GetName ()))
        {
          Ptr<OutputStreamWrapper> stream = i->GetStream ();
          AsciiTrace ("t ", context, stream, ccnx, ns3Packet, ccnxPacket, interface);
        }
    }
}

void
CCNxStackHelper::AsciiRxTraceCallback (std::string context, const CCNxL3Protocol *ccnx, Ptr<const Packet> ns3Packet,
                                       Ptr<const CCNxPacket> ccnxPacket, uint32_t interface)
{
  TraceFilterListType *list = m_traceFilters[ccnx->GetNode ()->GetId ()];
  for (std::list<TraceFilter>::const_iterator i = list->cbegin (); i != list->cend (); ++i)
    {
      if (i->Match (ccnxPacket->GetMessage ()->GetName ()))
        {
          Ptr<OutputStreamWrapper> stream = i->GetStream ();
          AsciiTrace ("r ", context, stream, ccnx, ns3Packet, ccnxPacket, interface);
        }
    }
}

void
CCNxStackHelper::AsciiDropTraceCallback (std::string context, const CCNxL3Protocol *ccnx, Ptr<const Packet> ns3Packet,
                                         Ptr<const CCNxPacket> ccnxPacket, uint32_t interface, CCNxL3Protocol::DropReason reason)
{
  TraceFilterListType *list = m_traceFilters[ccnx->GetNode ()->GetId ()];
  for (std::list<TraceFilter>::const_iterator i = list->cbegin (); i != list->cend (); ++i)
    {
      if (i->Match (ccnxPacket->GetMessage ()->GetName ()))
        {
          Ptr<OutputStreamWrapper> stream = i->GetStream ();
          AsciiTrace ("d ", context, stream, ccnx, ns3Packet, ccnxPacket, interface);
        }
    }
}
