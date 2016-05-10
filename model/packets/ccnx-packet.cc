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
 * # is up to the contributors to maintain their section in this file up to date
 * # and up to the user of the software to verify any claims herein.
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

#include "ns3/tag.h"
#include "ns3/ccnx-packet.h"
#include "ns3/log.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxPacket");

//static inline void
//hexdump (const char *label, size_t size, const uint8_t *data)
//{
//    printf ("hexdump %s:\n", label);
//    for (size_t i = 0; i < size; i++)
//        {
//        printf ("%02x ", data[i]);
//        if ((i + 1) % 16 == 0)
//            {
//            printf ("\n");
//            }
//        }
//    if (size % 16 != 0)
//        {
//        printf ("\n");
//        }
//}

Ptr<CCNxPacket>
CCNxPacket::CreateFromMessage (Ptr<CCNxMessage> message)
{
  // We cannot use Create<CCNxPacket>() because the call to the protected method needs to be here.
  Ptr<CCNxPacket> packet = Ptr<CCNxPacket> (new CCNxPacket (), false);
  packet->m_message = message;
  packet->m_codecFixedHeader.SetHeader (packet->GenerateFixedHeader (message->GetMessageType ()));
  packet->m_ns3Packet = 0;
  return packet;
}

Ptr<CCNxPacket>
CCNxPacket::CreateFromMessage (Ptr<CCNxMessage> message, Ptr<CCNxValidation> validation)
{
  // We cannot use Create<CCNxPacket>() because the call to the protected method needs to be here.
  Ptr<CCNxPacket> packet = Ptr<CCNxPacket> (new CCNxPacket (), false);
  packet->m_message = message;
  packet->m_validation = validation;
  packet->m_codecFixedHeader.SetHeader (packet->GenerateFixedHeader (message->GetMessageType ()));
  packet->m_ns3Packet = 0;
  return packet;
}

Ptr<CCNxPacket>
CCNxPacket::CreateFromNs3Packet (Ptr<const Packet> ns3Packet)
{
  // We cannot use Create<CCNxPacket>() because the call to the protected method needs to be here.
  Ptr<CCNxPacket> packet = Ptr<CCNxPacket> (new CCNxPacket (), false);
  packet->m_ns3Packet = ns3Packet->Copy ();
  packet->Deserialize ();
  return packet;
}

CCNxPacket::CCNxPacket () : m_hash (0)
{
  // emtpy protected method
}

CCNxPacket::~CCNxPacket ()
{
  // empty use DoDispose()
}

void CCNxPacket::DoDispose (void)
{
  m_ns3Packet = 0;
}

Ptr<Packet>
CCNxPacket::CreateNs3Packet ()
{
  if (m_ns3Packet == NULL)
    {
      m_ns3Packet = GenerateNs3Packet ();
    }

  return m_ns3Packet->Copy ();
}

Ptr<CCNxFixedHeader>
CCNxPacket::GetFixedHeader () const
{
  return m_codecFixedHeader.GetHeader ();
}

Ptr<CCNxMessage>
CCNxPacket::GetMessage () const
{
  return m_message;
}

Ptr<CCNxValidation>
CCNxPacket::GetValidation () const
{
  return m_validation;
}

CCNxPerHopHeader
CCNxPacket::GetPerhopHeaders() const
{
	return m_codecPerHopHeader.GetHeader();
}

size_t
CCNxPacket::ComputePacketSize () const
{
  size_t length = 0;

  length += m_codecFixedHeader.GetSerializedSize ();

  length += m_codecPerHopHeader.GetSerializedSize();

  CCNxMessage::MessageType messageType = m_message->GetMessageType ();
  if ( messageType == CCNxMessage::Interest)
    {
      Ptr<CCNxInterest> interest = DynamicCast<CCNxInterest, CCNxMessage> (m_message);
      CCNxCodecInterest codec;
      codec.SetHeader (interest);
      length += codec.GetSerializedSize ();

    }
  else if (messageType == CCNxMessage::ContentObject)
    {
      Ptr<CCNxContentObject> content = DynamicCast<CCNxContentObject, CCNxMessage> (m_message);
      CCNxCodecContentObject codec;
      codec.SetHeader (content);
      length += codec.GetSerializedSize ();
    }
  else
    {
      NS_ASSERT_MSG (false, "Unsupported m_message run time type " << messageType);
    }
  return length;
}

/*
 * This will create a virtual packet that does not actually allocate any bytes for payload.
 * We tell it how big it is and attach the CCNxPacket as a tag.
 */
Ptr<Packet>
CCNxPacket::GenerateNs3Packet ()
{
  // Set the total per hop header length
  m_codecPerHopHeader.SetHeaderLength(m_codecPerHopHeader.GetSerializedSize());

  Ptr<CCNxFixedHeader> fh = GenerateFixedHeader (m_message->GetMessageType ());
  m_codecFixedHeader.SetHeader (fh);

  Ptr<Packet> p = Create<Packet> ();

  CCNxMessage::MessageType messageType = m_message->GetMessageType ();
  if ( messageType == CCNxMessage::Interest)
    {
      Ptr<CCNxInterest> interest = DynamicCast<CCNxInterest, CCNxMessage> (m_message);
      m_codecInterest.SetHeader (interest);
      p->AddHeader (m_codecInterest);

    }
  else if (messageType == CCNxMessage::ContentObject)
    {
      Ptr<CCNxContentObject> content = DynamicCast<CCNxContentObject, CCNxMessage> (m_message);
      m_codecContentObject.SetHeader (content);
      p->AddHeader (m_codecContentObject);
    }
  else
    {
      NS_ASSERT_MSG (false, "Unsupported m_message run time type " << messageType);
    }

  // Is this the correct place for per hop headers???
  p->AddHeader (m_codecPerHopHeader);

  // The fixed header goes outside the message header
  p->AddHeader (m_codecFixedHeader);

  size_t expectedSize = ComputePacketSize ();
  size_t actualSize = p->GetSize ();
  NS_ASSERT_MSG (expectedSize == actualSize, "Packet size " << actualSize << " does not match expected size " << expectedSize);
  NS_ASSERT_MSG (expectedSize == fh->GetPacketLength (), "Fixed Header size " << fh->GetPacketLength () <<
                 " does not match expected size " << expectedSize);

  // TODO: Add PacketTag of the Hash

  return p;
}

