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

#ifndef CCNS3SIM_HELPER_CCNX_ASCII_TRACE_HELPER_H_
#define CCNS3SIM_HELPER_CCNX_ASCII_TRACE_HELPER_H_

#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/packet.h"
#include "ns3/ccnx-packet.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx
 *
 * A helper class to setup tracing for use with the CCNxStackHelper.  We follow the IPv4 model
 * where this class defines the abstract interface for an Ascii trace helper and it is
 * implemented in CCnxStackHelper, which derives from this class (among others).
 *
 * This class' job is to manage the public API to enable tracing on specific name prefixes and/or nodes.
 * It does not implement the actual methods to trace.  Those are done by the implementation class of
 * this base class.  For example, CCNxStackHelper derives from this class and implements the three
 * pure virtual protected methods.  Inside of those implementations, it sets each nodes attribute
 * for the tracing functions that it provides.
 */
class CCNxAsciiTraceHelper
{
public:
  /**
   * Construct an CCNxAsciiTraceHelper.
   */
  CCNxAsciiTraceHelper ();

  /**
   * Destroy an CCNxAsciiTraceHelper
   */
  virtual ~CCNxAsciiTraceHelper ();

  /**
   * Enable packet tracking on all nodes for all CCNx packets at the CCNxL3Protocol
   * @param [in] stream The stream to write to
   */
  void EnableAsciiCCNx (Ptr<OutputStreamWrapper> stream);

  /**
   * Enable packet tracing on all nodes for packets that match the given name prefix
   * If the prefix is NULL or ccnx:/, then all packets will be traced.
   *
   * @param [in] stream The stream to write to
   * @param [in] prefix The name prefix to trace or null (Ptr<CCNxName>(0)).
   */
  void EnableAsciiCCNx (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> prefix);

  /**
   * Enable packet tracing on the specified node for packets that match the given name prefix.
   * If the prefix is NULL or ccnx:/, then all packets will be traced.
   *
   * @param [in] stream The stream to write to
   * @param [in] prefix The name prefix to trace or null (Ptr<CCNxName>(0)).
   * @param [in] node The node to trace on
   */
  void EnableAsciiCCNx (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> prefix, Ptr<Node> node);

  /**
   * Enable packet tracing on the specified nodes for packets that match the given name prefix
   * @param [in] stream The stream to write to
   * @param [in] prefix The name prefix to trace or null (Ptr<CCNxName>(0)).
   * @param [in] nodes The set of nodes to trace on
   */
  void EnableAsciiCCNx (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> prefix, NodeContainer nodes);

protected:
  /**
   * Enable ascii trace output on the indicated name prefix to an output stream on the specific node.
   * The tracing implementation will filter packets based on the set of namePrefix specified (you may call
   * this function many times with different prefixes).
   *
   * If the prefix is ccnx:/ or null, then all packets will be traced.
   *
   * The implementation provides a Ptr<OutputStreamWrapper> where all output goes.
   *
   * @param stream
   * @param namePrefix
   * @param node
   */
  virtual void EnableAsciiCCNxInternal (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> namePrefix, Ptr<Node> node) = 0;

private:
  /**
   * Base class implementation of the public API method
   * @param [in] stream The stream to write to
   */
  void EnableAsciiCCNxImpl (Ptr<OutputStreamWrapper> stream);

  /**
   * Base class implementation of the public API method
   *
   * @param [in] stream The stream to write to
   * @param [in] prefix The name prefix to trace or null (Ptr<CCNxName>(0)).
   */
  void EnableAsciiCCNxImpl (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> prefix);

  /**
   * Base class implementation of the public API method
   *
   * @param [in] stream The stream to write to
   * @param [in] prefix The name prefix to trace or null (Ptr<CCNxName>(0)).
   * @param [in] node The node to trace on
   */
  void EnableAsciiCCNxImpl (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> prefix, Ptr<Node> node);

  /**
  * Base class implementation of the public API method
   *
   * @param [in] stream The stream to write to
   * @param [in] prefix The name prefix to trace or null (Ptr<CCNxName>(0)).
   * @param [in] nodes The set of nodes to trace on
   */
  void EnableAsciiCCNxImpl (Ptr<OutputStreamWrapper> stream, Ptr<CCNxName> prefix, NodeContainer nodes);
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_HELPER_CCNX_ASCII_TRACE_HELPER_H_ */
