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
#include "ccnx-fixedheader.h"

using namespace ns3;
using namespace ns3::ccnx;

CCNxFixedHeader::CCNxFixedHeader (uint8_t ver, CCNxFixedHeaderType packetType, uint16_t packetLength,
                                  uint8_t hopLimit, uint8_t returnCode, uint8_t headerLength) :
  m_version (ver), m_packetType (packetType), m_packetLength (packetLength),
  m_hopLimit (hopLimit), m_returnCode (returnCode), m_headerLength (headerLength)
{
}

uint8_t
CCNxFixedHeader::GetVersion () const
{
  return m_version;
}

CCNxFixedHeaderType
CCNxFixedHeader::GetPacketType () const
{
  return m_packetType;
}

uint16_t
CCNxFixedHeader::GetPacketLength () const
{
  return m_packetLength;
}

uint8_t
CCNxFixedHeader::GetHeaderLength () const
{
  return m_headerLength;
}

uint8_t
CCNxFixedHeader::GetHopLimit () const
{
  return m_hopLimit;
}

uint8_t
CCNxFixedHeader::GetReturnCode () const
{
  return m_returnCode;
}

void
CCNxFixedHeader::SetPacketLength (uint16_t length)
{
  m_packetLength = length;
}

void
CCNxFixedHeader::SetHeaderLength (uint16_t length)
{
  m_headerLength = length;
}

void
CCNxFixedHeader::SetHopLimit (uint8_t limit)
{
  m_hopLimit = limit;
}

bool
CCNxFixedHeader::Equals (const Ptr<CCNxFixedHeader> other) const
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
CCNxFixedHeader::Equals (CCNxFixedHeader const &other) const
{
  bool result = false;
  if (m_version == other.m_version
      && m_packetType == other.m_packetType
      && m_packetLength == other.m_packetLength
      && m_hopLimit == other.m_hopLimit
      && m_returnCode == other.m_returnCode
      && m_headerLength == other.m_headerLength )
    {
      result = true;
    }
  return result;
}

std::ostream &
ns3::ccnx::operator << (std::ostream& os, CCNxFixedHeader const & header)
{
  os << "{ Version "   << (uint32_t) header.GetVersion () << ", PacketType " << header.GetPacketType ();
  os << ", PacketLength " << header.GetPacketLength ();
  os << ", HopLimit " << (uint32_t) header.GetHopLimit () << ", ReturnCode " << (uint32_t) header.GetReturnCode ();
  os << ", HeaderLength " << (uint32_t) header.GetHeaderLength () << " }";
  return os;
}


