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
#include "ccnx-codec-fixedheader.h"
#include "ns3/ccnx-tlv.h"
#include "ccnx-codec-registry.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxCodecFixedHeader");

NS_OBJECT_ENSURE_REGISTERED (CCNxCodecFixedHeader);

TypeId
CCNxCodecFixedHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxCodecFixedHeader")
    .SetParent<Header> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxCodecFixedHeader> ();
  return tid;
}

TypeId
CCNxCodecFixedHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

// virtual from Header

uint32_t
CCNxCodecFixedHeader::GetSerializedSize (void) const
{
  uint32_t length = 8; // Length of fixed size header

  // Check if there are any per hop headers and add length (if any)
  for (size_t i = 0; i < GetPerHopHeader()->size(); ++i)
  {
    Ptr<CCNxPerHopHeaderEntry> perhopEntry = GetPerHopHeader()->GetHeader(i);
    uint16_t type = perhopEntry->GetInstanceTLVType();
    Ptr<CCNxCodecPerHopHeaderEntry> codec = CCNxCodecRegistry::PerHopLookupCodec(type);
    NS_ASSERT_MSG ( (codec), "Could not find codec for type " << type);
    length += codec->GetSerializedSize(perhopEntry);
  }

  return length;
}

void
CCNxCodecFixedHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;

  i.WriteU8 (m_fixedheader->GetVersion ());
  i.WriteU8 (PacketTypeValueFromEnum (m_fixedheader->GetPacketType ()));
  i.WriteHtonU16 (m_fixedheader->GetPacketLength ());
  i.WriteU8 (m_fixedheader->GetHopLimit ());
  i.WriteU8 (m_fixedheader->GetReturnCode ());
  i.WriteU8 (0); // reserved byte
  i.WriteU8 (GetSerializedSize());

  for (size_t k = 0; k < GetPerHopHeader()->size(); ++k)
  {
      Ptr<CCNxPerHopHeaderEntry> perhopEntry = GetPerHopHeader()->GetHeader(k);
      uint16_t type = perhopEntry->GetInstanceTLVType();
      Ptr<CCNxCodecPerHopHeaderEntry> codec = CCNxCodecRegistry::PerHopLookupCodec(type);
      NS_ASSERT_MSG ( (codec), "Could not find codec for type " << type);
      codec->Serialize(perhopEntry, &i);
  }
}

uint32_t
CCNxCodecFixedHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  NS_ASSERT_MSG (start.GetSize () >= 8, "Need to have at least 8 bytes to read");
  NS_LOG_DEBUG ("Deserialize buffer: distance " << start.GetDistanceFrom (start) << " size " << start.GetSize ());
  Buffer::Iterator i = start;

  uint8_t ver = i.ReadU8 ();
  uint8_t packetType = i.ReadU8 ();
  uint16_t packetLength = i.ReadNtohU16 ();
  uint8_t hopLimit = i.ReadU8 ();
  uint8_t returnCode = i.ReadU8 ();
  i.ReadU8 (); // reserved byte
  uint8_t headerLength = i.ReadU8 ();

  CCNxFixedHeaderType pt = PacketTypeEnumFromValue (packetType);
  m_fixedheader = Create<CCNxFixedHeader> (ver, pt, packetLength, hopLimit, returnCode, headerLength);

  NS_LOG_DEBUG ("Fixed Header: " << *m_fixedheader);

  uint32_t perhopLen = headerLength - 8;

  while (perhopLen != 0)
  {
      NS_ASSERT_MSG(perhopLen >= 4, "underrun - not enough bytes for a T and L");
      uint16_t type = CCNxTlv::ReadType (i);
      uint16_t length = CCNxTlv::ReadLength (i);
      NS_ASSERT_MSG(length < perhopLen, "too long");

      // backup to start of TLV
      i.Prev(CCNxTlv::GetTLSize());

      Ptr<CCNxCodecPerHopHeaderEntry> codec = CCNxCodecRegistry::PerHopLookupCodec(type);
      NS_ASSERT_MSG ( (codec), "Could not find codec for type " << type);

      size_t bytesRead = 0;
      Ptr<CCNxPerHopHeaderEntry> perHopHeaderEntry = codec->Deserialize(&i, &bytesRead);

      NS_ASSERT_MSG(bytesRead == length + CCNxTlv::GetTLSize(), "did not read right length");

      // Add per Hop header to the vector
      m_perHopHeader->AddHeader(perHopHeaderEntry);

      // Move the iterator pointer forward by bytesRead
      perhopLen = perhopLen - bytesRead;
  }

    return GetSerializedSize();
}

void
CCNxCodecFixedHeader::Print (std::ostream &os) const
{
  if (m_fixedheader)
    {
      os << *m_fixedheader;
    }
  else
    {
      os << "NULL Fixed header";
    }

  for (size_t i = 0; i < GetPerHopHeader()->size(); ++i)
  {
      Ptr<CCNxPerHopHeaderEntry> perhopEntry = GetPerHopHeader()->GetHeader(i);
      uint16_t type = perhopEntry->GetInstanceTLVType();
      Ptr<CCNxCodecPerHopHeaderEntry> codec = CCNxCodecRegistry::PerHopLookupCodec(type);
      NS_ASSERT_MSG ( (codec), "Could not find codec for type " << type);
      codec->Print(perhopEntry, os);
  }
}

CCNxCodecFixedHeader::CCNxCodecFixedHeader () : m_fixedheader (0)
{
  m_perHopHeader = Create<CCNxPerHopHeader>();
}

CCNxCodecFixedHeader::~CCNxCodecFixedHeader ()
{
  // empty
}

Ptr<CCNxFixedHeader>
CCNxCodecFixedHeader::GetFixedHeader () const
{
  return m_fixedheader;
}

void
CCNxCodecFixedHeader::SetFixedHeader (Ptr<CCNxFixedHeader> header)
{
  m_fixedheader = header;
}

Ptr<CCNxPerHopHeader>
CCNxCodecFixedHeader::GetPerHopHeader () const
{
  return m_perHopHeader;
}

CCNxFixedHeaderType
CCNxCodecFixedHeader::PacketTypeEnumFromValue (uint8_t value)
{
  switch (value)
    {
    case 1:
      return CCNxFixedHeaderType_Interest;
    case 2:
      return CCNxFixedHeaderType_Object;
    case 3:
      return CCNxFixedHeaderType_InterestReturn;
    default:
      NS_ASSERT_MSG (false, "Unsupported value: " << (uint32_t) value);
    }
  return CCNxFixedHeaderType_Interest;
}

uint8_t
CCNxCodecFixedHeader::PacketTypeValueFromEnum (CCNxFixedHeaderType type)
{
  switch (type)
    {
    case CCNxFixedHeaderType_Interest:
      return 1;
    case CCNxFixedHeaderType_Object:
      return 2;
    case CCNxFixedHeaderType_InterestReturn:
      return 3;
    default:
      NS_ASSERT_MSG (false, "Unsupported value");
    }
  return 0;
}
