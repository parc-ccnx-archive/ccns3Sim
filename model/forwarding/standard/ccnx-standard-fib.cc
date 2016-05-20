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



#include "ccnx-standard-fib.h"
#include "ns3/ccnx-name-builder.h"
#include "ns3/simple-ref-count.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/object.h"
#include "ns3/integer.h"
#include "ns3/ccnx-standard-forwarder-work-item.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxStandardFib");
NS_OBJECT_ENSURE_REGISTERED (CCNxStandardFib);


static const Time _defaultLayerDelayConstant = MicroSeconds (1);
static const Time _defaultLayerDelaySlope = NanoSeconds (10);
static unsigned _defaultLayerDelayServers = 1;

/**
 * Used as a default callback for m_recieveInterestCallback in case the user does not set it.
 */
static void
NullLookupCallback (Ptr<CCNxForwarderMessage>, Ptr<CCNxConnectionList> egressConnections)
{
  NS_ASSERT_MSG (false, "You must set the Lookup Callback via SetLookupCallback()");
}

TypeId
CCNxStandardFib::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxStandardFib")
    .SetParent<CCNxFib> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxStandardFib> ()
    .AddAttribute ("LayerDelayConstant", "The amount of constant layer delay",
                   TimeValue (_defaultLayerDelayConstant),
                   MakeTimeAccessor (&CCNxStandardFib::m_layerDelayConstant),
                   MakeTimeChecker ())
    .AddAttribute ("LayerDelaySlope", "The slope of the layer delay (in terms of name bytes)",
                   TimeValue (_defaultLayerDelaySlope),
                   MakeTimeAccessor (&CCNxStandardFib::m_layerDelaySlope),
                   MakeTimeChecker ())
    .AddAttribute ("LayerDelayServers", "The number of servers for the layer delay input queue",
                   IntegerValue (_defaultLayerDelayServers),
                   MakeIntegerAccessor (&CCNxStandardFib::m_layerDelayServers),
                   MakeIntegerChecker<unsigned> ())
  ;

  return tid;
}


CCNxStandardFib::CCNxStandardFib ()
  :   m_LookupCallback (MakeCallback (&NullLookupCallback)),
  m_layerDelayConstant (_defaultLayerDelayConstant), m_layerDelaySlope (_defaultLayerDelaySlope),
  m_layerDelayServers (_defaultLayerDelayServers)
{

}

CCNxStandardFib::~CCNxStandardFib ()
{

}

void
CCNxStandardFib::DoInitialize ()
{
  m_inputQueue = Create<DelayQueueType> (m_layerDelayServers,
                                         MakeCallback (&CCNxStandardFib::GetServiceTime, this),
                                         MakeCallback (&CCNxStandardFib::DequeueCallback, this));
}

void
CCNxStandardFib::SetLookupCallback (LookupCallback lookupCallback)
{
  m_LookupCallback = lookupCallback;
}

bool
CCNxStandardFib::RemoveConnection (  Ptr<CCNxConnection>  connection)
{
  std::pair< FibType::iterator, bool > fibRvPair;

  FibType::iterator itbegin = fib.begin ();
  FibType::iterator itend = fib.end ();

  std::pair<bool,int> rvPair;

  for (FibType::iterator it = itbegin; it != itend; )
    {
      rvPair = it->second->RemoveConnection (connection);


      if (rvPair.second == 0)      //erase fib entry
        {
          FibType::iterator itlast = it;
          ++it;   //erase invalidates ptr, so must inc it first
          fib.erase (itlast);
        }
      else
        {
          ++it;
        }
    }

  NS_LOG_INFO ( __func__ << " successful.");
  return true;
}


unsigned
CCNxStandardFib::PerformLookup (Ptr<CCNxStandardForwarderWorkItem> workItem)
{
  unsigned nameCompoentsUsed = 0; //TODO CCN - is nameCompoentsUsed being properly incremented?

  Ptr<CCNxConnectionList> egress = Create<CCNxConnectionList> ();
  workItem->SetConnectionsList (egress);

  Ptr<const CCNxName> ccnxName = workItem->GetPacket ()->GetMessage ()->GetName ();
  Ptr<CCNxConnection> ingress = workItem->GetIngressConnection ();

  CCNxNameBuilder builder;

  FibType::iterator bestMatch = fib.end ();
  NS_LOG_INFO ( __func__ << " fib size =" << fib.size ());

  //copy first segment of name to name copy and lookup in fib.
  //if match, add next name segment and look again until no match or name ends
  //if no match, try again until end of name
  int i = 0;
  while (i < ccnxName->GetSegmentCount () )
    {
      builder.Append (ccnxName->GetSegment (i));
      i++;
      FibType::iterator it = fib.find (builder.CreateName ());

      if ( it != fib.end () )
        {
          bestMatch = it;
        }
      else
        {
          if (bestMatch != fib.end ())
            {
              break;       //name was matching but stopped
            }
        }
    }


  if (bestMatch == fib.end ())
    {
      NS_LOG_INFO ( __func__ << " No fib match!");
    }
  else         //get vector and translate  into list, removing ingress connection
    {
      CCNxStandardFibEntry::ConnectionsVecType connectionsVec = bestMatch->second->GetConnections ();
      for (CCNxStandardFibEntry::ConnectionsVecType::iterator it = connectionsVec.begin (); it != connectionsVec.end (); it++)
        {
          Ptr<CCNxConnection> conn = *it;
          if (conn->GetConnectionId () != ingress->GetConnectionId ())
            {
              egress->push_back (conn);
            }
        }
    }

  NS_LOG_DEBUG ("Lookup match count: " << egress->size ());

  //TODO CCN - if connections is empty, best match was same as ingress, so need to recurse until different connection found or no result
  return nameCompoentsUsed;
}

