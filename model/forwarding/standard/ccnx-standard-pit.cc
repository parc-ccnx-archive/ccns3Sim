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

#include "ccnx-standard-pit.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/integer.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxStandardPit");
NS_OBJECT_ENSURE_REGISTERED (CCNxStandardPit);

static const Time _defaultLifetime = Seconds (0.2f);  //default interest lifetime  after arrival at a node

static const Time _defaultLayerDelayConstant = MicroSeconds (1);
static const Time _defaultLayerDelaySlope = NanoSeconds (1);
static unsigned _defaultLayerDelayServers = 1;

/**
 * Used as a default callback for m_recieveInterestCallback in case the user does not set it.
 */
static void
NullReceiveInterestCallback (Ptr<CCNxForwarderMessage>, enum CCNxPit::Verdict)
{
  NS_ASSERT_MSG (false, "You must set the ReceiveInterest Callback via SetReceiveInterestCallback()");
}

/**
 * Used as a default callback for m_satisfyInterestCallback in case the user does not set it.
 */
static void
NullSatisfyInterestCallback (Ptr<CCNxForwarderMessage>, Ptr<CCNxConnectionList>)
{
  NS_ASSERT_MSG (false, "You must set the SatisfyInterest Callback via SetSatisfyInterestCallback()");
}

TypeId
CCNxStandardPit::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxStandardPit")
    .SetParent<CCNxPit> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxStandardPit> ()
    .AddAttribute ("DefaultLifetime", "The PIT Lifetime of an Interest without a lifetime header.",
                   TimeValue (_defaultLifetime),
                   MakeTimeAccessor (&CCNxStandardPit::m_defaultLifetime),
                   MakeTimeChecker ())
    .AddAttribute ("LayerDelayConstant", "The amount of constant layer delay",
                   TimeValue (_defaultLayerDelayConstant),
                   MakeTimeAccessor (&CCNxStandardPit::m_layerDelayConstant),
                   MakeTimeChecker ())
    .AddAttribute ("LayerDelaySlope", "The slope of the layer delay (in terms of name bytes)",
                   TimeValue (_defaultLayerDelaySlope),
                   MakeTimeAccessor (&CCNxStandardPit::m_layerDelaySlope),
                   MakeTimeChecker ())
    .AddAttribute ("LayerDelayServers", "The number of servers for the layer delay input queue",
                   IntegerValue (_defaultLayerDelayServers),
                   MakeIntegerAccessor (&CCNxStandardPit::m_layerDelayServers),
                   MakeIntegerChecker<unsigned> ())
  ;

  return tid;
}


CCNxStandardPit::CCNxStandardPit () : m_defaultLifetime (_defaultLifetime),
  m_receiveInterestCallback (MakeCallback (&NullReceiveInterestCallback)),
  m_satisfyInterestCallback (MakeCallback (&NullSatisfyInterestCallback)),
  m_layerDelayConstant (_defaultLayerDelayConstant),
  m_layerDelaySlope (_defaultLayerDelaySlope),
  m_layerDelayServers (_defaultLayerDelayServers)
{
  NS_LOG_DEBUG ("_defaultLifetime=" << _defaultLifetime << ", m_defaultLifetime=" << m_defaultLifetime);
}

CCNxStandardPit::~CCNxStandardPit ()
{
}

void
CCNxStandardPit::DoInitialize ()
{
  m_inputQueue = Create<DelayQueueType> (m_layerDelayServers,
                                         MakeCallback (&CCNxStandardPit::GetServiceTime, this),
                                         MakeCallback (&CCNxStandardPit::ServiceInputQueue, this));
}

void
CCNxStandardPit::SetReceiveInterestCallback (ReceiveInterestCallback receiveInterestCallback)
{
  m_receiveInterestCallback = receiveInterestCallback;
}

void
CCNxStandardPit::SetSatisfyInterestCallback (SatisfyInterestCallback satisfyInterestCallback)
{
  m_satisfyInterestCallback = satisfyInterestCallback;
}


#define NOCLEANUP

Time
CCNxStandardPit::GetServiceTime (Ptr<CCNxForwarderMessage> item)
{
  Time delay = m_layerDelayConstant;

  Ptr<const CCNxName> name = item->GetPacket ()->GetMessage ()->GetName ();
  if (name)
    {
      size_t nameBytes = 0;
      for (int i = 0; i < name->GetSegmentCount (); ++i)
        {
          Ptr<const CCNxNameSegment> segment = name->GetSegment (i);
          nameBytes += segment->GetValue ().size ();
        }

      delay += m_layerDelaySlope * nameBytes;
    }

  return delay;
}

