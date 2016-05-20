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

#ifndef CCNS3_CCNXROUTINGHELPER_H
#define CCNS3_CCNXROUTINGHELPER_H

#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/ccnx-routing-protocol.h"

namespace ns3 {


class Node;

namespace ccnx {

class CCNxRoutingProtocol;
class CCNxListRouting;

/**
 * @ingroup ccnx-routing
 *
 * \brief a factory to create ns3::ccnx::CCNxRoutingProtocol on a node.
 *
 * This is a base class for a specific routing helper, such as CCNxStaticRoutingHelper
 * or NfpRoutingHelper.
 *
 * To use a derived class, you assign it to CCNxStackHelper::SetRoutingHelper().
 *
 */
class CCNxRoutingHelper
{
public:
  /*
   * Destroy an instance of an CCNxRoutingHelper
   */
  virtual ~CCNxRoutingHelper ();

  /**
   * \brief virtual constructor
   * \returns pointer to clone of this CCNxRoutingHelper
   *
   * This method is mainly for internal use by the other helpers;
   * clients are expected to free the dynamic memory allocated by this method
   */
  virtual CCNxRoutingHelper * Copy (void) const = 0;

  /**
   * \param node the node within which the new routing protocol will run
   *
   * The Create() method should initialize the routing protocol (call ns3::Object::Initialize())
   * on the instantiated object.
   *
   * \returns a newly-created routing protocol
   */
  virtual Ptr<CCNxRoutingProtocol> Create (Ptr<Node> node) const = 0;

  /**
   */
  template<class T>
  static Ptr<T> GetRouting (Ptr<CCNxRoutingProtocol> protocol);

  // ===== Methods to display tables

  /**
   * Print every node's routing table with the specified delay from now
   *
   * Example:
   * @code
   * {
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   NfpRoutingHelper nfpHelper;
   *   nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   nfpHelper.PrintRoutingTableAllNodesWithDealy (Time (Seconds (30)), trace);
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetRoutingHelper (nfpHelper);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the routing table.
   * @param [in] stream The stream to write to.
   */
  static void PrintRoutingTableAllNodesWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream);

  /**
   * Print every node's routing table every printInterval delay.  Will keep scheduling
   * itself forever.
   *
   * Example:
   * @code
   * {
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   NfpRoutingHelper nfpHelper;
   *   nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   nfpHelper.PrintRoutingTableAllNodesWithInterval (Time (Seconds (5)), trace);
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetRoutingHelper (nfpHelper);
   * }
   * @endcode
   *
   * @param [in] printInterval The interval at which to print the routing table.
   * @param [in] stream The stream to write to.
   */
  static void PrintRoutingTableAllNodesWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream);

  /**
   * Print a specific node's routing table with the specified delay from now.
   *
   * Example:
   * @code
   * {
   *   NodeContainer nodes;
   *   nodes.Create(3);
   *
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   NfpRoutingHelper nfpHelper;
   *   nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   nfpHelper.PrintRoutingTableWithDealy (Time (Seconds (30)), trace, nodes.Get(0));
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetRoutingHelper (nfpHelper);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the routing table.
   * @param [in] stream The stream to write to.
   * @param [in] node The node to print
    */
  static void PrintRoutingTableWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Print a specific node's routing table every printInterval delay.  Will keep scheduling
   * itself forever.
   *
   * Example:
   * @code
   * {
   *   NodeContainer nodes;
   *   nodes.Create(3);
   *
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   NfpRoutingHelper nfpHelper;
   *   nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   nfpHelper.PrintRoutingTableWithInterval (Time (Seconds (1)), trace, nodes.Get(0));
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetRoutingHelper (nfpHelper);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the routing table.
   * @param [in] stream The stream to write to.
   * @param [in] node The node to print
   */
  static void PrintRoutingTableWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Print every node's neighbor table at the specified time.
   *
   * Example:
   * @code
   * {
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   NfpRoutingHelper nfpHelper;
   *   nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   nfpHelper.PrintRoutingTableAllNodesWithDelay (Time (Seconds (5)), trace);
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetRoutingHelper (nfpHelper);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the neighbor table.
   * @param [in] stream The stream to write to.
   */
  static void PrintNeighborTableAllNodesWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream);

  /**
   * Print every node's neighbor table at every interval.
   *
   * Example:
   * @code
   * {
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   NfpRoutingHelper nfpHelper;
   *   nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   nfpHelper.PrintNeighborTableAllNodesWithInterval (Time (Seconds (5)), trace);
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetRoutingHelper (nfpHelper);
   * }
   * @endcode
   *
   * @param [in] printDelay The interval to print the neighbor table.
   * @param [in] stream The stream to write to.
   */
  static void PrintNeighborTableAllNodesWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream);

