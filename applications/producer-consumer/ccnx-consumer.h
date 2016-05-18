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

#ifndef CCNS3_CCNXCONSUMER_H
#define CCNS3_CCNXCONSUMER_H

#include <map>
#include <vector>

#include "ns3/object.h"
#include "ns3/nstime.h"
#include "ns3/timer.h"

#include "ns3/string.h"

#include "ns3/random-variable-stream.h"

#include "ns3/node-container.h"
#include "ns3/application.h"
#include "ns3/application-container.h"

#include "ns3/ccnx-name.h"
#include "ns3/ccnx-name-builder.h"
#include "ns3/ccnx-contentobject.h"
#include "ns3/ccnx-application.h"
#include "ns3/ccnx-portal.h"
#include "ns3/ccnx-content-repository.h"

namespace ns3 {
namespace ccnx {
/**
  * @ingroup ccnx-apps
  *
  * The consumer class generate interests randomly for a given prefix.
  * The prefix, number of objects to express interests for are determined by the ContentRepositoryPtr that is
  * shared with the producer class.
  * The consumer class is derived from the CCNxApplication class which in turn is derived from ns3::Application.
  * The interests generated are routed according FIB entry.
  *
  */

class CCNxConsumer : public CCNxApplication
{
public:
  static TypeId GetTypeId (void);
  /**
   * \brief virtual constructor
   * \returns pointer to clone of this CCNxConsumer
   * This method is used by helper to generate interests.
   */
  CCNxConsumer (void);
  /**
   * Virtual method to destroy an instance of a CCNxConsumer
   */
  virtual ~CCNxConsumer ();

  /*
    * SetContentRepository:
    * A repository to which this consumer is associated with is set by this method.
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
    *     Ptr <CCNxConsumer> ccnxProducer = Create<CCNxConsumer>();
    *     ccnxProducer->SetContentRepository(globalContentRepository);
    * }
    * @endcode
    *
    */
  void SetContentRepository (Ptr<CCNxContentRepository> repositoryPtr);

private:
  /**
   *
   * This is inherited from the base class ns3::Application
   */
  virtual void StartApplication (void);
  /**
   *
   * This is inherited from the base class ns3::Application
   */
  virtual void StopApplication (void);
  /**
   * This private method is a registered callback to portal. Portal will invoke this callback function every time a packet is received.
   * The ConsumerClass will decode the message to check if this were an interest or content. Ofcourse we are not expecting an interest
   * but upon receiving content we will then strike it out from our OutStandingInterest list.
   *
   * @param [in] Pointer to CCNxPortal class
   */
  void ReceiveCallback (Ptr<CCNxPortal> portal);
  /**
   * THis is a private method to insert an interest to m_outstandingRequests invoked by GenerateTraffic method.
   */
  void InsertOutStandingInterest (Ptr<const CCNxName> interest);
  /**
   * THis is a private method to erase an interest from m_outstandingRequests invoked by ReceiveCallback method.
   */
  void RemoveOutStandingInterest (Ptr<const CCNxName> interest);
  /**
   * THis is a private method invoked by StopApplication() to see if there were any interests that were not honored by
   * the network.
   */
  bool FindOutStandingInterest (Ptr<const CCNxName> interest);
  /**
   * THis is a private method to pick a random name CCNxContentRepository::GetRandomName() convert this into an
   * interest and then send it out of the portal m_consumerPortal. This method is invoked periodically determined by
   * m_requestInterval. m_requestInterval is an attribute that is set via helper class (CCNxConsumerHelper) in milliseconds.
   * GenerateTraffic() will continue to request interests until the StopApplication() is invoked.
   */
  void GenerateTraffic ();

  Ptr<CCNxPortal> m_consumerPortal;
  Time m_requestInterval;
  Timer m_requestIntervalTimer;
#if 0
  typedef std::set<Ptr<const CCNxName>, CCNxName::isLessPtrCCNxName> NameIndexLookupType;
  NameIndexLookupType m_outstandingRequests;
#else
  	typedef struct {
  		Ptr <const CCNxName> interestName;
  		uint64_t       txTime;
  	}OutstandingRequestEntryType;

  	typedef std::map < Ptr <const CCNxName> , OutstandingRequestEntryType , CCNxName::isLessPtrCCNxName > OutstandingRequestType;
  	OutstandingRequestType m_outstandingRequests;
#endif
  Ptr<CCNxContentRepository> m_globalContentRepositoryPrefix;

  /**
   * Statistics and a show method to display them.
   */
  void ShowStatistics ();
  uint32_t m_goodInterestsSent;
  uint32_t m_goodContentReceived;
  uint32_t m_interestProcessFails;
  uint32_t m_contentProcessFails;
  uint64_t m_count;
  uint64_t m_sum;
  uint64_t m_sumSquare;
};
}
}

#endif //CCNS3_CCNXCONSUMER_H
