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

#ifndef CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_FORWARDER_HELPER_H_
#define CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_FORWARDER_HELPER_H_

#include "ns3/object-factory.h"
#include "ns3/ccnx-forwarding-helper.h"
#include "ns3/nstime.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-standard-forwarder
 *
 * Installs the `CCNxStandardForwarder` protocol on a node as the `CCNxForwarder`.
 *
 * Example:
 * @code
 * {
 *     CCNxStandardForwarderHelper forwarderHelper;
 *     forwarderHelper.SetPitType(AcmePit::GetTypeId());
 *
 *     CCNxStackHelper ccnx;
 *     ccnx.SetForwardingHelper(forwarderHelper);
 *     ccnx.Install (nodes);
 *     ccnx.AddInterfaces (devices);
 * }
 * @endcode
 */
class CCNxStandardForwarderHelper : public CCNxForwardingHelper
{
public:
  CCNxStandardForwarderHelper ();
  virtual ~CCNxStandardForwarderHelper ();

  /**
   * Sets the constant of the layer delay.
   *
   * The layer delay is:
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * packetBytes\f$
   *
   * This value is set via the attribute "LayerDelayConstant".  The default is 1 usec.
   *
   * @see SetLayerDelaySlope, SetLayerDelayServers
   */
  void SetLayerDelayConstant (Time delay);

  /**
   * Sets the slope of the layer delay.
   *
   * The layer delay is:
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * packetBytes\f$
   *
   * This value is set via the attribute "LayerDelaySlope".  The default is 0.
   *
   * @see SetLayerDelayServers, SetLayerDelayConstant
   */
  void SetLayerDelaySlope (Time slope);

  /**
   * The number of parallel servers processing the input delay queue.
   * The more servers there are, the lower the amortized delay is.
   *
   * Each server has a layer delay of
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * packetBytes\f$
   *
   * This value is set via the attribute "LayerDelayServers".  The default is 1.
   *
   * @see SetLayerDelaySlope, SetLayerDelayConstant
   */
  void SetLayerDelayServers (unsigned serverCount);

  /**
   * Sets a custom PIT implementation by its `ObjectFactory`.  If not set,
   * the forwarder will use its default PIT type.
   *
   * @param factory The ObjectFactory to create a CCNxPit
   *
   * Example:
   * @code
   * {
       *     AcmePitFactory acmePitFactory;
       *     acmePitFactory.SetMaxEntries(1000);
       *
   *     CCNxStandardForwarderHelper forwarderHelper;
       *     forwarderHelper.SetPitFactory(acmePitFactory);
   *
   *     CCNxStackHelper ccnx;
   *     ccnx.SetForwardingHelper(forwarderHelper);
   *     ccnx.Install (nodes);
   *     ccnx.AddInterfaces (devices);
   * }
   * @endcode
   */
  void SetPitFactory (ObjectFactory factory);

  /**
       * Sets a custom FIB implementation by its `ObjectFactory`.  If not set,
   * the forwarder will use its default FIB type.
   *
       * @param factory The ObjectFactory to create a CCNxFib
   *
   * Example:
   * @code
   * {
       *     AcmeFibFactory acmeFibFactory;
       *     acmeFibFactory.SetMaxEntries(1000);
       *
   *     CCNxStandardForwarderHelper forwarderHelper;
       *     forwarderHelper.SetFibFactory(acmeFibFactory);
   *
   *     CCNxStackHelper ccnx;
   *     ccnx.SetForwardingHelper(forwarderHelper);
   *     ccnx.Install (nodes);
   *     ccnx.AddInterfaces (devices);
   * }
   * @endcode
   */
  void SetFibFactory (ObjectFactory factory);

  /**
       * Sets a custom ContentStore implementation by its `ObjectFactory`.  If not set,
   * the forwarder will use its default ContentStore type.
   *
       * @param factory The ObjectFactory to create a CCNxContentStore
   *
   * Example:
   * @code
   * {
       *     AcmeContentStoreFactory acmeContentStoreFactory;
       *     acmeContentStoreFactory.SetMaxEntries(1000);
       *
   *     CCNxStandardForwarderHelper forwarderHelper;
       *     forwarderHelper.SetContentStoreFactory(acmeContentStoreFactory);
   *
   *     CCNxStackHelper ccnx;
   *     ccnx.SetForwardingHelper(forwarderHelper);
   *     ccnx.Install (nodes);
   *     ccnx.AddInterfaces (devices);
   * }
   * @endcode
   */
  void SetContentStoreFactory (ObjectFactory factory);

  /**
   * This method is implemented by the concrete layer 3 helper, for example
   * inside class CCNxStandardForwarderHelper.
   *
   * @param node The NS3 node to install the layer 3 protocol on
   */
  virtual void Install (Ptr<Node> node) const;

  /**
   * Return the run time type id of the object that will be installed.
   *
   * @return The typeid of the object that will be installed.
   */
  virtual TypeId GetForwardingTypeId () const;

private:
  ObjectFactory m_factory;
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_FORWARDING_STANDARD_CCNX_STANDARD_FORWARDER_HELPER_H_ */
