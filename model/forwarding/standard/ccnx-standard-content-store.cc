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
 * # is up to the contributors to maintain their section in this file up to date
 * # and up to the user of the software to verify any claims herein.
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

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/integer.h"
#include "ns3/ccnx-delay-queue.h"

#include "ccnx-standard-content-store.h"

using namespace ns3;
using namespace ns3::ccnx;

static const long long _defaultObjectCapacity = 10000;  //size_t and uint64_t dont work with AddAttribute

static const Time _defaultLayerDelayConstant = MicroSeconds (1);
static const Time _defaultLayerDelaySlope = NanoSeconds (10);
static unsigned _defaultLayerDelayServers = 1;

//TODO - Marc: in simulation, where does content object hash come from? is it guaranteed to be unique? needs to be

NS_LOG_COMPONENT_DEFINE ("CCNxStandardContentStore");
NS_OBJECT_ENSURE_REGISTERED (CCNxStandardContentStore);

/**
 * Used as a default callback for m_recieveInterestCallback in case the user does not set it.
 */
//TODO - ask marc if NullMatchInterestCallback should be global or CCNxStandardContentStore scoped?
static void
NullMatchInterestCallback (Ptr<CCNxForwarderMessage> message)
{
  NS_ASSERT_MSG (false, "You must set the MatchInterest Callback via SetMatchInterestCallback()");
}

/**
 * Used as a default callback for m_satisfyInterestCallback in case the user does not set it.
 */
static void
NullAddContentObjectCallback (Ptr<CCNxForwarderMessage> message )
{
  NS_ASSERT_MSG (false, "You must set the AddContentObject Callback via SetAddContentObjectCallback()");
}

TypeId
CCNxStandardContentStore::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxStandardContentStore")
    .SetParent<CCNxContentStore> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxStandardContentStore> ()
    .AddAttribute ("ObjectCapacity",
                  "The maximum number of content objects to store (default = 10000)",
		   IntegerValue (_defaultObjectCapacity),
		   MakeIntegerAccessor (&CCNxStandardContentStore::m_objectCapacity),
		   MakeIntegerChecker<long long> ())
    .AddAttribute ("LayerDelayConstant", "The amount of constant layer delay",
		  TimeValue (_defaultLayerDelayConstant),
		  MakeTimeAccessor (&CCNxStandardContentStore::m_layerDelayConstant),
		  MakeTimeChecker ())
    .AddAttribute ("LayerDelaySlope", "The slope of the layer delay (in terms of name bytes)",
		  TimeValue (_defaultLayerDelaySlope),
		  MakeTimeAccessor (&CCNxStandardContentStore::m_layerDelaySlope),
		  MakeTimeChecker ())
    .AddAttribute ("LayerDelayServers", "The number of servers for the layer delay input queue",
		  IntegerValue (_defaultLayerDelayServers),
		  MakeIntegerAccessor (&CCNxStandardContentStore::m_layerDelayServers),
		  MakeIntegerChecker<unsigned> ());
    return tid;
}


CCNxStandardContentStore::CCNxStandardContentStore () : m_objectCapacity (_defaultObjectCapacity),
      m_matchInterestCallback (MakeCallback (&NullMatchInterestCallback)),
      m_addContentObjectCallback (MakeCallback (&NullAddContentObjectCallback)),
      m_layerDelayConstant (_defaultLayerDelayConstant), m_layerDelaySlope (_defaultLayerDelaySlope),
      m_layerDelayServers (_defaultLayerDelayServers)
{

}

CCNxStandardContentStore::~CCNxStandardContentStore ()
{

}


void
CCNxStandardContentStore::DoInitialize ()
{
  m_inputQueue = Create<DelayQueueType> (m_layerDelayServers,
                                         MakeCallback (&CCNxStandardContentStore::GetServiceTime, this),
                                         MakeCallback (&CCNxStandardContentStore::ServiceInputQueue, this));
}

void
CCNxStandardContentStore::SetMatchInterestCallback (MatchInterestCallback matchInterestCallback)
{
  m_matchInterestCallback = matchInterestCallback;
}

void
CCNxStandardContentStore::SetAddContentObjectCallback (AddContentObjectCallback addContentObjectCallback)
{
  m_addContentObjectCallback = addContentObjectCallback;
}


Time
CCNxStandardContentStore::GetServiceTime (Ptr<CCNxStandardForwarderWorkItem> workItem)
{

  NS_ASSERT_MSG ( workItem!=0, "Got null casting CCNxForwarderMessage to CCNxStandardForwarderWorkItem");


  Time delay = m_layerDelayConstant;

  Ptr<const CCNxName> name = workItem->GetPacket ()->GetMessage ()->GetName ();
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


void
CCNxStandardContentStore::MatchInterest (Ptr<CCNxForwarderMessage> message)
{
  Ptr<CCNxStandardForwarderWorkItem> workItem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);
  NS_ASSERT_MSG (message->GetPacket()->GetFixedHeader ()->GetPacketType () == CCNxFixedHeaderType_Interest,
                 "Lookup given a non-Interest packet: " << *message->GetPacket ());

  m_inputQueue->push_back (workItem);
}

