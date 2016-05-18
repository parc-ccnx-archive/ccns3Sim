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

#ifndef CCNS3SIM_APPLICATIONS_PRODUCER_CONSUMER_CCNX_CONTENT_REPOSITORY_H_
#define CCNS3SIM_APPLICATIONS_PRODUCER_CONSUMER_CCNX_CONTENT_REPOSITORY_H_


#include <map>
#include <vector>
#include <set>

#include "ns3/uinteger.h"
#include "ns3/object.h"

#include "ns3/random-variable-stream.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-name-builder.h"
#include "ns3/ccnx-buffer.h"
#include "ns3/ccnx-contentobject.h"

namespace ns3 {
namespace ccnx {
/**
 * @ingroup ccnx-apps
 * The content repository class is specific to the producer consumer app implementation.
 * This class when instantiated provides the entire set of possible content objects.
 * This is then used by the producer class to serve content objects from.
 * This is then used by the consumer class to generate random interest requests for the content objects.
 *
 */
class CCNxContentRepository : public SimpleRefCount<CCNxContentRepository>
{
public:
  /**
         * \brief virtual constructor
         * \returns pointer to clone of this CCNxContentRepository
         * This method is useful for applications to create a common repository
         * from which contentObjects are served and interests are created.
         * @param [in] Pointer to CCNxName a prefix for all the content objects to generate.
         * @param [in] contentObjectSize, the payload/size of each content object.
         * @param [in] contentObjetCount, the number of such content objects to create.
         */

  CCNxContentRepository ( Ptr <const CCNxName> repositoryPrefix,
                          uint32_t contentObjectSize,
                          uint32_t contentObjectCount);
/**
 * Destroy the object instantiated
 */
  virtual ~CCNxContentRepository ();


  /**
   * This method returns the size of the content Object given a prefix with which a repository was created.
   * @param [in] Ptr CCNxName the prefix with which a repository was created.
   */
  uint32_t GetContentObjectSize (  ) const;

  /**
   * This method returns the number of such content Object created with a given  prefix .
   * @param [in] Ptr CCNxName the prefix with which a repository was created.
   */
  uint32_t GetContentObjectCount (  ) const;


  /**
   * This method returns a random Name from repository
   *
   */
  Ptr <const CCNxName> GetRandomName (void);

  /**
   * This method returns the prefix with which the repository was constructed.
   */
  Ptr <const CCNxName> GetRepositoryPrefix (void) const;

  /**
   * This method will return a Content Object with no payload
   */
  Ptr<CCNxContentObject> GetContentObject (Ptr <const CCNxName>contentObjectName);

private:
  void AddNameToRepository (Ptr <const CCNxName> repositoryPrefix, Ptr <const CCNxName> objectName);
  void CreateRepository (Ptr <const CCNxName> repositoryPrefix,
                         uint32_t contentObjectSize,
                         uint32_t contentObjectCount);


  Ptr <UniformRandomVariable> m_uniformRandomVariable;

  Ptr <const CCNxName> m_repositoryPrefix;

  uint32_t m_objectSize;

  typedef std::vector < Ptr <CCNxContentObject> > ContentObjectList;
  ContentObjectList m_contentObjects;

};
}
}




#endif /* SRC_CCNS3SIM_APPLICATIONS_PRODUCER_CONSUMER_CCNX_CONTENT_REPOSITORY_H_ */
