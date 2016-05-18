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


#ifndef CCNS3_CCNXCONNECTION_H
#define CCNS3_CCNXCONNECTION_H

#include "ns3/address.h"
#include "ns3/net-device.h"
#include "ns3/simple-ref-count.h"
#include "ns3/output-stream-wrapper.h"

#include "ns3/ccnx-packet.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-l3
 *
 * Represents a peering association with a neighbor.
 * Also represents the reverse route from which a packet came.
 *
 * Implementation classes:
 *  CCNxConnectionDevice - a NetDevice
 *  CCNxConnectionL4 - a CCNxL4Protocol
 */
class CCNxConnection : public SimpleRefCount<CCNxConnection>
{
public:
  /**
   * Typedef for Connection Id Type
   */
  typedef uint32_t ConnIdType;
  typedef std::list<ConnIdType> ConnIdsType;

  /**
   * A special value to always mean "localhost"
   */
  static const ConnIdType ConnIdLocalHost;

  /**
   * Constructor to create CCNxConnection.
   */
  CCNxConnection ();

  /**
   * Destructor for CCNxConnection.
   */
  virtual ~CCNxConnection ();

  /**
   * A (globally) unique ID for this connection.
   *
   * Can be used as an index in a hash table, etc.
   */
  ConnIdType GetConnectionId (void) const;

  /**
   * Set this connection's Id = local host.
   *
   * This will prevent a route from being added to the fib.
   */
  void SetConnectionLocalHost (void);

  /**
   * Send the specified CCNx packet out this connection.
   *
   * @param [in] packet The packet to send via this connection
   * @param [in] ingress The ingress connection of the packet
   *
   * Implemented by a concrete class (e.g. CCNxConnectionDevice or CCNxConnectionL4)
   */
  virtual bool Send (Ptr<CCNxPacket> packet, Ptr<CCNxConnection> ingress) = 0;

  /**
   * If the connection is closed, the remote side terminated it and
   * it can no longer be used.
   */
  bool isClosed (void) const;

  /**
   * Mark the connection as closed.
   */
  void Close (void);

protected:
  ConnIdType m_id; //protected so it can be set in derived CCNxVirtualConnection class (used for unit tests only)

private:
  static ConnIdType GetNextId ();
  static ConnIdType m_nextId;
  bool m_closed;
};

}
}


#endif //CCNS3_CCNXCONNECTION_H