void
CCNxStandardContentStore::AddContentObject (Ptr<CCNxForwarderMessage> message, Ptr<CCNxConnectionList> egressConnections)
{
  Ptr<CCNxStandardForwarderWorkItem> workItem = DynamicCast<CCNxStandardForwarderWorkItem, CCNxForwarderMessage> (message);
  NS_ASSERT_MSG (message->GetPacket()->GetFixedHeader ()->GetPacketType () == CCNxFixedHeaderType_Object,
                 "Lookup given a non-content packet: " << *message->GetPacket ());
  workItem->SetConnectionsList(egressConnections); //will fwd packet after this, so must retain egressConnections
  m_inputQueue->push_back (workItem);
}

/**
 * Callback from delay queue after a work item has waited its service time
 *
 * @param item [in] The work item to service
 */
void
CCNxStandardContentStore::ServiceInputQueue (Ptr<CCNxStandardForwarderWorkItem> workItem)
{
  // Interests are input via MatchInterst and ContentObjects are input via AddContentObject.
  // Because we ensure this invariant before queuing a CCNxStandardWorkItem, we use that
  // invariant here to de-multiplex work items to the proper handler.


  switch (workItem->GetPacket ()->GetFixedHeader ()->GetPacketType () )
    {
    case CCNxFixedHeaderType_Interest:
      ServiceMatchInterest (workItem);
      break;

    case CCNxFixedHeaderType_Object:
      ServiceAddContentObject (workItem);
      break;

    default:
      NS_ASSERT_MSG (false, "ServiceInputQueue got unsupported packet type: " << *workItem->GetPacket ());
      break;
    }
}

void
CCNxStandardContentStore::ServiceMatchInterest (Ptr<CCNxStandardForwarderWorkItem> workItem)
{
  NS_LOG_FUNCTION (this);

  Ptr<CCNxInterest> interest = DynamicCast<CCNxInterest, CCNxMessage> (workItem->GetPacket()->GetMessage ());
  NS_ASSERT_MSG (workItem->GetPacket()->GetFixedHeader ()->GetPacketType () == CCNxFixedHeaderType_Interest,
                 "ServiceMatchInterest given a non-Interest packet: " << *workItem->GetPacket ());

  Ptr<CCNxStandardContentStoreEntry> entry = Ptr<CCNxStandardContentStoreEntry> (0) ;

  //look for matching content in one map only
  if (interest->GetName () and interest->GetName ()->GetSegmentCount ())
    {
      if (interest->HasKeyidRestriction ())
        {        //name and keyid map only
          CSByNameKeyidType::iterator it = m_csByNameKeyid.find (workItem->GetPacket());
	      if (it!=m_csByNameKeyid.end())
            {
              entry = it->second;
            }
        }
      else
        {         //name map only
          CSByNameType::iterator it = m_csByName.find (workItem->GetPacket());
		if (it!=m_csByName.end())
            {
              entry = it->second;
            }
        }
    }
  else
    {   //hash map only
      CSByHashType::iterator it = m_csByHash.find (workItem->GetPacket());
      if (it!=m_csByHash.end())
        {
          entry = it->second;
        }
    }

 if (entry)
    {
     if (IsEntryValid(entry))
       {
	//increment it's use count
	 entry->IncrementUseCount();

	 RefreshLruListEntry(entry);

	 workItem->SetContentStorePacket(entry->GetPacket());
       }
     else
       { //entry not valid, remove it and dont add a content store packet
	NS_LOG_INFO ("removing expired or stale content in CS matching this interest - no content returned");
	RemoveContentObject(entry->GetPacket());
       }
    }
 else
   {
     //no entry found
     NS_LOG_INFO ("unable to find content for this interest in CS");
   }

 m_matchInterestCallback (workItem ); //overloading CCNxForwarderMessage with workItem since this callback cant use workItem

}

void
CCNxStandardContentStore::ServiceAddContentObject (Ptr<CCNxStandardForwarderWorkItem> workItem)
{
  NS_LOG_FUNCTION (this);
  bool result = false;
  Ptr<CCNxPacket> cPacket = workItem->GetPacket();

  //check if object already in content store
  if (!FindEntryInHashMap(cPacket))
  {
      //create new entry
      Ptr<CCNxStandardContentStoreEntry> newEntry = Create<CCNxStandardContentStoreEntry> (cPacket);
      result = GetObjectCount() < GetObjectCapacity();

      if (!result)
        {
	  result=RemoveContentObject(m_lruList.back()->GetPacket());
        }
      if (result)
	{
	  result = AddLruListEntry(newEntry);
	}
      if (result)
	{
	  result = AddMapEntry(cPacket,newEntry);
	}

  }
  m_addContentObjectCallback (workItem); //overloading CCNxForwarderMessage with workItem since this callback cant use workItem
}

