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

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ccnx-standard-content-store-lruList.h"
#include "ns3/ccnx-standard-content-store-entry.h"
#include <list>
#include <unordered_map>
#include <assert.h>

using namespace ns3;
using namespace ns3::ccnx;


NS_LOG_COMPONENT_DEFINE ("CCNxStandardContentStoreLruList");


CCNxStandardContentStoreLruList::CCNxStandardContentStoreLruList ()
{

}

CCNxStandardContentStoreLruList::~CCNxStandardContentStoreLruList ()
{

}



bool
CCNxStandardContentStoreLruList::AddEntry(Ptr<CCNxStandardContentStoreEntry> entry)
{

      LruMapType::iterator it = m_lruMap.find(entry); //search map

      if(it != m_lruMap.end())
	{ // already exists
	      m_lruList.erase(it->second); //remove  from list. should be fast
	      m_lruList.push_front(entry); //add entry to the front of the list
	      it->second = m_lruList.begin(); //update map
	}
      else
	{  //not in map
	      m_lruList.push_front(entry); //add entry to the front of the list
	      m_lruMap.insert(make_pair(entry, m_lruList.begin())); //add it to the map
	}


      return true;

}

bool
CCNxStandardContentStoreLruList::DeleteEntry(Ptr<CCNxStandardContentStoreEntry> entry)
{
  bool result=false;

  LruMapType::iterator it = m_lruMap.find(entry);

  if(it != m_lruMap.end())
    { // already exists, remove it from list and map (quicker to update map entry?)
      result=true;
      m_lruList.erase(it->second);
      m_lruMap.erase(it);
    }
  else
    {
      NS_LOG_ERROR("Can't delete Entry - entry not found in m_lruMap.");
    }
  return result;


}

uint64_t
CCNxStandardContentStoreLruList::GetSize() const
{

  NS_ASSERT_MSG(m_lruList.size()==m_lruMap.size(), "LRU list and map sizes differ" );
  return m_lruList.size();


}

Ptr<CCNxStandardContentStoreEntry>
CCNxStandardContentStoreLruList::GetBackEntry()
{
  return m_lruList.back();
}


Ptr<CCNxStandardContentStoreEntry>
CCNxStandardContentStoreLruList::GetFrontEntry()
{
  return m_lruList.front();
}







