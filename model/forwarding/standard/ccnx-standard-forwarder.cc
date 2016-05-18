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

#include <iomanip>

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ccnx-standard-forwarder.h"
#include "ns3/ccnx-l3-protocol.h"
#include "ns3/ccnx-routing-error.h"

#include "ns3/ccnx-standard-content-store.h"
#include "ns3/ccnx-standard-pit.h"
#include "ns3/ccnx-standard-fib.h"
#include "ns3/ccnx-standard-content-store-factory.h"
#include "ns3/ccnx-standard-pit-factory.h"
#include "ns3/ccnx-standard-fib-factory.h"

#include "ns3/integer.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxStandardForwarder");
NS_OBJECT_ENSURE_REGISTERED (CCNxStandardForwarder);

static const Time _defaultLayerDelayConstant = MicroSeconds (1);
static const Time _defaultLayerDelaySlope = Seconds (0);
static unsigned _defaultLayerDelayServers = 1;

static ObjectFactory
GetDefaultPitFactory ()
{
  static CCNxStandardPitFactory factory;
  return factory;
}

static ObjectFactory
GetDefaultFibFactory ()
{
  static CCNxStandardFibFactory factory;
  return factory;
}

static ObjectFactory
GetDefaultContentStoreFactory ()
{
  // This will have a TypeId of 0 - Default value is no content store
  static ObjectFactory factory;
  return factory;
}

TypeId
CCNxStandardForwarder::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxStandardForwarder")
    .SetParent<CCNxForwarder> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxStandardForwarder> ()
    .AddAttribute ("PitFactory", "The ObjectFactory to create a CCNxPit.",
                   ObjectFactoryValue (GetDefaultPitFactory ()),
                   MakeObjectFactoryAccessor (&CCNxStandardForwarder::m_pitFactory),
                   MakeObjectFactoryChecker ())
    .AddAttribute ("FibFactory", "The type of FIB to use.",
                   ObjectFactoryValue (GetDefaultFibFactory ()),
                   MakeObjectFactoryAccessor (&CCNxStandardForwarder::m_fibFactory),
                   MakeObjectFactoryChecker ())
   .AddAttribute ("ContentStoreFactory", "The type of Content Store to use.",
		   ObjectFactoryValue (GetDefaultContentStoreFactory ()),
		   MakeObjectFactoryAccessor (&CCNxStandardForwarder::m_contentStoreFactory),
		   MakeObjectFactoryChecker ())
   .AddAttribute ("LayerDelayConstant", "The amount of constant layer delay",
                   TimeValue (_defaultLayerDelayConstant),
                   MakeTimeAccessor (&CCNxStandardForwarder::m_layerDelayConstant),
                   MakeTimeChecker ())
    .AddAttribute ("LayerDelaySlope", "The slope of the layer delay (in terms of packet bytes)",
                   TimeValue (_defaultLayerDelaySlope),
                   MakeTimeAccessor (&CCNxStandardForwarder::m_layerDelaySlope),
                   MakeTimeChecker ())
    .AddAttribute ("LayerDelayServers", "The number of servers for the layer delay input queue",
                   IntegerValue (_defaultLayerDelayServers),
                   MakeIntegerAccessor (&CCNxStandardForwarder::m_layerDelayServers),
                   MakeIntegerChecker<unsigned> ())
  ;
  return tid;
}


CCNxStandardForwarder::CCNxStandardForwarder ()
  : m_pitFactory (GetDefaultPitFactory ()), m_fibFactory (GetDefaultFibFactory ()),
    m_contentStoreFactory (GetDefaultContentStoreFactory ()),
  m_layerDelayConstant (_defaultLayerDelayConstant), m_layerDelaySlope (_defaultLayerDelaySlope),
  m_layerDelayServers (_defaultLayerDelayServers)
{
  memset(&m_forwarderStats, 0, sizeof(m_forwarderStats));
}

CCNxStandardForwarder::~CCNxStandardForwarder ()
{
  // empty (use DoDispose)
}