Ptr<CCNxFixedHeader>
CCNxPacket::GenerateFixedHeader (CCNxMessage::MessageType messageType) const
{
  NS_LOG_FUNCTION_NOARGS ();
  size_t packetLength = ComputePacketSize ();
  CCNxFixedHeaderType type = GetMessageTypeAsPacketType (messageType);
  uint32_t perHopLength = GetPerHopHeaderLength ();
  uint32_t headerLength = m_codecFixedHeader.GetSerializedSize () + perHopLength;
  uint8_t hopLimit = GetHopLimit ();
  uint8_t returnCode = GetReturnCode ();

  NS_ASSERT_MSG (headerLength <= 0xFF, "headerLength too long");
  NS_ASSERT_MSG (packetLength <= 0xFFFF, "packetLength too long");

  Ptr<CCNxFixedHeader> fh = Create<CCNxFixedHeader> (1, type, packetLength, hopLimit, returnCode, headerLength);
  NS_LOG_DEBUG ("Create fixed header " << *fh);
  return fh;
}

CCNxFixedHeaderType
CCNxPacket::GetMessageTypeAsPacketType (CCNxMessage::MessageType messageType = CCNxMessage::Interest) const
{
  CCNxFixedHeaderType packetType = CCNxFixedHeaderType_Interest;
  if (messageType == CCNxMessage::ContentObject)
    {
      packetType = CCNxFixedHeaderType_Object;
    }
  return packetType;
}

uint32_t
CCNxPacket::GetPerHopHeaderLength (void) const
{
  return m_codecPerHopHeader.GetHeaderLength();
}

uint8_t
CCNxPacket::GetHopLimit (void) const
{
  // FIXME
  return 0;
}

uint8_t
CCNxPacket::GetReturnCode (void) const
{
  // FIXME
  return 0;
}

/**
 * Trim the ns3 packet down to the size specified in the CCNx packet
 * @param ccnxPacket
 * @param ns3Packet
 */
void
CCNxPacket::TrimNs3Packet ()
{
  if (GetFixedHeader ()->GetPacketLength () < m_ns3Packet->GetSize ())
    {
      size_t trimLength = m_ns3Packet->GetSize () - GetFixedHeader ()->GetPacketLength ();
      NS_LOG_INFO ("Trimming " << trimLength << " bytes from end of packet");
      m_ns3Packet->RemoveAtEnd (trimLength);
    }
}


void
CCNxPacket::Deserialize ()
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_LOG_DEBUG ("Deserialize " << *m_ns3Packet);

  /*
   * We want to work on a copy so we leave the headers in m_ns3Packet.
   * That way, we do not need to re-serialize.  The only thing that will change
   * here is we'll trim the packet if layer 2 added trailers.
   */
  Ptr<Packet> copy = m_ns3Packet->Copy ();
  uint32_t hdrSize = copy->RemoveHeader (m_codecFixedHeader);
  NS_LOG_DEBUG ("Deserialize: hdrSize = " << hdrSize);

  /*
   * Check for Per hop headers. If the length is more than the length of fixed header (=8),
   * then there is atleast one per hop header
   */
  if (hdrSize >= 8)
  {
	  m_codecPerHopHeader.SetHeaderLength(hdrSize-8);
      uint32_t msgSize = copy->RemoveHeader (m_codecPerHopHeader);
      NS_LOG_DEBUG ("Deserialize: per hop headers = " << msgSize);
  }

  switch (m_codecFixedHeader.GetHeader ()->GetPacketType ())
    {
    case CCNxFixedHeaderType_Interest:
      {
        uint32_t msgSize = copy->RemoveHeader (m_codecInterest);
        NS_LOG_DEBUG ("Deserialize: interest = " << msgSize);
        m_message = m_codecInterest.GetHeader ();
        break;
      }
    case CCNxFixedHeaderType_Object:
      {
        uint32_t msgSize = copy->RemoveHeader (m_codecContentObject);
        NS_LOG_DEBUG ("Deserialize: content = " << msgSize);
        m_message = m_codecContentObject.GetHeader ();
        break;
      }
    default:
      NS_ASSERT_MSG (false, "Unsupported Packet Type");
    }

  TrimNs3Packet ();
}

void
CCNxPacket::SetContentObjectHash (CCNxHashValue hash)
{
  m_hash = hash;
}

CCNxHashValue
CCNxPacket::GetContentObjectHash (void) const
{
  return m_hash;
}

void
CCNxPacket::AddPerHopHeaderEntry (Ptr<CCNxPerHopHeaderEntry> perHopHeaderEntry)
{
	m_codecPerHopHeader.GetHeader().AddHeader(perHopHeaderEntry);
}

std::ostream &
ns3::ccnx::operator<< (std::ostream &os, CCNxPacket const &packet)
{
  os << "header " << *packet.GetFixedHeader ()
     << ", message " << *packet.GetMessage ()
     << ", validation " << packet.GetValidation ();
  return os;
}