  /**
   * Print all node's neighbor table at the specified time.
   *
   * Example:
   * @code
   * {
   *   NodeContainer nodes;
   *   nodes.Create(3);
   *
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   NfpRoutingHelper nfpHelper;
   *   nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   nfpHelper.PrintNeighborTableWithDealy (Time (Seconds (30)), trace, nodes.Get(0));
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetRoutingHelper (nfpHelper);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the neighbor table.
   * @param [in] stream The stream to write to.
   */
  static void PrintNeighborTableWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Print a specific node's neighbor table every printInterval delay.  Will keep scheduling
   * itself forever.
   *
   * Example:
   * @code
   * {
   *   NodeContainer nodes;
   *   nodes.Create(3);
   *
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   NfpRoutingHelper nfpHelper;
   *   nfpHelper.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   nfpHelper.PrintNeighborTableAllNodesWithInterval (Time (Seconds (1)), trace, nodes.Get(0));
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetRoutingHelper (nfpHelper);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the neighbor table.
   * @param [in] stream The stream to write to.
   * @param [in] node The node to print
   */
  static void PrintNeighborTableWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);




private:
  /**
   * Prints the routing table at a specific node.  This method is scheduled by the static helper functions
   * such as PrintRoutingTableAllDelay() and PrintRoutingTableDelay().
   *
   * This results in a call to the routing protocol itself so the output format is determined
   * by the actual implementation.
   *
   * @param [in] stream The stream to write to
   * @param [in] node The node to print
   */
  static void PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Prints the routing table at a specific node.  This method is scheduled by the static helper functions
   * such as PrintRoutingTableAllEvery() and PrintRoutingTableEvery().  The method will re-schedule itself
   * with the specified delay.
   *
   * This results in a call to the routing protocol itself so the output format is determined
   * by the actual implementation.
   *
   * @param [in] node The node to print
   * @param [in] stream The stream to write to
   */
  static void PrintRoutingTableInterval (Ptr<OutputStreamWrapper> stream, Ptr<Node> node, Time printInterval);

  /**
   * Prints the neighbor table at a specific node.  This method is scheduled by the static helper functions
   * such as PrintNeighborTableAllDelay() and PrintNeighborTableDelay().
   *
   * This results in a call to the routing protocol itself so the output format is determined
   * by the actual implementation.
   *
   * @param [in] stream The stream to write to
   * @param [in] node The node to print
   */
  static void PrintNeighborTable (Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Prints the neighbor table at a specific node.  This method is scheduled by the static helper functions
   * such as PrintNeighborTableAllEvery() and PrintNeighborTableEvery().  The method will re-schedule itself
   * with the specified delay.
   *
   * This results in a call to the routing protocol itself so the output format is determined
   * by the actual implementation.
   *
   * @param [in] node The node to print
   * @param [in] stream The stream to write to
   */
  static void PrintNeighborTableInterval (Ptr<OutputStreamWrapper> stream, Ptr<Node> node, Time printInterval);

};

}     // namespace ccnx
} // namespace ns3



#endif //CCNS3_CCNXROUTINGHELPER_H
