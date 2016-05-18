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
#include "ccnx-codec-contentobject.h"
#include "ns3/ccnx-schema-v1.h"
#include "ns3/ccnx-tlv.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxCodecContentObject");

NS_OBJECT_ENSURE_REGISTERED (CCNxCodecContentObject);

TypeId
CCNxCodecContentObject::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ccnx::CCNxCodecContentObject")
    .SetParent<Header> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxCodecContentObject> ();
  return tid;
}

TypeId
CCNxCodecContentObject::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

// virtual from Header

uint32_t
CCNxCodecContentObject::GetSerializedSize (void) const
{
  uint32_t bytes = CCNxTlv::GetTLSize ();      // first header
  bytes += m_nameCodec.GetSerializedSize (); // name TLV

  if (m_content->GetPayloadType ())
    {
      bytes += CCNxTlv::GetTLSize () + 1;
    }

  if (m_content->GetExpiryTime ())
    {
      bytes += CCNxTlv::GetTLSize () + 8;
    }

  if (m_content->GetPayload ())
    {
      bytes += CCNxTlv::GetTLSize () + m_content->GetPayload ()->GetSize ();
    }

  return bytes;
}

void
CCNxCodecContentObject::Serialize (Buffer::Iterator outputIterator) const
{
  NS_LOG_FUNCTION (this << &outputIterator);

  uint16_t bytes = (uint16_t) GetSerializedSize ();
  NS_ASSERT_MSG (bytes >= CCNxTlv::GetTLSize (), "Serialized size must be at least 4 bytes");

  // -4 because it includes the T_OBJECT TLV.
  CCNxTlv::WriteTypeLength (outputIterator, CCNxSchemaV1::T_OBJECT, bytes - CCNxTlv::GetTLSize ());

  // The name codec includes the T_NAME TLV
  m_nameCodec.Serialize (outputIterator);

  // The call to Serialize is not by reference, so start is not updated.  Need to skip
  // over all the bytes we just wrote.
  outputIterator.Next (m_nameCodec.GetSerializedSize ());

  // Default payload type is CCNxContentObjectPayloadType_Data
  if (m_content->GetPayloadType () != CCNxContentObjectPayloadType_Data)
    {
      CCNxTlv::WriteTypeLength (outputIterator, CCNxSchemaV1::T_PAYLDTYPE, 1);
      outputIterator.WriteU8 (SerializePayloadTypeToSchemaValue (m_content->GetPayloadType ()));
    }

  if (m_content->GetExpiryTime ())
    {
      CCNxTlv::WriteTypeLength (outputIterator, CCNxSchemaV1::T_EXPIRY, 8);
      outputIterator.WriteHtonU64 (m_content->GetExpiryTime ()->getTime ());
    }

  if (m_content->GetPayload ())
    {
      Ptr<CCNxBuffer> payload = m_content->GetPayload ();
      CCNxTlv::WriteTypeLength (outputIterator, CCNxSchemaV1::T_PAYLOAD, (uint16_t) payload->GetSize ());
      outputIterator.Write (payload->Begin (), payload->End ());
    }
}

uint32_t
CCNxCodecContentObject::Deserialize (Buffer::Iterator inputIterator)
{
  NS_LOG_FUNCTION (this << &inputIterator);
  NS_ASSERT_MSG (inputIterator.GetSize () >= CCNxTlv::GetTLSize (), "Need to have at least 4 bytes to read");
  Buffer::Iterator iterator = inputIterator;

  uint32_t bytesRead = 0;

  uint16_t messageType = CCNxTlv::ReadType (iterator);
  if (messageType == CCNxSchemaV1::T_OBJECT)
    {
      uint16_t messageLength = CCNxTlv::ReadLength (iterator);
      NS_LOG_DEBUG ("Message type " << messageType << " length " << messageLength);

      // Do not count the initial 4 bytes read until the very end so we can compare
      // bytesRead against messageLength (which is the inner length)

      Ptr<const CCNxName> name = Ptr<const CCNxName> (0);
      CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
      Ptr<CCNxTime> expiryTime = Ptr<CCNxTime> (0);
      Ptr<CCNxBuffer> payload = Ptr<CCNxBuffer> (0);

      while (bytesRead < messageLength)
        {
          uint16_t nestedType = CCNxTlv::ReadType (iterator);
          uint16_t nestedLength = CCNxTlv::ReadLength (iterator);
          bytesRead += CCNxTlv::GetTLSize ();

          NS_LOG_DEBUG ("Nested  type " << nestedType << " length " << nestedLength << " bytesRead " << bytesRead);

          NS_ASSERT_MSG (bytesRead + nestedLength <= messageLength, "length goes beyond end of messageLength");

          switch (nestedType)
            {
            case CCNxSchemaV1::T_NAME:
              name = DeserializeName (iterator, nestedLength);
              break;

            case CCNxSchemaV1::T_PAYLDTYPE:
              payloadType = DeserializePayloadType (iterator, nestedLength);
              break;

            case CCNxSchemaV1::T_EXPIRY:
              expiryTime = DeserializeExpiryTime (iterator, nestedLength);
              break;

            case CCNxSchemaV1::T_PAYLOAD:
              payload = DeserializePayload (iterator, nestedLength);
              break;

            default:
              NS_ASSERT_MSG (true, "Unknown nested type " << nestedType);
            }

          bytesRead += nestedLength;
        }

      m_content = Create<CCNxContentObject> (name, payload, payloadType, expiryTime);
    }
  else
    {
      NS_ASSERT_MSG (false, "Message type is not T_OBJECT");
    }

  // add the initial TL length
  return bytesRead + CCNxTlv::GetTLSize ();
}