void
CCNxStandardForwarder::DoDispose (void)
{

}


void
CCNxStandardForwarder::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);

  m_pit = m_pitFactory.Create<CCNxPit> ();
  m_pit->SetReceiveInterestCallback (MakeCallback (&CCNxStandardForwarder::PitReceiveInterestCallback, this));
  m_pit->SetSatisfyInterestCallback (MakeCallback (&CCNxStandardForwarder::PitSatisfyInterestCallback, this));
  m_pit->Initialize ();

  m_fib = m_fibFactory.Create<CCNxFib> ();
  m_fib->SetLookupCallback (MakeCallback (&CCNxStandardForwarder::FibLookupCallback, this));
  m_fib->Initialize ();

  if (m_contentStoreFactory.GetTypeId().GetUid())
    {
    m_contentStore = m_contentStoreFactory.Create<CCNxContentStore> ();
    m_contentStore->SetMatchInterestCallback (MakeCallback (&CCNxStandardForwarder::ContentStoreMatchInterestCallback, this));
    m_contentStore->SetAddContentObjectCallback (MakeCallback (&CCNxStandardForwarder::ContentStoreAddContentObjectCallback, this));
    m_contentStore->Initialize ();
    }


  m_inputQueue = Create<DelayQueueType> (m_layerDelayServers,
                                         MakeCallback (&CCNxStandardForwarder::GetServiceTime, this),
                                         MakeCallback (&CCNxStandardForwarder::ServiceInputQueue, this));
}

Time
CCNxStandardForwarder::GetServiceTime (Ptr<CCNxStandardForwarderWorkItem> item)
{
  Time delay = m_layerDelayConstant + m_layerDelaySlope * item->GetPacket ()->GetFixedHeader ()->GetPacketLength ();
  return delay;
}

/*
 * Called after the input queue delay from RouteInput() and RouteOutput().
 *
 * Pass the work item on to the next stage of processing in the PIT
 */
void
CCNxStandardForwarder::ServiceInputQueue (Ptr<CCNxStandardForwarderWorkItem> item)
{
  NS_LOG_FUNCTION (this << item->GetPacket () << item->GetIngressConnection () << item->GetEgressConnection ());

  switch (item->GetPacket ()->GetFixedHeader ()->GetPacketType ())
    {
    case CCNxFixedHeaderType_Interest:
      {
	m_forwarderStats.interestsToPit++;
        m_pit->ReceiveInterest (item);
        break;
      }
    case CCNxFixedHeaderType_Object:
      {
	m_forwarderStats.contentObjectsToPit++;
        m_pit->SatisfyInterest (item);
        break;
      }

    case CCNxFixedHeaderType_InterestReturn:
      {
        NS_ASSERT_MSG (false, "Unsupported packetType");
        m_forwarderStats.dropUnsupportedPacketType++;
        break;
      }

    default:
      {
        NS_ASSERT_MSG (false, "Unsupported packetType");
        m_forwarderStats.dropUnsupportedPacketType++;
        break;
      }
    }

  // If it did not get passed to the pit or other table, this is the same as dropping it.
}

void
CCNxStandardForwarder::FinishRouteLookup (Ptr<CCNxStandardForwarderWorkItem> item, Ptr<CCNxConnectionList> egressConnections)
{

  m_forwarderStats.packetsOut++;

  // If we were passed a NULL egressConnections, create an empty list
  if (!egressConnections)
    {
      egressConnections = Create<CCNxConnectionList> ();
    }

  if (item->GetEgressConnection ())
    {
      // User specified an egressFromUser connection, so use that.
      item->SetRouteError (CCNxRoutingError::CCNxRoutingError_NoError);
      NS_LOG_DEBUG (": user has overridden fib lookup");
      egressConnections->clear ();
      egressConnections->push_back (item->GetEgressConnection ());
    }


  //skip this code unless debugging enabled
  if (g_log.IsEnabled(LOG_LEVEL_DEBUG))
    {
      std::string sourceConnString;
      if (item->GetIngressConnection())
	{
	  sourceConnString = static_cast<std::ostringstream*>( &(std::ostringstream() << item->GetIngressConnection()->GetConnectionId()) )->str();
	}
      else
	{
	  sourceConnString = "ContentStore";
	}
      NS_LOG_DEBUG ("packet=" << *item->GetPacket () << ", from " << sourceConnString << ", will be fwded to " << egressConnections->size () << " destinations.");
      if (egressConnections->size ())
	{
	  NS_LOG_DEBUG ("first destination is " << egressConnections->front ()->GetConnectionId () );
	}
    }

  m_routeCallback (item->GetPacket (), item->GetIngressConnection (), item->GetRouteError (), egressConnections);
}

