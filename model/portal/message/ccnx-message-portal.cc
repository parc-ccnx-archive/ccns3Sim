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
 *
 */

#include "ns3/object.h"
#include "ns3/log.h"

#include "ns3/ccnx-message-portal.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxMessagePortal");

NS_OBJECT_ENSURE_REGISTERED (CCNxMessagePortal);

TypeId
CCNxMessagePortal::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxMessagePortal")
    .SetParent<CCNxPortal> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxMessagePortal> ();
  return tid;

}

CCNxMessagePortal::CCNxMessagePortal (void) : m_ccnx (NULL)
{
  NS_LOG_FUNCTION (this);
}

CCNxMessagePortal::~CCNxMessagePortal (void)
{
  NS_LOG_FUNCTION (this);
}

void
CCNxMessagePortal::SetCCNx (Ptr<CCNxL3Protocol> ccnx)
{
  NS_LOG_FUNCTION (this << ccnx);
  NS_ASSERT_MSG (m_ccnx == NULL, "Cannot set CCNx protocol more than once");
  m_ccnx = ccnx;
}

bool
CCNxMessagePortal::ReceiveFromLayer3 (Ptr<CCNxPacket> packet, Ptr<CCNxConnection> incoming)
{
  NS_LOG_FUNCTION (this << packet << incoming);
  NS_LOG_DEBUG ("packet " << packet << ", incoming " << incoming);

  m_inputQueue.push_back (new QueueEntryType (packet, incoming));
  NotifyRecv ();
  return true;
}

Ptr<CCNxL3Protocol>
CCNxMessagePortal::GetCCNx (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ccnx;
}

bool
CCNxMessagePortal::RegisterPrefix (Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << prefix);
  m_ccnx->RegisterPrefix (*this, prefix);
  return true;
}

void
CCNxMessagePortal::UnregisterPrefix (Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << prefix);
  m_ccnx->UnregisterPrefix (*this, prefix);
}

bool
CCNxMessagePortal::RegisterAnchor (Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << prefix);
  m_ccnx->RegisterAnchor (*this, prefix);
  return true;
}

void
CCNxMessagePortal::UnregisterAnchor (Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << prefix);
  m_ccnx->UnregisterAnchor (*this, prefix);
}


bool
CCNxMessagePortal::Close (void)
{
  NS_LOG_FUNCTION (this);

  m_ccnx->UnregisterProtocol (*this);
  m_ccnx = 0;

  return true;
}

bool
CCNxMessagePortal::Send (Ptr<CCNxPacket> packet)
{
  NS_LOG_FUNCTION (this << packet);

  Layer3SendCallback scb = GetLayer3SendCallback ();
  if (!scb.IsNull ())
    {
      NS_LOG_DEBUG ("packet " << packet);
      scb (*this, packet);
      NotifyDataSent (packet);
      return true;
    }

  NS_LOG_WARN ("No send callback defined, packet " << packet);
  return false;
}

bool
CCNxMessagePortal::SendTo (Ptr<CCNxPacket> packet, uint32_t egressConnectionId)
{
  NS_LOG_FUNCTION (this << packet);

  bool result = false;
  Layer3SendToCallback scb = GetLayer3SendToCallback ();
  if (!scb.IsNull ())
    {
      NS_LOG_DEBUG ("CCNxMessagePortal::SendTo(" << packet << ", " << egressConnectionId << ")");
      scb (*this, packet, egressConnectionId);
      NotifyDataSent (packet);
      result = true;
    }
  else
    {
      NS_LOG_ERROR ("CCNxMessagePortal::SendTo(" << packet << ", " << egressConnectionId << ") No send callback defined");
    }

  return result;
}

Ptr<CCNxPacket>
CCNxMessagePortal::Recv (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<CCNxPacket> packet;
  if (!m_inputQueue.empty ())
    {
      QueueEntryType *entry = m_inputQueue.front ();
      m_inputQueue.pop_front ();

      packet = entry->first;
      delete entry;
    }

  return packet;
}

Ptr<CCNxPacket>
CCNxMessagePortal::RecvFrom (Ptr<CCNxConnection> &incomingConnection)
{
  NS_LOG_FUNCTION (this);

  Ptr<CCNxPacket> packet;
  if (!m_inputQueue.empty ())
    {
      QueueEntryType *entry = m_inputQueue.front ();
      m_inputQueue.pop_front ();

      packet = entry->first;
      incomingConnection = entry->second;
      delete entry;
    }

  return packet;
}