Ptr<const CCNxName>
CCNxCodecContentObject::DeserializeName (Buffer::Iterator &start, uint16_t length)
{
  NS_LOG_FUNCTION (this << &start);

  /*
   * The while loop in Deserialize has read the 4 bytes of T and L of the T_NAME
   * So we need to back up 4 bytes so CCNxCodecName can read the T_NAME and length.
   */

  start.Prev (CCNxTlv::GetTLSize ());
  m_nameCodec.Deserialize (start);
  NS_LOG_DEBUG ("Deserialized name " << m_nameCodec.GetHeader ());

  // now skip over the name
  start.Next (length + CCNxTlv::GetTLSize ());

  return m_nameCodec.GetHeader ();
}

CCNxContentObjectPayloadType
CCNxCodecContentObject::DeserializePayloadType (Buffer::Iterator &start, uint16_t length)
{
  NS_LOG_FUNCTION (this << &start);
  NS_ASSERT_MSG (length == 1, "Must be at least 1 byte in PayloadType");

  uint8_t value = start.ReadU8 ();

  NS_LOG_DEBUG ("Deserialized PayloadType " << value);

  return DeserializeSchemaValueToPayloadType (value);
}

Ptr<CCNxTime>
CCNxCodecContentObject::DeserializeExpiryTime (Buffer::Iterator &start, uint16_t length)
{
  NS_LOG_FUNCTION (this << &start);
  NS_ASSERT_MSG (length == 8, "Must be at least 8 bytes in Expiry Time");

  uint64_t value = start.ReadNtohU64 ();

  NS_LOG_DEBUG ("Deserialized Expiry Time " << value);
  return Create<CCNxTime> (value);
}

Ptr<CCNxBuffer>
CCNxCodecContentObject::DeserializePayload (Buffer::Iterator &start, uint16_t length)
{
  NS_LOG_FUNCTION (this << &start);

  Ptr<CCNxBuffer> payload = Create<CCNxBuffer> (0);
  payload->AddAtStart (length);
  start.Read (payload->Begin (), length);

  NS_LOG_DEBUG ("Deserialized Payload length " << length);
  return payload;
}

void
CCNxCodecContentObject::Print (std::ostream &os) const
{
  if (m_content)
    {
      os << *m_content;
    }
  else
    {
      os << "NULL header";
    }
}

CCNxCodecContentObject::CCNxCodecContentObject () : m_content (0)
{
  // empty
}

CCNxCodecContentObject::~CCNxCodecContentObject ()
{
  // empty
}

Ptr<CCNxContentObject>
CCNxCodecContentObject::GetHeader () const
{
  return m_content;
}

void
CCNxCodecContentObject::SetHeader (Ptr<CCNxContentObject> content)
{
  m_content = content;
  m_nameCodec.SetHeader (m_content->GetName ());
}

uint8_t
CCNxCodecContentObject::SerializePayloadTypeToSchemaValue (CCNxContentObjectPayloadType type)
{
  uint8_t schemaValue = 0;
  switch (type)
    {
    case CCNxContentObjectPayloadType_Data:
      schemaValue = CCNxSchemaV1::T_PAYLOADTYPE_DATA;
      break;
    case CCNxContentObjectPayloadType_Key:
      schemaValue = CCNxSchemaV1::T_PAYLOADTYPE_KEY;
      break;
    case CCNxContentObjectPayloadType_Link:
      schemaValue = CCNxSchemaV1::T_PAYLOADTYPE_LINK;
      break;
    case CCNxContentObjectPayloadType_Manifest:
      schemaValue = CCNxSchemaV1::T_PAYLOADTYPE_MANIFEST;
      break;
    default:
      NS_ASSERT_MSG (false, "Unsupported PayloadType");
      break;
    }
  return schemaValue;
}

CCNxContentObjectPayloadType
CCNxCodecContentObject::DeserializeSchemaValueToPayloadType (uint8_t type)
{
  CCNxContentObjectPayloadType payloadType = CCNxContentObjectPayloadType_Data;
  switch (type)
    {
    case CCNxSchemaV1::T_PAYLOADTYPE_DATA:
      payloadType = CCNxContentObjectPayloadType_Data;
      break;
    case CCNxSchemaV1::T_PAYLOADTYPE_KEY:
      payloadType = CCNxContentObjectPayloadType_Key;
      break;
    case CCNxSchemaV1::T_PAYLOADTYPE_LINK:
      payloadType = CCNxContentObjectPayloadType_Link;
      break;
    case CCNxSchemaV1::T_PAYLOADTYPE_MANIFEST:
      payloadType = CCNxContentObjectPayloadType_Manifest;
      break;
    default:
      NS_ASSERT_MSG (false, "Unsupported schema value");
      break;
    }
  return payloadType;
}



