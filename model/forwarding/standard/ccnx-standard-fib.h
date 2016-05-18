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


#ifndef CCNS3SIM_CCNXSTANDARDFIB_H
#define CCNS3SIM_CCNXSTANDARDFIB_H

#include <map>
#include <vector>
#include <list>

#include "ns3/ccnx-fib.h"
#include "ns3/ccnx-forwarder.h"
#include "ns3/ccnx-standard-fibEntry.h"
#include "ns3/ccnx-delay-queue.h"
#include "ns3/ccnx-standard-forwarder-work-item.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-standard-forwarder
 *
 * The CCNx standard Fib class is the top level of the  Fib object hierarchy.
 * CCNx standard fib is derived from `CCNxFib` base class.
 * This fib should work for all implementations but may be replaced if desired.
 */
class CCNxStandardFib : public CCNxFib
{
protected:
  /**
  * Standard fib consists of an STL map (Key=name object ptr, Value=fibEntry object ptr) and associated methods.
  *
  * The fib contains one entry for each routed name.
  * The fibEntry object contents are defined in the StandardFibEntry class.
  * The standard-fib class makes no assumptions about the structure of the fib entry.
  * A fib entry could contain more than one next hop, as well as weights or other information.
  * The standard-fibEntry class defines the structure of the fib entry.
  * This class is derived from object with a GetTypeId method to enable run-time replacement.
  * The comparePtrCCNxName operator is used for map key compare operations. It compares the name segments.
  * Internally the stl map is typically implemented as a binary tree.
  *
  */
  typedef std::map<Ptr<const CCNxName>, Ptr<CCNxStandardFibEntry>, CCNxName::isLessPtrCCNxName >  FibType;

  FibType fib;

public:
  CCNxStandardFib ();

  virtual ~CCNxStandardFib ();

  /**
   *  Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  //! @copydoc CCNxFib::RemoveConnection(Ptr<CCNxConnection>)
  virtual bool RemoveConnection (Ptr<CCNxConnection>  connection);

  /*! @copydoc CCNxFib::Lookup
   *
   */
  virtual void Lookup (Ptr<CCNxForwarderMessage> message);

  /**
   *
   * @copydoc CCNxFib::SetLookupCallback()
   */
  virtual void SetLookupCallback (LookupCallback lookupCallback);

  /**
   *
   * @copydoc CCNxFib::AddRoute()
   */
  virtual bool AddRoute (Ptr<const CCNxName> ccnxName,  Ptr<CCNxConnection>  connection);

  virtual bool DeleteRoute (Ptr<const CCNxName> ccnxName,  Ptr<CCNxConnection>  connection);

  virtual std::ostream & PrintRoutes (std::ostream & os);

  virtual std::ostream & PrintRoute (std::ostream & os, Ptr<const CCNxName> ccnxName);

  /**
   *  Count total number of entries in fib.
   *
   */
  virtual int CountEntries ();

protected:
  // ns3::Object::DoInitialize()
  void DoInitialize ();

private:
  /**
   * Perform the actual route lookup.
   *
   * This is called within the `GetServiceTime()` function.  It returns the number of
   * name components that we had to process.
   *
   * Side effect: The result of the FIB lookup will be put in the CCNxStandardForwarderWorkItem's egressConnections.
   *
   * @param message [in] The forwarder message to lookup in the FIB (must be CCNxInterest)
   * @param egress [in] Must be an allocated CCNxConnectionList.  The egress connections from the FIB lookup will go here.
   *
   * @return The number of name components processed.
   */
  unsigned PerformLookup (Ptr<CCNxStandardForwarderWorkItem> workItem);

  /**
   * Set by the forwarder as the callback for Lookup
   */
  LookupCallback m_LookupCallback;

  /**
   * The storage type of the CCNxDelayQueue
   */
  typedef CCNxDelayQueue<CCNxForwarderMessage> DelayQueueType;

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
  Time GetServiceTime (Ptr<CCNxForwarderMessage> item);

  /**
   * Callback from delay queue after a work item has waited its service time
   *
   * @param item [in] The work item to service
   */
  void DequeueCallback (Ptr<CCNxForwarderMessage> item);

  /**
   * The layer delay is a linear function of the number of name components looked up.
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * nameComponents\f$
   *
   * It is set via the attribute "LayerDelayConstant".  The default is 1 usec.
   */
  Time m_layerDelayConstant;

  /**
   * The layer delay is a linear function of the number of name components looked up.
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * nameComponents\f$
   *
   * It is set via the attribute "LayerDelaySlope".  The default is 0.
   */
  Time m_layerDelaySlope;

  /**
   * The number of parallel servers processing the input delay queue.
   * The more servers there are, the lower the amortized delay is.
   *
   * This value is set via the attribute "LayerDelayServers".  The default is 1.
   */
  unsigned m_layerDelayServers;


};          //class
}    //namespace
} //namespace







#endif //CCNS3SIM_CCNXSTANDARDFIB_H
