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

#ifndef CCNS3_CCNXPRODUCER_H
#define CCNS3_CCNXPRODUCER_H

#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "ns3/ccnx-application.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-name-builder.h"
#include "ns3/ccnx-portal.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/object-factory.h"
#include "ns3/ptr.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/random-variable-stream.h"
#include "ns3/ccnx-content-repository.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-apps
 *
 * The producer class is the custodian of content objects.
 * The producer class is derived from the CCNxApplication class which in turn is derived from ns3::Application.
 * The content objects are generated using the repository class and passed on to the producer class.
 * The producer class will honor matching interests with content objects as responses.
 *
 */


class CCNxProducer : public CCNxApplication
{
public:
  static TypeId GetTypeId (void);
  virtual ~CCNxProducer ();
  CCNxProducer ();

  /*
  * SetContentRepository:
  * A repository to which this producer is associated with is set by this method.
  *
  * @param [in] Pointer to CCNxContentRepository
  *
  * @return void
  *
  * Example:
  * @code
  * {
  *     Ptr <const CCNxName> prefix = Create <CCNxName>("ccnx:/name=simple/name=producer/name=consumer");
  *     uint32_t size = 124 ;
  *     uint32_t count = 10;
  *     Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository>(prefix,size,count);
  *     Ptr <CCNxProducer> ccnxProducer = Create<CCNxProducer>();
  *     ccnxProducer->SetContentRepository(globalContentRepository);
  * }
  * @endcode
  *
  */
  void SetContentRepository (Ptr <CCNxContentRepository> repositoryPtr);
private:
  /**
   *
   * This is inherited from the base class ns3::Application
   */
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * This private method is a registered callback to portal.
   * Portal will invoke this callback function every time a packet is received.
   * The ProducerClass will decode the message to check (interest / content ?)
   * It will respond back to the interest if valid with a content object.
   *
   * @param [in] Pointer to CCNxPortal class
   */
  void ReceiveCallback (Ptr<CCNxPortal> portal);

  /**
   * Portal for producer to send and receive data.
   */
  Ptr <CCNxPortal> m_producerPortal;

  /**
   * Pointer to Repository shared between Producer and Consumer. This is filled out by helper routines.
   */
  Ptr <CCNxContentRepository> m_globalContentRepositoryPrefix;

  /**
   * Statistics and counters for debugging.
   */
  void ShowStatistics ();
  uint32_t m_goodInterestsReceived;
  uint32_t m_goodContentServed;
  uint32_t m_interestProcessFails;
  uint32_t m_contentProcessFails;

};
}
}

#endif //CCNS3_CCNXPRODUCER_H
