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

#ifndef CCNS3SIM_CCNXSTANDARDFORWARDER_H
#define CCNS3SIM_CCNXSTANDARDFORWARDER_H

#include <map>
#include "ns3/object-factory.h"
#include "ns3/ccnx-forwarder.h"

#include "ns3/ccnx-delay-queue.h"
#include "ns3/ccnx-standard-forwarder-work-item.h"

#include "ns3/ccnx-pit.h"
#include "ns3/ccnx-fib.h"
#include "ns3/ccnx-content-store.h"


namespace ns3 {
namespace ccnx {

/**
 * @defgroup ccnx-standard-forwarder Standard Forwarder
 * @ingroup ccnx-forwarder
 */

/**
 * @ingroup ccnx-standard-forwarder
 *
 * CCNx protocol Standards-compliant Forwarder containing pit and fib.
 * returns next hop list for packets from L4 or L2. Note: Sending the packet is
 * the responsibility of the calling routine.
*
* Implementation compliant with the IRTF ICNRG research group documents.
*/
class CCNxStandardForwarder : public CCNxForwarder
{
public:
  static TypeId GetTypeId ();

  CCNxStandardForwarder ();
  virtual ~CCNxStandardForwarder ();

  virtual void RouteOutput (Ptr<CCNxPacket> packet,
                            Ptr<CCNxConnection> ingressConnection,
                            Ptr<CCNxConnection> egressConnection);

  virtual void RouteInput (Ptr<CCNxPacket> packet,
                           Ptr<CCNxConnection> ingressConnection);

  virtual bool AddRoute (Ptr<CCNxConnection> connection, Ptr<const CCNxName> name);

  virtual bool AddRoute (Ptr<const CCNxRoute> route);

  virtual bool RemoveRoute (Ptr<CCNxConnection> connection, Ptr<const CCNxName> name);

  virtual bool RemoveRoute (Ptr<const CCNxRoute> route);

  virtual void PrintForwardingTable (Ptr<OutputStreamWrapper> stream) const;

  virtual void PrintForwardingStatistics (Ptr<OutputStreamWrapper> stream) const;

  typedef enum
  {
    PitTable,FibTable
  } TableTypes;

  /**
   *  Count total number of entries in pit or fib.
   *
   * \param t Table to be used (PitTable or FibTable).
   */

  virtual int CountEntries (TableTypes t);




private:

  typedef struct _stats {
    size_t packetsIn;			/*!< total number of packets from CCNxL3Protocol */
    size_t packetsOut;			/*!< total number of packets returned to CCNxL3Protocol */

    size_t dropUnsupportedPacketType;	/*!< packets dropped due to unsupported PacketType */

    size_t interestsToPit;		/*!< number of interest sent to PIT */
    size_t interestsVerdictForward;	/*!< Number of interests from PIT to forward */
    size_t interestsVerdictAggregate;	/*!< Number of interests from PIT to aggregate */

    size_t interestsToContentStore;	/*!< Number of interests sent to CS */
    size_t interestsContentStoreHits;	/*!< Number of interests with hit in CS */
    size_t interestsContentStoreMisses;	/*!< Number of interests with miss in CS */

    size_t interestsToFib;		/*!< Number of interests sent to FIB */
    size_t interestsFibForwarded;	/*!< Number of interests with positive size connection list */
    size_t interestsFibNotForwarded;	/*!< Number of interests with zero size connection list */

    size_t contentObjectsToPit;			/*!< number of content objects sent to PIT */
    size_t contentObjectsMatchedInPit;		/*!< Number of content objects matched (positive size connection list) */
    size_t contentObjectsNotMatchedInPit;	/*!< Number of content objects not matched (zero size connection list) */

    size_t contentObjectsToContentStore;  	/*!< Number of content objects sent to Content Store */
    size_t contentObjectsAddedToContentStore;	/*!< Number of content objects added to Content Store */
    size_t contentObjectsNotAddedToContentStore; /*!< Number of content objects Not Added to Content Store */
  } ForwarderStats;

  ForwarderStats m_forwarderStats;

  /**
   *  common function for all AddRoute functions.
   *
   * \param connection Pointer to connection object.
   * \param name Pointer to name object.
   * \return true if route added, false otherwise
   *
   */
  bool InnerAddRoute (Ptr<CCNxConnection> connection, Ptr<const CCNxName> name);

  /**
   *  common function for all RemoveRoute functions.
   *
   * \param connection Pointer to connection object.
   * \param name Pointer to name object.
   * \return true if route removed, false otherwise
   *
   */
  bool InnerRemoveRoute (Ptr<CCNxConnection> connection, Ptr<const CCNxName> name);

  /**
   *   cleanup function - currently void - called by deconstructor?
   *
   */
  virtual void DoDispose (void);

  /**
   *   initialization function  - selects pit and fib type, then creates pit and fib.
   *   called by Initialize().
   *
   *
   */
  virtual void DoInitialize (void);


