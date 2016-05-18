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
#include "ns3/ccnx-codec-name.h"
#include "ns3/ccnx-schema-v1.h"
#include "ns3/ccnx-tlv.h"
#include "ns3/ccnx-name-builder.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxCodecName");

NS_OBJECT_ENSURE_REGISTERED (CCNxCodecName);

static const uint16_t typeLengthBytes = 4;


TypeId
CCNxCodecName::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxCodecName")
    .SetParent<Header> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxCodecName> ();
  return tid;
}

TypeId
CCNxCodecName::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

// virtual from Header

uint32_t
CCNxCodecName::GetSerializedSize (void) const
{
  // TODO: Cache this result
  uint32_t bytes = typeLengthBytes;   // opening T_NAME
  unsigned count = m_name->GetSegmentCount ();
  bytes += typeLengthBytes * count;

  for (unsigned i = 0; i < count; i++)
    {
      Ptr<const CCNxNameSegment> segment = m_name->GetSegment (i);
      bytes += segment->GetValue ().size ();
    }

  return bytes;
}

void
CCNxCodecName::Serialize (Buffer::Iterator start) const
{
  uint32_t bytes = GetSerializedSize ();
  NS_ASSERT_MSG (bytes >= typeLengthBytes, "Serialized length must be at least 4 bytes");

  CCNxTlv::WriteTypeLength (start, CCNxSchemaV1::T_NAME, bytes - typeLengthBytes);

  unsigned count = m_name->GetSegmentCount ();
  for (unsigned i = 0; i < count; i++)
    {
      Ptr<const CCNxNameSegment> segment = m_name->GetSegment (i);

      size_t len = segment->GetValue ().size ();
      NS_ASSERT_MSG (len <= 0xFFFF, "Name segment length too large");
      CCNxTlv::WriteTypeLength (start, NameSegmentTypeToSchemaValue (segment->GetType ()), (uint16_t) len);
      start.Write ((const uint8_t *) segment->GetValue ().c_str (), (uint16_t) len);
    }
}

uint32_t
CCNxCodecName::Deserialize (Buffer::Iterator start)
{
  NS_ASSERT_MSG (start.GetSize () >= CCNxTlv::GetTLSize (), "Must have at least 4 bytes in buffer");

  // don't count the outer T_NAME bytes until after the while() loop so we
  // can compare directly against outerLength
  uint32_t bytesRead = 0;
  uint16_t outerType = CCNxTlv::ReadType (start);
  uint16_t outerLength = CCNxTlv::ReadLength (start);

  NS_ASSERT_MSG (outerType == CCNxSchemaV1::T_NAME, "Outer type must be T_NAME");

  m_name = 0;

  CCNxNameBuilder builder;

  while (bytesRead < outerLength)
    {
      uint16_t innerType = CCNxTlv::ReadType (start);
      uint16_t innerLength = CCNxTlv::ReadLength (start);
      bytesRead += typeLengthBytes;

      NS_ASSERT_MSG (innerLength + bytesRead <= outerLength, "innerLength goes beyond end of Name");
      NS_ASSERT_MSG (innerLength > 0, "Empty segments not supported");

      // This is very inefficient.  Need to figure out a better way of managing this memory.
      uint8_t temp[innerLength];
      start.Read (temp, innerLength);
      std::string s ((char *) temp, innerLength);
      Ptr<const CCNxNameSegment> segment = Create<const CCNxNameSegment> (SchemaValueToNameSegmentType (innerType), s);
      builder.Append (segment);

      bytesRead += innerLength;
    }

  NS_ASSERT_MSG (bytesRead == outerLength, "Did not consume all bytes");

  m_name = builder.CreateName ();

  // Add in the initial 4 bytes from the T_NAME bytes
  return bytesRead + CCNxTlv::GetTLSize ();
}

void
CCNxCodecName::Print (std::ostream &os) const
{
  if (m_name)
    {
      os << "{ " << *m_name << " }";
    }
  else
    {
      os << "{ null name }";
    }
}

CCNxCodecName::CCNxCodecName () : m_name (0)
{
  // empty
}

CCNxCodecName::~CCNxCodecName ()
{
  // empty (use DoDispose)
}

Ptr<const CCNxName> CCNxCodecName::GetHeader () const
{
  return m_name;
}

void CCNxCodecName::SetHeader (Ptr<const CCNxName> name)
{
  m_name = name;
}

uint16_t
CCNxCodecName::NameSegmentTypeToSchemaValue (CCNxNameSegmentType type)
{
  uint16_t schemaValue = 0;
  switch (type)
    {
    case CCNxNameSegment_Name:
      schemaValue = CCNxSchemaV1::T_NAMESEG_NAME;
      break;
    case CCNxNameSegment_Chunk:
      schemaValue = CCNxSchemaV1::T_NAMESEG_CHUNK;
      break;
    case CCNxNameSegment_Version:
      schemaValue = CCNxSchemaV1::T_NAMESEG_VERSION;
      break;
    case CCNxNameSegment_App0:
      schemaValue = CCNxSchemaV1::T_NAMESEG_APP0;
      break;
    case CCNxNameSegment_App1:
      schemaValue = CCNxSchemaV1::T_NAMESEG_APP1;
      break;
    case CCNxNameSegment_App2:
      schemaValue = CCNxSchemaV1::T_NAMESEG_APP2;
      break;
    case CCNxNameSegment_App3:
      schemaValue = CCNxSchemaV1::T_NAMESEG_APP3;
      break;
    case CCNxNameSegment_App4:
      schemaValue = CCNxSchemaV1::T_NAMESEG_APP4;
      break;
    default:
      NS_ASSERT_MSG (false, "Unsupported CCNxNameSegmentType");
      break;
    }
  return schemaValue;
}

CCNxNameSegmentType
CCNxCodecName::SchemaValueToNameSegmentType (uint16_t type)
{
  CCNxNameSegmentType nameSegmentType = CCNxNameSegment_Name;
  switch (type)
    {
    case CCNxSchemaV1::T_NAMESEG_NAME:
      nameSegmentType = CCNxNameSegment_Name;
      break;
    case CCNxSchemaV1::T_NAMESEG_CHUNK:
      nameSegmentType = CCNxNameSegment_Chunk;
      break;
    case CCNxSchemaV1::T_NAMESEG_VERSION:
      nameSegmentType = CCNxNameSegment_Version;
      break;
    case CCNxSchemaV1::T_NAMESEG_APP0:
      nameSegmentType = CCNxNameSegment_App0;
      break;
    case CCNxSchemaV1::T_NAMESEG_APP1:
      nameSegmentType = CCNxNameSegment_App1;
      break;
    case CCNxSchemaV1::T_NAMESEG_APP2:
      nameSegmentType = CCNxNameSegment_App2;
      break;
    case CCNxSchemaV1::T_NAMESEG_APP3:
      nameSegmentType = CCNxNameSegment_App3;
      break;
    case CCNxSchemaV1::T_NAMESEG_APP4:
      nameSegmentType = CCNxNameSegment_App4;
      break;
    default:
      NS_ASSERT_MSG (false, "Unsupported schema value");
      break;
    }
  return nameSegmentType;
}

bool
CCNxCodecName::Equals (CCNxCodecName const & other) const
{
  bool result = false;
  if (m_name && other.m_name)
    {
      if (m_name->Equals (*other.m_name))
        {
          result = true;
        }
    }
  return result;
}