void
CCNxStandardForwarder::RouteOutput (Ptr<CCNxPacket> packet,
                                    Ptr<CCNxConnection> ingressConnection,
                                    Ptr<CCNxConnection> egressConnection)
{
  NS_LOG_FUNCTION (this << packet << ingressConnection);
  m_forwarderStats.packetsIn++;

  Ptr<CCNxStandardForwarderWorkItem> item = Create<CCNxStandardForwarderWorkItem> (packet, ingressConnection, egressConnection);
  m_inputQueue->push_back (item);
}

void
CCNxStandardForwarder::RouteInput (Ptr<CCNxPacket> packet,
                                   Ptr<CCNxConnection> ingressConnection)
{
  NS_LOG_FUNCTION (this << packet << ingressConnection);
  m_forwarderStats.packetsIn++;

  Ptr<CCNxStandardForwarderWorkItem> item = Create<CCNxStandardForwarderWorkItem> (packet, ingressConnection, Ptr<CCNxConnection> (0));
  m_inputQueue->push_back (item);
}

/*
 * Called with the result of doing a Pit::ReceiveInterest()
 */
void
CCNxStandardForwarder::PitReceiveInterestCallback (Ptr<CCNxForwarderMessage> message, enum CCNxPit::Verdict verdict)
{
  NS_LOG_FUNCTION (message->GetPacket () << message->GetIngressConnection () << verdict);
  m_forwarderStats.packetsIn++;
  if (verdict == CCNxPit::Forward)
    {
      m_forwarderStats.interestsVerdictForward++;
      if (m_contentStore)
	{
	      NS_LOG_DEBUG ("INTEREST:Verdict=" << verdict << ".  Starting check for match in content store.");
	      m_forwarderStats.interestsToContentStore++;
	      // start next asynchronous call
	      m_contentStore->MatchInterest (message);
	}
      else
	{
	      NS_LOG_DEBUG ("INTEREST:Verdict=" << verdict << ".  Starting FIB lookup");
	      // start next asynchronous call
	      m_forwarderStats.interestsToFib++;
	      m_fib->Lookup (message);
	}
    }
  else //verdict == Aggregate, discard interest
    {
      m_forwarderStats.interestsVerdictAggregate++;
      Ptr<CCNxStandardForwarderWorkItem> workItem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);
      NS_ASSERT_MSG ( (workItem), "Got null dynamic cast from CCNxForwarderMessage to CCNxStandardForwarderWorkItem");
      FinishRouteLookup (workItem, Ptr<CCNxConnectionList> (0));
    }
}