/**
 * Callback from delay queue after a work item has waited its service time
 *
 * @param item [in] The work item to service
 */
void
CCNxStandardPit::ServiceInputQueue (Ptr<CCNxForwarderMessage> item)
{
  // Interests are input via ReceiveInterst and ContentObjets are input via SatisfyInterest.
  // Because we ensure this invariant before queuing a CCNxStandardWorkItem, we use that
  // invariant here to de-multiplex work items to the proper handler.

  switch (item->GetPacket ()->GetFixedHeader ()->GetPacketType () )
    {
    case CCNxFixedHeaderType_Interest:
      ServiceReceiveInterest (item);
      break;

    case CCNxFixedHeaderType_Object:
      ServiceSatisfyInterest (item);
      break;

    default:
      NS_ASSERT_MSG (false, "ServiceInputQueue got unsupported packet type: " << *item->GetPacket ());
      break;
    }
}

Time
CCNxStandardPit::CalculateInterestExpiryTime(Ptr<CCNxPacket> interestPacket)
{
  // Currently, there is no InterestLifetime header, so always use default lifetime
  Time expiryTime = Simulator::Now() + _defaultLifetime;
  return expiryTime;
}

/*
 * Receive an Interest.  If it is added to the Pit, the Forward verdict is returned indicating
 * that the Interest should be forwarded.  If the Interest is aggregated with a prior Interest,
 * the Verdict Aggregate is returned.
 */
void
CCNxStandardPit::ServiceReceiveInterest (Ptr<CCNxForwarderMessage> item)
{
  //convert packet to  interest message with keyid, expiry, hash and name fields
  Ptr<CCNxInterest> interest = DynamicCast<CCNxInterest, CCNxMessage> (item->GetPacket ()->GetMessage ());

  NS_LOG_DEBUG("ServiceReceiveInterest for " << item->GetPacket()->GetMessage()->GetName());

  Ptr<CCNxStandardPitEntry> pitEntry = LookupPitEntry(interest);

  if ( !pitEntry ) {
      pitEntry = AddPitEntry(interest);
      NS_ASSERT_MSG ( (pitEntry), "Failed to add a PIT entry");
  }

  Time interestExpiryTime = CalculateInterestExpiryTime(item->GetPacket());
  CCNxPit::Verdict verdict = pitEntry->ReceiveInterest(interest, item->GetIngressConnection(), interestExpiryTime);

  NS_LOG_DEBUG ("at end of ReceiveInterest - name,hash pit sizes =[" << m_tableByName.size () << "," << m_tableByHash.size () << "]");
  NS_LOG_DEBUG ("m_defaultLifetime=" << m_defaultLifetime << ", _defaultLifetime=" << _defaultLifetime);

  m_receiveInterestCallback (item, verdict);
}

void
CCNxStandardPit::ServiceSatisfyInterest (Ptr<CCNxForwarderMessage> item)
{
  NS_ASSERT_MSG(item->GetPacket()->GetFixedHeader()->GetPacketType() == CCNxFixedHeaderType_Object,
		"Packet is not a content object: " << *item->GetPacket()->GetFixedHeader());

  NS_LOG_DEBUG("ServiceSatisfyInterest for " << item->GetPacket()->GetMessage()->GetName());

  Ptr<CCNxContentObject> contentObject = DynamicCast<CCNxContentObject, CCNxMessage> (item->GetPacket ()->GetMessage ());

  /*
   * Lookup the content object in all three tables and use the union of the returned values.
   */

  CCNxStandardPitEntry::ReverseRouteType reverseRouteSet;

  if (contentObject->GetName()) {
      Ptr<CCNxStandardPitEntry> entry = LookupPitEntryByName (contentObject->GetName());
      if (entry) {
          CCNxStandardPitEntry::ReverseRouteType aSet = entry->SatisfyInterest(item->GetIngressConnection());
          reverseRouteSet.insert (aSet.begin(), aSet.end());

          if (entry->size() == 0) {
              RemovePitEntryByName (contentObject->GetName ());
          }

	  NS_LOG_DEBUG("Content matched LookupPitEntryByName reverseRouteSet size = " << reverseRouteSet.size());

      } else {
	  NS_LOG_DEBUG("Content object has name, but did not match LookupPitEntryByName");
      }

      // TODO CCN: If it has a ValidationAlgorithm and that has a KeyId do a NameAndKeyId lookup here
      // then add those to the reverseRouteSet.
  }

  {
    Ptr<CCNxHashValue> hash = Create<CCNxHashValue>(item->GetPacket()->GetContentObjectHash()->GetValue());
    Ptr<CCNxStandardPitEntry> entry = LookupPitEntryByHash (hash);
    if (entry) {
        CCNxStandardPitEntry::ReverseRouteType aSet = entry->SatisfyInterest(item->GetIngressConnection());
        // because it's a set, this insert will make a union
        reverseRouteSet.insert (aSet.begin(), aSet.end());

        if (entry->size() == 0) {
            RemovePitEntryByHash (hash);
        }

	 NS_LOG_DEBUG("Content matched LookupPitEntryByName reverseRouteSet size = " << reverseRouteSet.size());
    } else {
	 NS_LOG_DEBUG("Content object did not match LookupPitEntryByHash");
    }
  }

  // Now convert the Set into a ConnectionList
  Ptr<CCNxConnectionList> satisfiedConnections = Create<CCNxConnectionList>();
  for (CCNxStandardPitEntry::ReverseRouteType::iterator i = reverseRouteSet.begin(); i != reverseRouteSet.end(); ++i) {
      satisfiedConnections->push_back(*i);
  }

  NS_LOG_DEBUG ( __func__ << " returning conn list of size = " << satisfiedConnections->size () );
  if (satisfiedConnections->size ())
    {
      NS_LOG_DEBUG ( __func__ << " connId of first entry = " << satisfiedConnections->front ()->GetConnectionId () );
    }

  m_satisfyInterestCallback (item, satisfiedConnections);
}

