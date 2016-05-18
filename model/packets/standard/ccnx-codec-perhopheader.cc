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
#include "ns3/ccnx-tlv.h"
#include "ccnx-codec-registry.h"
#include "ccnx-codec-perhopheader.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxCodecPerHopHeader");

NS_OBJECT_ENSURE_REGISTERED (CCNxCodecPerHopHeader);

TypeId
CCNxCodecPerHopHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxCodecPerHopHeader")
    .SetParent<Header> ()
    .SetGroupName ("CCNx");
  return tid;
}

TypeId
CCNxCodecPerHopHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CCNxCodecPerHopHeader::GetSerializedSize (void) const
{
  uint32_t length = 0;

  for (size_t i = 0; i < GetHeader()->size(); ++i)
  {
    Ptr<CCNxPerHopHeaderEntry> perhopEntry = GetHeader()->GetHeader(i);
    uint16_t type = perhopEntry->GetInstanceTLVType();
    Ptr<CCNxCodecPerHopHeaderEntry> codec = CCNxCodecRegistry::PerHopLookupCodec(type);
    NS_ASSERT_MSG ( (codec), "Could not find codec for type " << type);
    length += codec->GetSerializedSize(perhopEntry);
  }

  return length;
}

void
CCNxCodecPerHopHeader::Serialize (Buffer::Iterator outputIterator) const
{
  NS_LOG_FUNCTION (this << &outputIterator);

  for (size_t i = 0; i < GetHeader()->size(); ++i)
  {
      Ptr<CCNxPerHopHeaderEntry> perhopEntry = GetHeader()->GetHeader(i);
      uint16_t type = perhopEntry->GetInstanceTLVType();
      Ptr<CCNxCodecPerHopHeaderEntry> codec = CCNxCodecRegistry::PerHopLookupCodec(type);
      NS_ASSERT_MSG ( (codec), "Could not find codec for type " << type);
      codec->Serialize(perhopEntry, outputIterator);
  }
}

uint32_t
CCNxCodecPerHopHeader::Deserialize (Buffer::Iterator inputIterator)
{
  uint32_t totalLen = m_deserializeLength;

  Buffer::Iterator iterator = inputIterator;

  while (totalLen != 0)
  {
      NS_ASSERT_MSG(totalLen >= 4, "underrun - not enough bytes for a T and L");
      uint16_t type = CCNxTlv::ReadType (iterator);
      uint16_t length = CCNxTlv::ReadLength (iterator);
      NS_ASSERT_MSG(length < totalLen, "too long");

      // backup to start of TLV
      iterator.Prev(CCNxTlv::GetTLSize());

      Ptr<CCNxCodecPerHopHeaderEntry> codec = CCNxCodecRegistry::PerHopLookupCodec(type);
      NS_ASSERT_MSG ( (codec), "Could not find codec for type " << type);

      size_t bytesRead = 0;
      Ptr<CCNxPerHopHeaderEntry> perHopHeaderEntry = codec->Deserialize(iterator, &bytesRead);

      NS_ASSERT_MSG(bytesRead == length + CCNxTlv::GetTLSize(), "did not read right length");

      // Add per Hop header to the vector
      m_perHopHeader->AddHeader(perHopHeaderEntry);

      // Move the iterator pointer forward by bytesRead
      totalLen = totalLen - bytesRead;
  }
  return m_deserializeLength;
}

void
CCNxCodecPerHopHeader::Print (std::ostream &os) const
{
  for (size_t i = 0; i < GetHeader()->size(); ++i)
  {
      Ptr<CCNxPerHopHeaderEntry> perhopEntry = GetHeader()->GetHeader(i);
      uint16_t type = perhopEntry->GetInstanceTLVType();
      Ptr<CCNxCodecPerHopHeaderEntry> codec = CCNxCodecRegistry::PerHopLookupCodec(type);
      NS_ASSERT_MSG ( (codec), "Could not find codec for type " << type);
      codec->Print(perhopEntry, os);
  }
}

CCNxCodecPerHopHeader::CCNxCodecPerHopHeader ()
			: m_deserializeLength (0)
{
  m_perHopHeader = Create<CCNxPerHopHeader>();
}

CCNxCodecPerHopHeader::~CCNxCodecPerHopHeader ()
{
  // empty
}

Ptr<CCNxPerHopHeader>
CCNxCodecPerHopHeader::GetHeader () const
{
  return m_perHopHeader;
}

void
CCNxCodecPerHopHeader::SetDeserializeLength (uint32_t length)
{
  m_deserializeLength = length;
}