void
CCNxStandardForwarder::PitSatisfyInterestCallback (Ptr<CCNxForwarderMessage> message, Ptr<CCNxConnectionList> egressConnections)
{
  NS_LOG_FUNCTION (message->GetPacket () << message->GetIngressConnection () << egressConnections);

  // For a content object, no additional processing (until ready to put in content store)
  Ptr<CCNxStandardForwarderWorkItem> item = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);
  NS_ASSERT_MSG ( (item), "Got null dynamic cast from CCNxForwarderMessage to CCNxStandardForwarderWorkItem");

  if (egressConnections->size() ) //match!
      {
      m_forwarderStats.contentObjectsMatchedInPit++;
      if  (m_contentStore) // there is a CS, so add this content
	{
	  m_forwarderStats.contentObjectsToContentStore++;
	  NS_LOG_DEBUG ("CONTENT:name=" << *message->GetPacket()->GetMessage()->GetName() <<" matched Pit Entry  - starting add to Content Store. 1st egressConn=" << egressConnections->front()->GetConnectionId());
	  m_contentStore->AddContentObject(message,egressConnections); //will fwd packet after this, so must retain egressConnections
	}
      else
	{  //match but no content store - fwd packet
	      NS_LOG_DEBUG ("CONTENT: match but no content store, finishing route lookup.");
	      FinishRouteLookup (item, egressConnections);
	}
      }
  else
    { //no match - drop pkt
      m_forwarderStats.contentObjectsNotMatchedInPit++;
      NS_LOG_DEBUG ("CONTENT: no Pit Entry match, discarding packet.");
      FinishRouteLookup (item, Ptr<CCNxConnectionList>(0));
    }

}

void
CCNxStandardForwarder::FibLookupCallback (Ptr<CCNxForwarderMessage> message, Ptr<CCNxConnectionList> egressConnections)
{
  NS_LOG_FUNCTION (message->GetPacket () << message->GetIngressConnection () << egressConnections);
  NS_LOG_DEBUG ("Route lookup egress list size: " << egressConnections->size ());

  Ptr<CCNxStandardForwarderWorkItem> item = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);
  NS_ASSERT_MSG ( (item), "Got null dynamic cast from CCNxForwarderMessage to CCNxStandardForwarderWorkItem");

  if (egressConnections && egressConnections->size() > 0) {
      m_forwarderStats.interestsFibForwarded++;
  } else {
      m_forwarderStats.interestsFibNotForwarded++;
  }
  FinishRouteLookup (item, egressConnections); //back to layer 3 protocol eventually
}

void
CCNxStandardForwarder::ContentStoreMatchInterestCallback (Ptr<CCNxForwarderMessage> message)
{
  Ptr<CCNxStandardForwarderWorkItem> workItem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);
  NS_ASSERT_MSG ( (workItem), "Got null dynamic cast from CCNxForwarderMessage to CCNxStandardForwarderWorkItem");

  NS_LOG_FUNCTION (workItem->GetPacket () );
  if ( workItem->GetContentStorePacket() )
    { //matching content found.
      m_forwarderStats.interestsContentStoreHits++;
      //Replace interest packet with content packet, set ingressConnection to Null and send back to pit
      NS_LOG_DEBUG ("INTEREST sent to content store, found match.  Sending Content back to pit.");
      Ptr<CCNxStandardForwarderWorkItem> newWorkItem =
	  Create<CCNxStandardForwarderWorkItem> (workItem->GetContentStorePacket(),Ptr<CCNxConnection>(0),Ptr<CCNxConnection>(0));
      m_pit->SatisfyInterest (newWorkItem);
    }
  else
    { //no match, verdict was forward, send to fib
      m_forwarderStats.interestsContentStoreMisses++;
      m_forwarderStats.interestsToFib++;
      NS_LOG_DEBUG ("INTEREST sent to content store but no match.  Starting FIB lookup.");
      m_fib->Lookup(workItem);
    }
}


void
CCNxStandardForwarder::ContentStoreAddContentObjectCallback (Ptr<CCNxForwarderMessage> message)
{
  Ptr<CCNxStandardForwarderWorkItem> workItem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);
  NS_ASSERT_MSG ( (workItem), "Got null dynamic cast from CCNxForwarderMessage to CCNxStandardForwarderWorkItem");
  NS_LOG_DEBUG ("CONTENT:name=" << *message->GetPacket()->GetMessage()->GetName() <<" returned from CS. 1st egressConn=" << workItem->GetConnectionsList()->front()->GetConnectionId());

  if (workItem->GetContentAddedFlag())
    {
      m_forwarderStats.contentObjectsAddedToContentStore++;
    }
  else
    {
      m_forwarderStats.contentObjectsNotAddedToContentStore++;
    }
  NS_LOG_FUNCTION (workItem->GetPacket ());
  FinishRouteLookup (workItem, workItem->GetConnectionsList()); //back to layer 3 protocol eventually

}

