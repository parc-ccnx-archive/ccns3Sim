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

#ifndef CCNS3_CCNXL3INTERFACE_H
#define CCNS3_CCNXL3INTERFACE_H

#include "ns3/ptr.h"
#include "ns3/net-device.h"

#include "ns3/simple-ref-count.h"

#include "ns3/ccnx-connection.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-l3
 *
 * Represents a device interface at the L3 level.  Has controls to turn the interface
 * up/down or forwarding/non-forwarding at the L3 level without affecting the actual device.
 */
class CCNxL3Interface : public SimpleRefCount<CCNxL3Interface>
{
public:
  /**
   * Constructor for CCNxL3Interface
   */
  CCNxL3Interface (Ptr<NetDevice> device);

  /**
   * Returns the corresponding call to ns3::NetDevice::GetMtu()
   * @return The maximum transmission unit on the NetDevice
   */
  uint16_t GetMtu (void) const;

  /**
   * Returns the corresponding call to ns3::NetDevice::GetIfIndex().
   * @return The interface index
   */
  uint32_t GetIfIndex (void) const;

  /**
   * @return true if the underlying interface is in the "up" state,
   *          false otherwise.  This is a L3 status.
   */
  bool IsUp (void) const;

  /**
   * Set the interface into the "up" state. In this state, it is
   * considered valid during CCNx forwarding.  This is a L3 status.
   *
   * When an Interface is set up or down, a notification is sent to the CCNxRoutingProtocol.
   * Therefore, you need to be sure the CCNxL3Interface has been added to CCNxL3Protocol before
   * setting Up or Down.
   */
  void SetUp (void);

  /**
   * Set the interface into the "down" state. In this state, it is
   * ignored during CCNx forwarding.  This is a L3 status.
   *
   * When an Interface is set up or down, a notification is sent to the CCNxRoutingProtocol.
   * Therefore, you need to be sure the CCNxL3Interface has been added to CCNxL3Protocol before
   * setting Up or Down.
   */
  void SetDown (void);

  /**
   * @return true if CCNx forwarding enabled at L3 level, else returns false
   */
  bool IsForwarding (void) const;

  /**
   * @param [in] val Value to set the forwarding flag
   * If set to true, CCNx forwarding is enabled for input datagrams on this device
   */
  void SetForwarding (bool val);

  /**
   * If the interface supports broadcasting, then this connection will send to
   * the broadcast address on the interface.
   * @return Pointer to CCNxConnection Object
   */
  Ptr<CCNxConnection> GetBroadcastConnection (void) const;

  /**
   * Configure a broadcast connection for the interface.
   * @param [in] Pointer to CCNxConnection Object
   */
  void SetBroadcastConnection (Ptr<CCNxConnection> conn);

private:
  bool m_up;
  bool m_forwarding;

  Ptr<NetDevice> m_device;
  Ptr<CCNxConnection> m_broadcast;
};
}
}

#endif //CCNS3_CCNXL3INTERFACE_H
