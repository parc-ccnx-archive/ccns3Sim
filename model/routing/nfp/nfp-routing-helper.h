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

#ifndef CCNS3SIM_NFP_ROUTING_HELPER_H_H
#define CCNS3SIM_NFP_ROUTING_HELPER_H_H

#include "ns3/object-factory.h"
#include "ns3/ccnx-routing-helper.h"
#include "ns3/node-container.h"
#include "ns3/nfp-computation-cost.h"
#include "ns3/nfp-stats.h"

namespace ns3 {
namespace ccnx {

/**
 * \ingroup nfp-routing
 *
 * Implements the base class CCNxRoutingProtocol.
 *
 * Example:
 * @code
 * {
 *    CCNxStackHelper stack;
 *    stack.SetRoutingProtocol("ns3::ccnx::NfpRoutingProtocol");
 * }
 * @endcode
 */
class NfpRoutingHelper : public CCNxRoutingHelper
{
public:
  NfpRoutingHelper ();

  NfpRoutingHelper (NfpRoutingHelper const &copy);

  virtual ~NfpRoutingHelper ();

  virtual CCNxRoutingHelper * Copy (void) const;

  /**
   * \param node the node within which the new routing protocol will run
   * \returns a newly-created routing protocol
   */
  virtual Ptr<CCNxRoutingProtocol> Create (Ptr<Node> node) const;

  /**
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set.
   *
   * This method controls the attributes of ns3::ccnx::NfpRoutingProtocol.  See its
   * TypeId implementation for a complete list lof attributes.
   *
   * Example:
   * @code
   *
   * {
   *  NodeContainer nc;
   *  nc.create(3);
   *
   *  NfpRoutingHelper helper;
   *  helper.Set("HelloInterval", TimeValue (Seconds (1)));
   *
   *  Ptr<CCNxRoutingProtocol> n3-rtr = Create(nc.Get(3));
   *
   * }
   * @endcode
   */
  void Set (std::string name, const AttributeValue &value);


  /**
   * Assign random number streams
   */
  int64_t SetSteams (NodeContainer &c, int64_t stream);

  /**
   * Exclude an interface from NFP protocol operation
   */
  //void ExcludeInterface (Ptr<Node> node, uint32_t interface);

  static void PrintComputationCostAllNodesWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream);
  static void PrintComputationCostWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);
  static void PrintComputationCost (Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Retrieve the computation cost associated with NFP from the specified node.
   *
   * This will fetch the aggregated instance of NfpRoutingProtocol from the node
   * and return its computation cost.
   *
   * @param node [in] The node to lookup
   */
  static NfpComputationCost GetComputationCost( Ptr<Node> node );

  /**
   * Returns the routing statistics from time 0 to now.
   *
   * @param node [in] The node to lookup
   * @return
   */
  static NfpStats GetStats( Ptr<Node> node );

private:
  /**
   * The factory we use to create protocol instances
   */
  ObjectFactory m_routerFactory;

};
}
}

#endif //CCNS3SIM_NFP_ROUTING_HELPER_H_H