/*
 * This does the actual route lookup because the service time depends on how many
 * name segments we need to process
 */
Time
CCNxStandardFib::GetServiceTime (Ptr<CCNxForwarderMessage> message)
{
  Time delay = m_layerDelayConstant;

  // We need to put the egress connections in to the CCNxStandardForwarderWorkItem
  Ptr<CCNxStandardForwarderWorkItem> workitem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);
  NS_ASSERT_MSG ( (workitem), "Got null casting CCNxForwarderMessage to CCNxStandardForwarderWorkItem");

  unsigned nameComponentsUsed = PerformLookup (workitem);
  delay += nameComponentsUsed * m_layerDelaySlope;

  return delay;
}

void
CCNxStandardFib::DequeueCallback (Ptr<CCNxForwarderMessage> message) //TODO CCN _this is after lookup
{
  // We saved the results of the route lookup in GetServiceTime() in the derived class.
  // For compatibility with the standard CCNxFib API, we extract them and put them where they
  // are supposed to be in the return call.

  Ptr<CCNxStandardForwarderWorkItem> workItem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);

  m_LookupCallback (message, workItem->GetConnectionsList () ); //passing result back to the forwarder
}

void
CCNxStandardFib::Lookup (Ptr<CCNxForwarderMessage> message)
{
  NS_ASSERT_MSG (message->GetPacket ()->GetFixedHeader ()->GetPacketType () == CCNxFixedHeaderType_Interest,
                 "Lookup given a non-Interest packet: " << *message->GetPacket ());

  m_inputQueue->push_back (message);
} //Lookup

bool
CCNxStandardFib::AddRoute (Ptr<const CCNxName> ccnxName,  Ptr<CCNxConnection>  connection)
{
  NS_LOG_FUNCTION (this);

  std::pair< FibType::iterator, bool > fibRvPair;

  //find this name in fib. if already present, it's fibEntry ptr is returned.
  //if not present, a new location is added to the map and a null ptr is returned.
  Ptr<CCNxStandardFibEntry> fibEntry = fib[ccnxName];

  //if route already in fib, modify entry. if not add new entry first.
  if (fibEntry != NULL)
    {

      fibEntry->AddConnection (connection);
    }
  else
    {
      NS_LOG_DEBUG ( __func__ << " adding new route[" << *ccnxName << "] = [" << connection->GetConnectionId () << "]");

      fib[ccnxName] = Create<CCNxStandardFibEntry> ();

      fib[ccnxName]->AddConnection (connection);

    }


  return true;

} //AddRoute


bool
CCNxStandardFib::DeleteRoute (Ptr<const CCNxName> ccnxName,  Ptr<CCNxConnection>  connection)
{

  Ptr<CCNxStandardFibEntry> fibEntry = Create<CCNxStandardFibEntry> ();

  FibType::iterator it = fib.find (ccnxName);
  std::pair <bool,int > rvPair;

  if (it != fib.end ())
    {
      rvPair = it->second->RemoveConnection (connection);
      if (rvPair.second == 0)          //erase fib entry
        {
          fib.erase (ccnxName);
        }
      NS_LOG_INFO ( __func__ << " successful.");
      return rvPair.first;      //only return true if name and connection were matched.
    }
  else
    {
      NS_LOG_INFO ( __func__ << " unsuccessful. no such route.");
      return false;
    }


} //DeleteRoute

int CCNxStandardFib::CountEntries ()
{
  return fib.size ();
}

std::ostream&
CCNxStandardFib::PrintRoutes (std::ostream & os)
{
  os << __func__ << "--------------------Routing Table--------------------" << std::endl;
  os <<  "Fib Entry Count =" << CountEntries () << std::endl;

  for (FibType::iterator it = fib.begin (); it != fib.end (); ++it )
    {
      PrintRoute (os, it->first);
    }
  os << this << "----------------End of Routing Table-----------------" << std::endl;
  return os;

}

/*
 * PRECONDITION: you know the ccnxName is in the FIB already
 */
std::ostream&
CCNxStandardFib::PrintRoute (std::ostream & os, Ptr<const CCNxName> ccnxName)
{
   Ptr<CCNxStandardFibEntry> ccnxStandardFibEntry = fib[ccnxName];

  os << "Key = " << *ccnxName;
  os << " Value =" << *ccnxStandardFibEntry;
  os << std::endl;
  return os;

}

