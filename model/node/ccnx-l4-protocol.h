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

/*
 * Abstract base class
 */
#ifndef CCNS3_CCNXL4PROTOCOL_H
#define CCNS3_CCNXL4PROTOCOL_H

#include <stdint.h>

#include "ns3/ccnx-connection.h"
#include "ns3/ccnx-packet.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-l3
 *
 * This class is used as an interface between the CCNxL3Protocol and CCNxPortal instances.
 * The CCNxL4Protocol is the view of a L4 protocol from the L3 side.
 *
 * Each L4 protocol instance gets an instance ID that can be used to identify it to
 * CCNxL3Protocol and demux on that instance ID.
 */
class CCNxL4Protocol
{
public:
  /**
   * Typedef for CCNxL4Id
   */
  typedef uint32_t CCNxL4Id;

  /**
   * Constructor to create CCNxL4Protocol.
   */
  CCNxL4Protocol ();

  /**
   * Destructor for CCNxL4Protocol.
   */
  virtual ~CCNxL4Protocol ();

  /**
   * Each instance of CCNxL4Protocol gets a unique ID (CCNxL4Id).  This is
   * used by the forwarder to demux between different portals.
   */
  CCNxL4Id GetInstanceId (void) const;

  /**
   * Typedef for the Receive callback used when Layer 3 receives a packet from Layer 4
   */
  typedef Callback<bool, Ptr<CCNxPacket>, Ptr<CCNxConnection> > ReceiveCallback;

  /**
   * Called from Layer 3 when it receives a packet for this Layer 4 protocol.
   *
   * @param [in] packet The incoming packet
   * @param [in] incoming The incoming connection
   * @return true if accepted, false otherwise
   */
  virtual bool ReceiveFromLayer3 (Ptr<CCNxPacket> packet, Ptr<CCNxConnection> incoming) = 0;

  /**
   * Typedef for the Send callback used to send a packet to Layer3
   */
  typedef Callback<bool, const CCNxL4Protocol &, Ptr<CCNxPacket> > Layer3SendCallback;

  /**
   * Typedef for the SendTo callback used to send a packet to Layer3
   * while specifying an egress connection id.
   */
  typedef Callback<bool, const CCNxL4Protocol &, Ptr<CCNxPacket>, uint32_t > Layer3SendToCallback;

  /**
   * This method allows a caller to set the current down target callback
   * set for this L4 protocol.  This is typically set by the L3 protocol to
   * register itself as the down target.
   *
   * @param [in] cb current Callback for the L4 protocol to send a packet
   */
  virtual void SetLayer3SendCallback (Layer3SendCallback cb);

  /**
   * Used to set the Layer 3 function to use to send a packet down the stack
   * while specifying the layer 3 egress connection id to use.
   *
   * @param [in] cb current Callback for the L4 protocol to send a packet
   */
  virtual void SetLayer3SendToCallback (Layer3SendToCallback cb);

  /**
   * This method allows a caller to get the current down target callback
   * set for this L4 protocol
   *
   * @return current Callback for the L4 protocol to send a packet
   */
  virtual Layer3SendCallback GetLayer3SendCallback (void) const;

  /**
   * This method allows a caller to get the current down target callback
   * set for this L4 protocol
   *
   * @return current Callback for the L4 protocol to send a packet
   */
  virtual Layer3SendToCallback GetLayer3SendToCallback (void) const;


protected:
  static CCNxL4Id GetNextId (void);
  static CCNxL4Id m_nextId;
  CCNxL4Id m_id;
  Layer3SendCallback m_sendCallback;
  Layer3SendToCallback m_sendToCallback;
};
}
}

#endif //CCNS3_CCNXL4PROTOCOL_H