bool
CCNxStandardForwarder::AddRoute (Ptr<CCNxConnection> connection, Ptr<const CCNxName> name)
{
  NS_LOG_FUNCTION (this);

  return InnerAddRoute (connection,name);

}

bool
CCNxStandardForwarder::InnerAddRoute (Ptr<CCNxConnection> connection, Ptr<const CCNxName> name)
{
  NS_LOG_FUNCTION (this << connection << name);
  bool added = false;

  if (connection->GetConnectionId () != CCNxConnection::ConnIdLocalHost)  //dont add localhost entries to fib
    {
      added = m_fib->AddRoute (name,  connection);
    }

  NS_LOG_WARN ( (added ? "Added" : "Didn't Add") << " nexthop connid " << connection->GetConnectionId () << " name " << *name);
  return added;
}

bool
CCNxStandardForwarder::RemoveRoute (Ptr<CCNxConnection> connection, Ptr<const CCNxName> name)
{
  NS_LOG_FUNCTION (this);
  return InnerRemoveRoute (connection,name);

}



bool
CCNxStandardForwarder::InnerRemoveRoute (Ptr<CCNxConnection> connection, Ptr<const CCNxName> name)
{
  NS_LOG_FUNCTION (this << connection << name);
  bool deleted = m_fib->DeleteRoute (name, connection);

  NS_LOG_WARN ( (deleted ? "Deleted" : "Didn't delete") << " nexthop connid " << connection->GetConnectionId () << " name " << *name);


  return deleted;
}


bool
CCNxStandardForwarder::AddRoute (Ptr<const CCNxRoute> route)
{
  NS_LOG_FUNCTION (this);
  bool success = true;
  for (CCNxRoute::const_iterator i = route->begin (); i != route->end (); ++i)
    {
      Ptr<const CCNxRouteEntry> entry = *i;
      success |= AddRoute (entry->GetConnection (), entry->GetPrefix ()); //TODO CCN add cost to fibEntry - currently cost is lost! ensure cost updated if entry already exists.
    }
  return success;
}

bool
CCNxStandardForwarder::RemoveRoute (Ptr<const CCNxRoute> route)
{
  NS_LOG_FUNCTION (this);
  bool success = true;
  for (CCNxRoute::const_iterator i = route->begin (); i != route->end (); ++i)
    {
      Ptr<const CCNxRouteEntry> entry = *i;
      success |= RemoveRoute (entry->GetConnection (), entry->GetPrefix ());
    }
  return success;
}

int
CCNxStandardForwarder::CountEntries (TableTypes t)
{
  int count = 0;

  switch (t)
    {
    case PitTable:
      {
        count = m_pit->CountEntries ();
        break;
      }
    case FibTable:
      {
        count = m_fib->CountEntries ();
        break;
      }
    default:
      {
        count = -1;
        break;
      }
    }
  return (count);
}

void
CCNxStandardForwarder::PrintForwardingTable (Ptr<OutputStreamWrapper> streamWrapper) const
{
  std::ostream *stream = streamWrapper->GetStream ();
  ns3::LogTimePrinter timePrinter = ns3::LogGetTimePrinter ();
  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId () << std::setw (0) << " StandardForwarder " << std::endl;
  m_fib->PrintRoutes(*stream);
}

/*
 * Example output:
 * (time)     4 StandardForwarder Packets    In 400 Out 398 UnsupportedType 2
 * (time)     4 StandardForwarder Interests  ToPit 206 ToForward 200 ToAggregate 6
 * (time)     4 StandardForwarder Interests  ToCs  200 Hits 20 Misses 180
 * (time)     4 StandardForwarder Interests  ToFib 180 Forwarded 150 NotForwarded 30
 * (time)     4 StandardForwarder Content    ToPit 192 MatchedInPit 150 NotMatchedInPit 42
 */