void
CCNxStandardPit::ReceiveInterest (Ptr<CCNxForwarderMessage> item)
{
  NS_ASSERT_MSG (item->GetPacket ()->GetFixedHeader ()->GetPacketType () == CCNxFixedHeaderType_Interest,
                 "ReceiveInterest given a non-Interest packet: " << *item->GetPacket ());

  m_inputQueue->push_back (item);
}

/**
 * Receive a ContentObject and try to satisfy interests in the Pit.  Returns a list
 * of the Connections to forward the ContentObject to.  The corresponding Pit entries
 * are removed.  The caller must then resolve each Connection in the list to an
 * egress connection.
 */

void
CCNxStandardPit::SatisfyInterest (Ptr<CCNxForwarderMessage> item)
{
  NS_ASSERT_MSG (item->GetPacket ()->GetFixedHeader ()->GetPacketType () == CCNxFixedHeaderType_Object,
                 "SatisfyInterest given a non-ContentObject packet: " << *item->GetPacket ());

  m_inputQueue->push_back (item);
}


// ============= LOAD AND STORE ROUTINES

CCNxStandardPit::NameAndKeyIdType
CCNxStandardPit::CreateNameAndKeyIdKey(Ptr<const CCNxName> name, Ptr<CCNxHashValue> keyId)
{
  NameAndKeyIdType key = NameAndKeyIdType(name, keyId);
  return key;
}

Ptr<CCNxStandardPitEntry>
CCNxStandardPit::LookupPitEntryByHash(Ptr<CCNxHashValue> hashRestriction)
{
  Ptr<CCNxStandardPitEntry> entry = Ptr<CCNxStandardPitEntry>(0);
  PitByHashType::iterator iterator =  m_tableByHash.find (hashRestriction);
  if (iterator != m_tableByHash.end()) {
      entry = iterator->second;
  }
  return entry;
}

Ptr<CCNxStandardPitEntry>
CCNxStandardPit::LookupPitEntryByName(Ptr<const CCNxName> name)
{
  Ptr<CCNxStandardPitEntry> entry = Ptr<CCNxStandardPitEntry>(0);
  PitByNameType::iterator iterator =  m_tableByName.find (name);
  if (iterator != m_tableByName.end()) {
      entry = iterator->second;
  }
  return entry;
}

Ptr<CCNxStandardPitEntry>
CCNxStandardPit::LookupPitEntryByNameAndKeyId(Ptr<const CCNxName> name, Ptr<CCNxHashValue> keyId)
{
  Ptr<CCNxStandardPitEntry> entry = Ptr<CCNxStandardPitEntry>(0);
  NameAndKeyIdType key = CreateNameAndKeyIdKey(name, keyId);
  PitByNameAndKeyIdType::iterator iterator =  m_tableByNameAndKeyId.find (key);
  if (iterator != m_tableByNameAndKeyId.end()) {
      entry = iterator->second;
  }
  return entry;
}

Ptr<CCNxStandardPitEntry>
CCNxStandardPit::LookupPitEntry(Ptr<CCNxInterest> interest)
{
  Ptr<CCNxStandardPitEntry> entry = Ptr<CCNxStandardPitEntry>(0);

  if (interest->HasHashRestriction ()) {
      entry = LookupPitEntryByHash(interest->GetHashRestriction ());
  } else if (interest->GetName ()) {
      if (interest->HasKeyidRestriction()) {
	  // TODO CCN: Fill in the keyid instead of NULL
	  entry = LookupPitEntryByNameAndKeyId(interest->GetName (), NULL);
      } else {
	  entry = LookupPitEntryByName(interest->GetName ());
      }
  }

  return entry;
}

