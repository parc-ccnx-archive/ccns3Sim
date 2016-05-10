/*
 * Copyright (c) 2016, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ccnx-standard-content-store-lruList.h"


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

	m_lruList.push_front(entry);
	return true;

}

bool
CCNxStandardContentStoreLruList::DeleteEntry(Ptr<CCNxStandardContentStoreEntry> entry)
{

	 m_lruList.remove(entry);
	 return true;

}

uint64_t
CCNxStandardContentStoreLruList::GetSize() const
{

	 return m_lruList.size();


}

bool
CCNxStandardContentStoreLruList::DeletePacket(Ptr<CCNxPacket> cPacket)
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
CCNxStandardContentStoreLruList::RefreshEntry (Ptr<CCNxStandardContentStoreEntry> entry)
{
  //move this entry to the front of the lru List
  m_lruList.remove(entry);
  m_lruList.push_front(entry);
  return true;
}



Ptr<CCNxPacket>
CCNxStandardContentStoreLruList::GetTailPacket () const
{
  return m_lruList.back()->GetPacket();
}

