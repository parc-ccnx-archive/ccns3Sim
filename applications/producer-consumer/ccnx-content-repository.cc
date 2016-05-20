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

#include "ns3/log.h"
#include "ns3/ccnx-content-repository.h"

using namespace ns3;
using namespace ns3::ccnx;
NS_LOG_COMPONENT_DEFINE ("CCNxContentRepository");

CCNxContentRepository::CCNxContentRepository (
  Ptr<const CCNxName> repositoryPrefix, uint32_t contentObjectSize,
  uint32_t contentObjectCount)
{
  NS_LOG_FUNCTION (
    this << repositoryPrefix << contentObjectSize << contentObjectCount);
  m_uniformRandomVariable = CreateObject<UniformRandomVariable> ();
  CreateRepository (repositoryPrefix, contentObjectSize, contentObjectCount);
}


CCNxContentRepository::~CCNxContentRepository ()
{
}

void
CCNxContentRepository::CreateRepository (Ptr<const CCNxName> repositoryPrefix,
                                         uint32_t contentObjectSize,
                                         uint32_t contentObjectCount)
{
  NS_LOG_FUNCTION (
    this << repositoryPrefix << contentObjectSize << contentObjectCount);
  char buffer[8];

  m_repositoryPrefix = repositoryPrefix;
  m_objectSize = contentObjectSize;
  for (uint32_t ii = 0; ii < contentObjectCount; ii++)
    {
      CCNxNameBuilder nameBuilder (*repositoryPrefix);
      snprintf (buffer, sizeof(buffer), "%d", ii);
      nameBuilder.Append (
        Create<CCNxNameSegment> (CCNxNameSegment_Chunk, buffer));
      Ptr<CCNxContentObject> contentObject = Create<CCNxContentObject> (
          nameBuilder.CreateName ());
      m_contentObjects.push_back (contentObject);
      NS_LOG_DEBUG ("Added name " << *nameBuilder.CreateName () << "to repository " << *repositoryPrefix);
    }
}

Ptr<CCNxContentObject>
CCNxContentRepository::GetContentObject (Ptr<const CCNxName> interestName)
{
  NS_LOG_FUNCTION (this << interestName);

  if (m_repositoryPrefix->IsPrefixOf (*interestName))
    {
      for (ContentObjectList::iterator ii = m_contentObjects.begin (); ii != m_contentObjects.end (); ++ii)
        {
          Ptr<CCNxContentObject> contentResponse  = *ii;
          if (interestName->Equals (*contentResponse->GetName ()))
            {
              return contentResponse;
            }
        }
    }

  return Ptr <CCNxContentObject> (0);
}

Ptr<const CCNxName>
CCNxContentRepository::GetRepositoryPrefix () const
{
  NS_LOG_FUNCTION (this);
  return m_repositoryPrefix;
}

uint32_t
CCNxContentRepository::GetContentObjectSize ( ) const
{
  NS_LOG_FUNCTION (this);
  return m_objectSize;
}

uint32_t
CCNxContentRepository::GetContentObjectCount ( ) const
{
  NS_LOG_FUNCTION (this);
  return m_contentObjects.size ();
}

Ptr<const CCNxName>
CCNxContentRepository::GetRandomName ()
{
  NS_LOG_FUNCTION (this);
  uint32_t randIndex = m_uniformRandomVariable->GetInteger (
      0, (m_contentObjects.size () - 1));
  Ptr <const CCNxName> randName = m_contentObjects[randIndex]->GetName ();
  NS_LOG_DEBUG ("randName is " << *randName);
  return randName;
}