bool
CCNxStandardContentStore::IsEntryValid(Ptr<CCNxStandardContentStoreEntry> entry) const
{
  bool dead = false;

    dead = entry->IsExpired() or entry->IsStale();

    if (dead)
      {
      Ptr<CCNxContentObject> object = DynamicCast<CCNxContentObject,CCNxMessage>(entry->GetPacket()->GetMessage());
      NS_LOG_INFO("content in CS named " << *object->GetName() << " has expired");
      }

    return (not dead);
}




Ptr<CCNxStandardContentStoreEntry>
CCNxStandardContentStore::FindEntryInHashMap(Ptr<CCNxPacket> cPacket)
{
  NS_LOG_FUNCTION (this);

  //check the hash map only - all objects should be in this map
  CSByHashType::iterator it = m_csByHash.find(cPacket);
  return (it != m_csByHash.end () ? it->second :Ptr<CCNxStandardContentStoreEntry>(0) );

}

bool
CCNxStandardContentStore::RemoveContentObject(Ptr<CCNxPacket> cPacket)
{
  bool result = false;
  Ptr<CCNxStandardContentStoreEntry> entry;


  NS_LOG_FUNCTION (this);

  //remove packet from all maps
  result=m_csByHash.erase(cPacket); //hash map erase should always return 1 or higher

  if (result)
    {
	if (cPacket->GetMessage()->GetName())
        {
	    result=m_csByName.erase(cPacket);
#ifdef KEYIDHACK
          if (1) //always erase this map, since all content is given a keyid
#else
	    if (cPacket->m_message->HasKeyid())
#endif
            {
		result=m_csByNameKeyid.erase(cPacket);
            }
        }   //name map

    }   //hash map


  if (result) //dont delete it if it was not found in the maps
    {
      DeleteLruListPacket(cPacket);
        {
        }
    }

  return result;
}

bool
CCNxStandardContentStore::AddLruListEntry(Ptr<CCNxStandardContentStoreEntry> entry)
{

	m_lruList.push_front(entry);
	return true;

}

bool
CCNxStandardContentStore::DeleteLruListEntry(Ptr<CCNxStandardContentStoreEntry> entry)
{

	 m_lruList.remove(entry);
	 return true;

}

bool
CCNxStandardContentStore::DeleteLruListPacket(Ptr<CCNxPacket> cPacket)
{
  /*
  * Do a Lazy delete of the Ptr<CCNxStandardContentStoreEntry> from linked list, ie:
  * If this is the packet at the end of the linked list, remove it.
  *
  * If it is not deleted, it will eventually get aged out.
  * A guaranteed delete requires mapping the cPacket to the Ptr<CCNxStandardContentStoreEntry>,
  * which requires searching the maps - not difficult but tedious and unnecessary most of the
  * time, since deletes will be typically used to remove the last item in the linked list.
  *
  */

     if (m_lruList.back()->GetPacket() == cPacket) //lazy delete
       {
	     m_lruList.pop_back();
	     return true;
       }
     return false;
}

bool
CCNxStandardContentStore::AddMapEntry(Ptr<CCNxPacket> cPacket, Ptr<CCNxStandardContentStoreEntry> newEntry)
{

  Ptr<CCNxContentObject> content = DynamicCast<CCNxContentObject, CCNxMessage>(cPacket->GetMessage());

  m_csByHash[cPacket] = newEntry;

  if (content->GetName())
    {
    m_csByName[cPacket] = newEntry;

  #ifdef KEYIDHACK
    if (1)  //for now, all content will treated as if it has a keyid
  #else
    if (content->HasKeyid())
  #endif
      {
        m_csByNameKeyid[cPacket] = newEntry;
      }
    }
  return true;
}





bool
CCNxStandardContentStore::RefreshLruListEntry (Ptr<CCNxStandardContentStoreEntry> entry)
{
  //move this entry to the front of the lru List
  m_lruList.remove(entry);
  m_lruList.push_front(entry);
  return true;
}



Ptr<CCNxPacket>
CCNxStandardContentStore::GetLruListTailPacket () const
{
  return m_lruList.back()->GetPacket();
}

size_t
CCNxStandardContentStore::GetObjectCount () const
{
  // since we have other methods to check the map sizes, we will do this count using the LRU list
  return m_lruList.size ();
}

size_t
CCNxStandardContentStore::GetObjectCapacity () const
{
  return m_objectCapacity;
}

size_t
CCNxStandardContentStore::GetMapByHashCount () const
{
  return m_csByHash.size ();
}

size_t
CCNxStandardContentStore::GetMapByNameCount () const
{
  return m_csByName.size ();
}

size_t
CCNxStandardContentStore::GetMapByNameKeyidCount () const
{
    return m_csByNameKeyid.size ();
}



