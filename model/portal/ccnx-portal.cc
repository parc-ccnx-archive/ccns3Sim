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

#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/node.h"

#include "ns3/ccnx-portal.h"
#include "ns3/ccnx-portal-factory.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxPortal");

NS_OBJECT_ENSURE_REGISTERED (CCNxPortal);

TypeId
CCNxPortal::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxPortal")
    .SetParent<Object> ()
    .SetGroupName ("CCNx");
  return tid;
}

CCNxPortal::CCNxPortal (void)
{
  NS_LOG_FUNCTION_NOARGS ();
}

CCNxPortal::~CCNxPortal (void)
{
  NS_LOG_FUNCTION (this);
}

Ptr<CCNxPortal>
CCNxPortal::CreatePortal (Ptr<Node> node, TypeId tid)
{
  NS_ASSERT_MSG ((node), "Passed null node pointer");
  NS_LOG_FUNCTION (node << tid);

  Ptr<CCNxPortalFactory> portalFactory = node->GetObject<CCNxPortalFactory> (tid);
  NS_ASSERT_MSG ((portalFactory), "Could not find the portal factory for type " << tid);

  Ptr<CCNxPortal> portal = portalFactory->CreatePortal ();
  NS_ASSERT_MSG ((portal), "Failed to create portal");
  return portal;
}

void
CCNxPortal::SetDataSentCallback (Callback<void, Ptr<CCNxPortal>, Ptr<CCNxPacket> > dataSentCallback)
{
  NS_LOG_FUNCTION (this << &dataSentCallback);
  m_dataSent = dataSentCallback;
}

void
CCNxPortal::SetRecvCallback (Callback<void, Ptr<CCNxPortal> > receiveDataCallback)
{
  NS_LOG_FUNCTION (this << &receiveDataCallback);
  m_receivedData = receiveDataCallback;
}

void
CCNxPortal::NotifyDataSent (Ptr<CCNxPacket> packet)
{
  NS_LOG_FUNCTION (this << *packet);
  if (!m_dataSent.IsNull ())
    {
      m_dataSent (this, packet);
    }
}

void
CCNxPortal::NotifyRecv (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_receivedData.IsNull ())
    {
      m_receivedData (this);
    }
}


