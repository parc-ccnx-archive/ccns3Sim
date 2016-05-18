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

#include <ns3/log.h>
#include "ccnx-interest.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxInterest");
NS_OBJECT_ENSURE_REGISTERED (CCNxInterest);

TypeId
CCNxInterest::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxInterest")
    .SetParent<CCNxMessage> ()
    .SetGroupName ("CCNx");
  return tid;
}

CCNxInterest::CCNxInterest (Ptr<const CCNxName> name, Ptr<CCNxBuffer> payload, Ptr<CCNxHashValue> keyIdRest, Ptr<CCNxHashValue> hashRest)
  : CCNxMessage (name, payload), m_keyidRestriction (keyIdRest), m_hashRestriction (hashRest)
{
  // empty
}

CCNxInterest::CCNxInterest (Ptr<const CCNxName> name, Ptr<CCNxBuffer> payload)
  : CCNxMessage (name, payload), m_keyidRestriction (0), m_hashRestriction (0)
{
  // empty
}

CCNxInterest::CCNxInterest (Ptr<const CCNxName> name)
  : CCNxMessage (name), m_keyidRestriction (0), m_hashRestriction (0)
{
  // empty
}

Ptr<CCNxHashValue>
CCNxInterest::GetKeyidRestriction () const
{
  return m_keyidRestriction;
}

Ptr<CCNxHashValue>
CCNxInterest::GetHashRestriction () const
{
  return m_hashRestriction;
}

bool
CCNxInterest::HasKeyidRestriction () const
{
  if (m_keyidRestriction == 0)
    {
      return false;
    }
  else
    {
      return true;
    }
}

bool
CCNxInterest::HasHashRestriction () const
{
  if (m_hashRestriction == 0)
    {
      return false;
    }
  else
    {
      return true;
    }
}

bool
CCNxInterest::Equals (const Ptr<CCNxInterest> other) const
{
  if (other)
    {
      return Equals (*other);
    }
  else
    {
      return false;
    }
}

bool
CCNxInterest::Equals (CCNxInterest const &other) const
{
  bool result = false;
  if (m_payload->Equals (*other.m_payload)
      && *m_keyidRestriction == *other.m_keyidRestriction
      && *m_hashRestriction == *other.m_hashRestriction
      && m_name->Equals (*other.m_name) )
    {
      result = true;
    }
  return result;
}

enum CCNxMessage::MessageType
CCNxInterest::GetMessageType (void) const
{
  return CCNxMessage::Interest;
}

std::ostream &
ns3::ccnx::operator<< (std::ostream &os, CCNxInterest const &interest)
{
  os << "{ Interest " << *interest.GetName ();
  if (interest.GetPayload ())
    {
      os << ", PayloadSize " << interest.GetPayload ()->GetSize ();
    }
  else
    {
      os << ", PayloadSize (none)";
    }
  if (interest.GetKeyidRestriction ())
    {
      os << ", KeyId " << interest.GetKeyidRestriction ()->GetValue ();
    }
  else
    {
      os << ", KeyId (none)";
    }
  if (interest.GetHashRestriction ())
    {
      os << ", Hash "  << interest.GetHashRestriction ()->GetValue () << " }";
    }
  else
    {
      os << ", Hash (none) }";
    }
  return os;
}