void
CCNxStandardForwarder::PrintForwardingStatistics (Ptr<OutputStreamWrapper> streamWrapper) const
{
  std::ostream *stream = streamWrapper->GetStream ();
  ns3::LogTimePrinter timePrinter = ns3::LogGetTimePrinter ();

  *stream << std::right;

  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId () << std::setw (0) << " StandardForwarder ";
  *stream << std::setw(10) << "Packets" << std::setw(0);
  *stream << " In " << m_forwarderStats.packetsIn;
  *stream << " Out " << m_forwarderStats.packetsOut;
  *stream << " UnsupportedType " << m_forwarderStats.dropUnsupportedPacketType << std::endl;

  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId () << std::setw (0) << " StandardForwarder ";
  *stream << std::setw(10) << "Interests" << std::setw(0);
  *stream << " ToPit " << m_forwarderStats.interestsToPit;
  *stream << " ToForward " << m_forwarderStats.interestsVerdictForward;
  *stream << " ToAggregate " << m_forwarderStats.interestsVerdictAggregate << std::endl;

  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId () << std::setw (0) << " StandardForwarder ";
  *stream << std::setw(10) << "Interests" << std::setw(0);
  *stream << " ToCS  " << m_forwarderStats.interestsToContentStore;
  *stream << " Hits " << m_forwarderStats.interestsContentStoreHits;
  *stream << " Misses " << m_forwarderStats.interestsContentStoreMisses << std::endl;

  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId () << std::setw (0) << " StandardForwarder ";
  *stream << std::setw(10) << "Interests" << std::setw(0);
  *stream << " ToFib " << m_forwarderStats.interestsToFib;
  *stream << " Forwarded " << m_forwarderStats.interestsFibForwarded;
  *stream << " NotForwarded " << m_forwarderStats.interestsFibNotForwarded << std::endl;

  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId () << std::setw (0) << " StandardForwarder ";
  *stream << std::setw(10) << "Content" << std::setw(0);
  *stream << " ToPit " << m_forwarderStats.contentObjectsToPit;
  *stream << " Matched " << m_forwarderStats.contentObjectsMatchedInPit;
  *stream << " NotMatched " << m_forwarderStats.contentObjectsNotMatchedInPit << std::endl;

  (*timePrinter)(*stream);
  *stream << std::setw (5) << m_node->GetId () << std::setw (0) << " StandardForwarder ";
  *stream << std::setw(10) << "Content" << std::setw(0);
  *stream << " ToContentStore " << m_forwarderStats.contentObjectsToContentStore;
  *stream << " Matched " << m_forwarderStats.contentObjectsAddedToContentStore;
  *stream << " NotMatched " << m_forwarderStats.contentObjectsNotAddedToContentStore << std::endl;

}

// ================
// Table Management methods

// Add a route to a remote node via a L2 device
//bool
//CCNxStandardForwarder::AddRoute (Ptr<CCNxConnection> conn, Ptr<const CCNxName> name)
//{
//  NS_LOG_FUNCTION (this << nexthop << name);
//
//  NbrMapType::iterator i = m_neighbors.find(nexthop);
//  NS_ASSERT_MSG (i != m_neighbors.end(), "Nexthop not found in neighbors table " << nexthop);
//
//  FakeFibMapType::iterator j = m_fakefib.find(name);
//  NS_ASSERT_MSG (j == m_fakefib.end(), "Name already exits in FIB " << *name);
//
//  Ptr<CCNxConnectionDevice> conn = i->second;
//
//  m_fakefib[name] = conn->GetConnectionId ();
//
//  NS_LOG_INFO("AddRoute conn " << conn->GetConnectionId () << " name " << *name);
//  return true;
//}
