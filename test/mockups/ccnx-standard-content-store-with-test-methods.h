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

#ifndef CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_SSTANDARD_CONTENT_WITH_TEST_METHODS_STORE_H_
#define CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_SSTANDARD_CONTENT_WITH_TEST_METHODS_STORE_H_

#include <map>
#include "ns3/ccnx-content-store.h"
#include "ns3/ccnx-standard-content-store-entry.h"
#include "ns3/ccnx-standard-forwarder-work-item.h"
#include "ns3/ccnx-delay-queue.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-hash-value.h"
#include "ns3/log.h"

namespace ns3 {
namespace ccnx {

class CCNxStandardContentStoreWithTestMethods : public CCNxStandardContentStore
{
public:

  /**
   * Returns the number of content object in the hash map.
   *
   * @return The number of objects in the hash map.
   */
  virtual size_t GetMapByHashCount() const
  {
    return m_csByHash.size ();
  }
  /**
   * Returns the number of content object in the name map.
   *
   * @return The number of objects in the name map.
   */
  virtual size_t GetMapByNameCount() const
  {
    return m_csByName.size ();
  }
  /**
   * Returns the number of content object in the nameKeyid map.
   *
   * @return The number of objects in the nameKeyid map.
   */
  virtual size_t GetMapByNameKeyidCount() const
  {
      return m_csByNameKeyid.size ();
  }



};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_SSTANDARD_CONTENT_WITH_TEST_METHODS_STORE_H_ */
