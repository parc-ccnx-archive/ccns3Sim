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

#ifndef CCNS3SIM_HELPER_CCNX_FORWARDING_HELPER_H_
#define CCNS3SIM_HELPER_CCNX_FORWARDING_HELPER_H_

#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-forwarder
 *
 * Abstract base class for the helper that will install a specific CCNx forwarder
 * on a node.  For example, `CCNxFlatForwarderHelper` or `CCNxStandardForwarderHelper`.
 */
class CCNxForwardingHelper
{
public:
  CCNxForwardingHelper ();
  virtual ~CCNxForwardingHelper ();

  /**
   * This method is implemented by the concrete forwarding helper, for example
   * inside class CCNxStandardForwarderHelper.
   *
   * The Install function should Initialize the Layer 3 protocol so it is
   * ready to accept configuration calls from the routing protocol and
   * forwarder, if necessary.
   *
   * @param node The NS3 node to install the layer 3 protocol on
   */
  virtual void Install (Ptr<Node> node) const = 0;

  /**
   * Return the run time type id of the object that will be installed.
   *
   * @return The typeid of the object that will be installed.
   */
  virtual TypeId GetForwardingTypeId () const = 0;

  // ===== Methods to display statistics

  /**
   * Print every node's forwarding statistics with the specified delay from now
   *
   * Example:
   * @code
   * {
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   CCNxStandardForwarder forwarder;
   *   forwarder.PrintForwardingStatisticsAllNodesWithDealy (Time (Seconds (30)), trace);
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetForwardingHelper (forwarder);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the forwarding statistics.
   * @param [in] stream The stream to write to.
   */
  static void PrintForwardingStatisticsAllNodesWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream);

  /**
   * Print every node's forwarding statistics every printInterval delay.  Will keep scheduling
   * itself forever.
   *
   * Example:
   * @code
   * {
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   CCNxStandardForwarder forwarder;
   *   forwarder.PrintForwardingStatisticsAllNodesWithInterval (Time (Seconds (5)), trace);
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetForwardingHelper (forwarder);
   * }
   * @endcode
   *
   * @param [in] printInterval The interval at which to print the forwarding statistics.
   * @param [in] stream The stream to write to.
   */
  static void PrintForwardingStatisticsAllNodesWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream);

  /**
   * Print a specific node's forwarding statistics with the specified delay from now.
   *
   * Example:
   * @code
   * {
   *   NodeContainer nodes;
   *   nodes.Create(3);
   *
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   CCNxStandardForwarder forwarder;
   *   forwarder.PrintForwardingStatisticsWithDealy (Time (Seconds (30)), trace, nodes.Get(0));
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetForwardingHelper (forwarder);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the forwarding statistics.
   * @param [in] stream The stream to write to.
   * @param [in] node The node to print
    */
  static void PrintForwardingStatisticsWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Print a specific node's forwarding statistics every printInterval delay.  Will keep scheduling
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
   *   CCNxStandardForwarder forwarder;
   *   forwarder.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   forwarder.PrintForwardingStatisticsWithInterval (Time (Seconds (1)), trace, nodes.Get(0));
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetForwardingHelper (forwarder);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the forwarding statistics.
   * @param [in] stream The stream to write to.
   * @param [in] node The node to print
   */
  static void PrintForwardingStatisticsWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Prints the forwarding statistics at a specific node.  This method is scheduled by the static helper functions
   * such as `PrintForwardingStatisticsAllNodesWithDelay()` and `PrintForwardingStatisticsWithDelay()`.
   *
   * This results in a call to the routing protocol itself so the output format is determined
   * by the actual implementation.
   *
   * @param [in] stream The stream to write to
   * @param [in] node The node to print
   */
  static void PrintForwardingStatistics (Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Prints the forwarding statistics at a specific node.  This method is scheduled by the static helper functions
   * such as `PrintForwardingStatisticsAllNodesWithInterval()` and `PrintForwardingStatisticsWithInterval()`.  The method will re-schedule itself
   * with the specified delay.
   *
   * This results in a call to the routing protocol itself so the output format is determined
   * by the actual implementation.
   *
   * @param [in] node The node to print
   * @param [in] stream The stream to write to
   */
  static void PrintForwardingStatisticsInterval (Ptr<OutputStreamWrapper> stream, Ptr<Node> node, Time printInterval);

  // ===== Methods to display forwarding table

  /**
   * Print every node's forwarding table (FIB) with the specified delay from now
   *
   * Example:
   * @code
   * {
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   CCNxStandardForwarder forwarder;
   *   forwarder.PrintForwardingTableAllNodesWithDealy (Time (Seconds (30)), trace);
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetForwardingHelper (forwarder);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the forwarding table (FIB).
   * @param [in] stream The stream to write to.
   */
  static void PrintForwardingTableAllNodesWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream);

  /**
   * Print every node's forwarding table (FIB) every printInterval delay.  Will keep scheduling
   * itself forever.
   *
   * Example:
   * @code
   * {
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   CCNxStandardForwarder forwarder;
   *   forwarder.PrintForwardingTableAllNodesWithInterval (Time (Seconds (5)), trace);
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetForwardingHelper (forwarder);
   * }
   * @endcode
   *
   * @param [in] printInterval The interval at which to print the forwarding table (FIB).
   * @param [in] stream The stream to write to.
   */
  static void PrintForwardingTableAllNodesWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream);

  /**
   * Print a specific node's forwarding table (FIB) with the specified delay from now.
   *
   * Example:
   * @code
   * {
   *   NodeContainer nodes;
   *   nodes.Create(3);
   *
   *   Ptr<OutputStreamWrapper> trace = Create<OutputStreamWrapper> (&std::cout);
   *
   *   CCNxStandardForwarder forwarder;
   *   forwarder.PrintForwardingTableWithDealy (Time (Seconds (30)), trace, nodes.Get(0));
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetForwardingHelper (forwarder);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the forwarding table (FIB).
   * @param [in] stream The stream to write to.
   * @param [in] node The node to print
    */
  static void PrintForwardingTableWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Print a specific node's forwarding table (FIB) every printInterval delay.  Will keep scheduling
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
   *   CCNxStandardForwarder forwarder;
   *   forwarder.Set ("HelloInterval", TimeValue (Seconds (1)));
   *   forwarder.PrintForwardingTableWithInterval (Time (Seconds (1)), trace, nodes.Get(0));
   *
   *   CCNxStackHelper ccnxStack;
   *   ccnxStack.SetForwardingHelper (forwarder);
   * }
   * @endcode
   *
   * @param [in] printDelay The time delay to print the forwarding table (FIB).
   * @param [in] stream The stream to write to.
   * @param [in] node The node to print
   */
  static void PrintForwardingTableWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Prints the forwarding table (FIB) at a specific node.  This method is scheduled by the static helper functions
   * such as `PrintForwardingTableAllNodesWithDelay()` and `PrintForwardingTableWithDelay()`.
   *
   * This results in a call to the routing protocol itself so the output format is determined
   * by the actual implementation.
   *
   * @param [in] stream The stream to write to
   * @param [in] node The node to print
   */
  static void PrintForwardingTable (Ptr<OutputStreamWrapper> stream, Ptr<Node> node);

  /**
   * Prints the forwarding table (FIB) at a specific node.  This method is scheduled by the static helper functions
   * such as `PrintForwardingTableAllNodesWithInterval()` and `PrintForwardingTableWithInterval()`.  The method will re-schedule itself
   * with the specified delay.
   *
   * This results in a call to the routing protocol itself so the output format is determined
   * by the actual implementation.
   *
   * @param [in] node The node to print
   * @param [in] stream The stream to write to
   */
  static void PrintForwardingTableInterval (Ptr<OutputStreamWrapper> stream, Ptr<Node> node, Time printInterval);

};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_HELPER_CCNX_FORWARDING_HELPER_H_ */