Ptr<CCNxStandardPitEntry>
CCNxStandardPit::AddPitEntry(Ptr<CCNxInterest> interest)
{

  Ptr<CCNxStandardPitEntry> entry = Ptr<CCNxStandardPitEntry>(0);

  if (interest->HasHashRestriction ()) {
      entry = AddPitEntryByHash(interest);
  } else if (interest->GetName ()){
      entry = AddPitEntryByName(interest);
  } else {
      NS_ASSERT_MSG(false, "Trying to add an interest without a hash restriction or a name");
  }

  return entry;
}

Ptr<CCNxStandardPitEntry>
CCNxStandardPit::AddPitEntryByHash(Ptr<CCNxInterest> interest)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  Ptr<const CCNxHashValue> key = interest->GetHashRestriction();
  m_tableByHash[key] = entry;
  return entry;
}

Ptr<CCNxStandardPitEntry>
CCNxStandardPit::AddPitEntryByName(Ptr<CCNxInterest> interest)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  Ptr<const CCNxName> key = interest->GetName();
  m_tableByName[key] = entry;
  return entry;
}

Ptr<CCNxStandardPitEntry>
CCNxStandardPit::AddPitEntryByNameAndKeyId(Ptr<CCNxInterest> interest)
{
  Ptr<CCNxStandardPitEntry> entry = Create<CCNxStandardPitEntry>();
  NameAndKeyIdType key = CreateNameAndKeyIdKey(interest->GetName(), interest->GetKeyidRestriction());
  m_tableByNameAndKeyId[key] = entry;
  return entry;
}

void
CCNxStandardPit::RemovePitEntryByHash(Ptr<CCNxHashValue> hashRestriction)
{
  m_tableByHash.erase(hashRestriction);
}

void
CCNxStandardPit::RemovePitEntryByNameAndKeyId(Ptr<const CCNxName> name, Ptr<CCNxHashValue> keyId)
{
  NameAndKeyIdType key = CreateNameAndKeyIdKey(name, keyId);
  m_tableByNameAndKeyId.erase(key);
}

/**
 * Remove the PIT entry in the name table
 *
 * @param name [in] The Interest name to remove
 */
void
CCNxStandardPit::RemovePitEntryByName(Ptr<const CCNxName> name)
{
  NS_LOG_DEBUG("RemovePitEntryByName: " << *name);

  m_tableByName.erase(name);
}

// ==============

/**
 * Removes all Pit entries that corresponds to the given Interest
 */
void
CCNxStandardPit::RemoveEntry (Ptr<CCNxInterest> interest)
{
//TODO CCN write RemoveEntry function

}

int
CCNxStandardPit::CountEntries ()
{
  return m_tableByName.size () + m_tableByHash.size () + m_tableByNameAndKeyId.size ();
}

/**
 * Display the Pit table
 */

void
CCNxStandardPit::Print (std::ostream &os) const
{

  os << "----------------------------------------m_tableByName Map----------------------------------------" << std::endl;

  int i = 0;

  for (CCNxStandardPit::PitByNameType::const_iterator it = m_tableByName.begin (); it != m_tableByName.end (); it++ )
    {

      os << "MapEntry[";
      os << i++ << "] = ";
      os << *it->first << std::endl;
      os << it->second->PrintPitEntry (os)  << std::endl;

    }


  os << "------------------------------------m_tableByNameAndKeyId Map----------------------------------------" << std::endl;

  i = 0;

  for (CCNxStandardPit::PitByNameAndKeyIdType::const_iterator it = m_tableByNameAndKeyId.begin (); it != m_tableByNameAndKeyId.end (); it++ )
    {

      os << "MapEntry[";
      os << i++ << "] = ";
      os << *(it->first.first) << ", " << *(it->first.second->GetValue()) << std::endl;
      os << it->second->PrintPitEntry (os)  << std::endl;

    }



  os << "----------------------------------------m_tableByHash Map----------------------------------------" << std::endl;


  i = 0;

  for (CCNxStandardPit::PitByHashType::const_iterator it = m_tableByHash.begin (); it != m_tableByHash.end (); it++ )
    {

      os << "MapEntry[";
      os << i++ << "] = ";
      os << it->first << std::endl;
      os << it->second->PrintPitEntry (os)  << std::endl;

    }

  os << std::endl;
  os << std::endl;

}



