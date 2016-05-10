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

#ifndef CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_CONTENT_STORE_LRU_LIST_H_
#define CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_CONTENT_STORE_LRU_LIST_H_

#include <map>


namespace ns3 {
namespace ccnx {

class CCNxStandardContentStoreLruList : public ns3::SimpleRefCount<CCNxStandardContentStoreLruList>
{
public:

  /**
   * Create a standard content store.  It is configured via the NS attribute system.
   */
  CCNxStandardContentStoreLruList ();

  /**
   * Virtual destructor for inheritance
   */
  virtual ~CCNxStandardContentStoreLruList ();



  virtual bool AddEntry(Ptr<CCNxStandardContentStoreEntry> entry);

  virtual bool DeleteEntry(Ptr<CCNxStandardContentStoreEntry> entry);

  virtual bool RefreshEntry(Ptr<CCNxStandardContentStoreEntry> entry);

  virtual bool DeletePacket(Ptr<CCNxPacket> cPacket);

  virtual   Ptr<CCNxPacket> GetTailPacket() const;

  virtual uint64_t GetSize() const;

protected:


private:

   /**
    * The Least Recently Used (lru) list has the least recently used packet at the end of the list,
    * and the most recently used at the beginning. The list allows the LRU algorithm to be used when
    * to evict packets when the content store has reached maximum size.
    */
   std::list<Ptr<CCNxStandardContentStoreEntry> > m_lruList;

};

}   /* namespace ccnx */
} /* namespace ns3 */






#endif /* CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_CONTENT_STORE_LRU_LIST_H_ */