  /**
   * The pit, the kind of which is selected and instantiated during DoInitialize.
   */
  Ptr<CCNxPit> m_pit;
  /**
   * The fib, the kind of which is selected and instantiated during DoInitialize.
   */
  Ptr<CCNxFib> m_fib;
  /**
   * The contentStore, the kind of which is selected and instantiated during DoInitialize.
   */
  Ptr<CCNxContentStore> m_contentStore;

  /**
   * The factory to create a PIT.  Maybe set by the attribute "PitFactory".
   * Defaults to `CCNxStandardPitFactory`.
   */
  ObjectFactory m_pitFactory;

  /**
   * The factory to create a FIB.  Maybe set by the attribute "FibFactory".
   * Defaults to `CCNxStandardFibFactory`.
   */
  ObjectFactory m_fibFactory;




  /**
   * The factory to create a ContentStore.  Maybe set by the attribute "ContentStoreFactory".
   * Defaults to No Content Store. To use the standard Content Store, set the attribute as
   * follows in the code which generates the standardForwarder:
   * @code
   * {
   *   static CCNxStandardContentStoreFactory factory;
   *   forwarder->SetAttribute ("ContentStoreFactory", ObjectFactoryValue (factory));
   * }
   * @endcode
   */
  ObjectFactory m_contentStoreFactory;

  /**
   * This function is called by the base class CCNxL3Protocol
   */
  //TODO CCN make these ccnxPacket parameters const

  /**
   * Operator used for FIB and PIT name comparison.
   */
  struct comparePtrCCNxName
  {
    bool operator() (const Ptr<const CCNxName> a, const Ptr<const CCNxName> b) const
    {
      return *a < *b;
    }
  };

  /**
   * The storage type of the CCNxDelayQueue
   */
  typedef CCNxDelayQueue<CCNxStandardForwarderWorkItem> DelayQueueType;

  /**
   * Input queue used to simulate processing delay
   */
  Ptr<DelayQueueType> m_inputQueue;

  /**
   * Callback from delay queue to compute the service time of a work item
   *
   * @param item [in] The work item being serviced
   * @return The service time of the work item
   */
  Time GetServiceTime (Ptr<CCNxStandardForwarderWorkItem> item);

  /**
   * Callback from delay queue after a work item has waited its service time
   *
   * @param item [in] The work item to service
   */
  void ServiceInputQueue (Ptr<CCNxStandardForwarderWorkItem> item);

  /**
   * The layer delay is:
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * packetBytes\f$
   *
   * It is set via the attribute "LayerDelayConstant".  The default is 1 usec.
   */
  Time m_layerDelayConstant;

  /**
   * The layer delay is:
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * packetBytes\f$
   *
   * It is set via the attribute "LayerDelaySlope".  The default is 0.
   */
  Time m_layerDelaySlope;

  /**
   * The number of parallel servers processing the input delay queue.
   * The more servers there are, the lower the amortized delay is.
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * packetBytes\f$
   *
   * This value is set via the attribute "LayerDelayServers".  The default is 1.
   */
  unsigned m_layerDelayServers;


  /**
   * Asynchronous callback from the Content Store after servicing a MatchInterest call.
   *
   * @param message [in] The forwarder message being routed (CCNxStandardForwarderWorkItem)
   */
  void ContentStoreMatchInterestCallback (Ptr<CCNxForwarderMessage> message);

  /**
   * Asynchronous callback from the Content Store after servicing a AddContentObject call.
   *
   * @param message [in] The content object message that was added (CCNxStandardForwarderWorkItem)
   */
  void ContentStoreAddContentObjectCallback (Ptr<CCNxForwarderMessage> message);

  /**
   * Asynchronous callback from the PIT after servicing a ReceiveInterest call.
   *
   * @param message [in] The forwarder message being routed (CCNxStandardForwarderWorkItem)
   * @param verdict [in] The PIT verdict for the Interest
   */
  void PitReceiveInterestCallback (Ptr<CCNxForwarderMessage> message, enum CCNxPit::Verdict vedict);

  /**
   * Asynchronous callback from the PIT after servicing a SatisfyInterest call.
   *
   * @param message [in] The forwarder message being routed (CCNxStandardForwarderWorkItem)
   * @param egressConnections [in] The egress connections to forward the ContentObject on (may be empty)
   */
  void PitSatisfyInterestCallback (Ptr<CCNxForwarderMessage> message, Ptr<CCNxConnectionList> egressConnections);

  /**
   * Asynchronous callback from the FIB after servicing a SatisfyInterest call.
   *
   * @param message [in] The forwarder message being routed (CCNxStandardForwarderWorkItem)
   * @param egressConnections [in] The egress connections to forward the ContentObject on (may be empty)
   */
  void FibLookupCallback (Ptr<CCNxForwarderMessage> message, Ptr<CCNxConnectionList> egressConnections);

  /**
   * Called at the end of processing a `RouteInput()` or `RouteOutput()`.
   *
   * Will determine the route error, if any, and call the `m_routeCallback`.
   *
   * @param message [in] The work item begin forwarded
   */
  void FinishRouteLookup (Ptr<CCNxStandardForwarderWorkItem> item, Ptr<CCNxConnectionList> egressConnections);
};
}
}

#endif //CCNS3SIM_CCNXSTANDARDFORWARDER_H
