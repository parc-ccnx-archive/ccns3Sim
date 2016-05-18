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

#include "nfp-payload.h"
#include "nfp-schema.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("NfpPayload");

NfpPayload::NfpPayload ()
{
  m_capacity = 0;
  m_remaining = 0;
  m_msgSeqnum = 0;
}

NfpPayload::NfpPayload (size_t capacity, Ptr<const CCNxName> routerName,
                        uint16_t msgSeqnum)
{
  NS_LOG_FUNCTION (this << capacity << routerName << msgSeqnum);

  m_routerNameCodec.SetHeader (routerName);
  m_capacity = capacity;
  m_msgSeqnum = msgSeqnum;

  size_t fixedOverhead = CCNxTlv::GetTLSize (); // T_MSG
  fixedOverhead += m_routerNameCodec.GetSerializedSize ();
  fixedOverhead += CCNxTlv::GetTLSize (); // T_MSG_SEQNUM
  fixedOverhead += sizeof(uint16_t);

  NS_ASSERT_MSG (fixedOverhead <= m_capacity,
                 "Fixed overhead of RouterName and msgSeqnum exceeds capacity");

  m_remaining = capacity - fixedOverhead;
}

NfpPayload::~NfpPayload ()
{
  // empty
}

Ptr<const CCNxName>
NfpPayload::GetRouterName (void) const
{
  return m_routerNameCodec.GetHeader ();
}

uint16_t
NfpPayload::GetMessageSeqnum (void) const
{
  return m_msgSeqnum;
}

void
NfpPayload::AppendMessage (Ptr<NfpMessage> message)
{
  NS_LOG_FUNCTION (this << message);

  size_t messageSize = message->GetSerializedSize ();
  NS_ASSERT_MSG (messageSize <= GetRemaining (),
                 "Message does not fit in payload.");

  m_remaining -= messageSize;
  m_messages.push_back (message);
}

/**
 * Returns the number of messages contained in this payload.
 *
 * @return Number of messages.
 */
size_t
NfpPayload::GetMessageCount (void) const
{
  return m_messages.size ();
}

Ptr<NfpMessage>
NfpPayload::GetMessage (size_t index) const
{
  NS_ASSERT_MSG (index < GetMessageCount (),
                 "Index beyond end of message array");
  return m_messages[index];
}

size_t
NfpPayload::GetRemaining (void) const
{
  return m_remaining;
}

uint32_t
NfpPayload::GetSerializedSize (void) const
{
  return (uint32_t) (m_capacity - m_remaining);
}

void
NfpPayload::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  uint32_t totalSize = GetSerializedSize ();
  NS_ASSERT_MSG (totalSize <= 0xFFFF, "Total size too large");

  uint16_t innerSize = (uint16_t) totalSize - CCNxTlv::GetTLSize ();

  CCNxTlv::WriteTypeLength (start, NfpSchema::T_MSG, innerSize);
  m_routerNameCodec.Serialize (start);
  start.Next (m_routerNameCodec.GetSerializedSize ());

  CCNxTlv::WriteTypeLength (start, NfpSchema::T_MSG_SEQNUM, 2);
  start.WriteHtonU16 (m_msgSeqnum);

  for (int i = 0; i < m_messages.size (); i++)
    {
      m_messages[i]->Serialize (start);
      start.Next (m_messages[i]->GetSerializedSize ());
    }
}

/**
 * Compute the amount of data remaining in `start` given that our cursor is at `current`.
 *
 * The calculation is start.GetSize() - current.GetDistanceFrom(start);
 *
 * PRECONDITION: `current` must not be before `start`.
 *
 * @param start The iterator's starting position.
 * @param current The iterator's current position.
 * @return The bytes remaining in the iterator.
 */
static uint32_t
ComputeRemaining (Buffer::Iterator const &start, Buffer::Iterator &current)
{
  return start.GetSize () - current.GetDistanceFrom (start);
}

uint32_t
NfpPayload::DeserializeRouterName (Buffer::Iterator const &start, Buffer::Iterator &current)
{
  uint32_t routerNameSize = m_routerNameCodec.Deserialize (current);
  current.Next (routerNameSize);

  NS_LOG_DEBUG ("RouterName " << *(m_routerNameCodec.GetHeader ())  << " offset " << current.GetDistanceFrom (start));
  return routerNameSize;
}

uint32_t
NfpPayload::DeserializeMessageSeqnum (Buffer::Iterator const &start, Buffer::Iterator &current)
{
  uint32_t remaining = ComputeRemaining (start, current);
  NS_ASSERT_MSG ( remaining >= CCNxTlv::GetTLSize (), "Must have at least 4 bytes in buffer");
  uint16_t innerType = CCNxTlv::ReadType (current);
  uint16_t innerLength = CCNxTlv::ReadLength (current);

  NS_ASSERT_MSG (innerType == NfpSchema::T_MSG_SEQNUM, "TLV type must be T_MSG_SEQNUM");
  NS_ASSERT_MSG (innerLength == sizeof(uint16_t), "TLV length must be " << sizeof(uint16_t));

  m_msgSeqnum = current.ReadNtohU16 ();

  NS_LOG_DEBUG ("MsgSeqnum " << m_msgSeqnum  << " offset " << current.GetDistanceFrom (start));
  return innerLength + CCNxTlv::GetTLSize ();
}

uint32_t
NfpPayload::DeserializeAdvertise (Buffer::Iterator const &start, Buffer::Iterator &current)
{
  Ptr<NfpAdvertise> advertise = Create<NfpAdvertise> ();
  uint32_t length = advertise->Deserialize (current);
  m_messages.push_back (advertise);
  current.Next (length);
  NS_LOG_DEBUG ("Advertise " << *advertise  << " offset " << current.GetDistanceFrom (start));

  return length;
}

uint32_t
NfpPayload::DeserializeWithdraw (Buffer::Iterator const &start, Buffer::Iterator &current)
{
  Ptr<NfpWithdraw> withdraw = Create<NfpWithdraw> ();
  uint32_t length = withdraw->Deserialize (current);
  m_messages.push_back (withdraw);
  current.Next (length);
  NS_LOG_DEBUG ("Withdraw " << *withdraw  << " offset " << current.GetDistanceFrom (start));

  return length;
}

uint32_t
NfpPayload::Deserialize (Buffer::Iterator start)
{
  // We need a copy so we can compute the distance
  Buffer::Iterator current = start;

  NS_LOG_FUNCTION (this << &current);
  NS_ASSERT_MSG (ComputeRemaining (start, current) >= CCNxTlv::GetTLSize (), "Must have at least 4 bytes in buffer");

  // don't count the outer T_MSG bytes until after the while() loop so we
  // can compare directly against outerLength
  uint32_t bytesRead = 0;
  uint16_t outerType = CCNxTlv::ReadType (current);
  uint16_t outerLength = CCNxTlv::ReadLength (current);

  NS_ASSERT_MSG (outerType == NfpSchema::T_MSG, "Outer type must be T_MSG");

  bytesRead += DeserializeRouterName (start, current);
  bytesRead += DeserializeMessageSeqnum (start, current);

  // Now loop and read messages until the end of the packet
  while (bytesRead < outerLength)
    {
      NS_ASSERT_MSG (ComputeRemaining (start, current) >= CCNxTlv::GetTLSize (), "Must have at least 4 bytes in buffer");

      uint16_t innerType = CCNxTlv::ReadType (current);
      uint16_t innerLength = CCNxTlv::ReadLength (current);

      // these functions need us to backup so they can read the outer TL block
      current.Prev (CCNxTlv::GetTLSize ());
      uint16_t expectedReadLength = innerLength + CCNxTlv::GetTLSize ();

      uint32_t readLength = 0;
      switch (innerType)
        {
        case NfpSchema::T_ADV:
          readLength = DeserializeAdvertise (start, current);
          break;

        case NfpSchema::T_WITHDRAW:
          readLength = DeserializeWithdraw (start, current);
          break;

        default:
          NS_ASSERT_MSG (false, "Unsupported message type " << innerType << " offset " << current.GetDistanceFrom (start));

          std::terminate ();
        }

      NS_ASSERT_MSG (readLength == expectedReadLength, "readlength " << readLength << " != expected " << expectedReadLength);
      bytesRead += readLength;
    }

  NS_ASSERT_MSG (bytesRead == outerLength, "Read beyond end of message!");

  m_capacity = bytesRead + CCNxTlv::GetTLSize ();
  m_remaining = 0;

  // Add in the initial 4 bytes from the T_MSG bytes
  return bytesRead + CCNxTlv::GetTLSize ();
}

bool
NfpPayload::Equals (NfpPayload const & other) const
{
  bool result = false;
  if ((m_msgSeqnum == other.m_msgSeqnum)
      && m_routerNameCodec.Equals (other.m_routerNameCodec))
    {
      if (m_messages.size () == other.m_messages.size ())
        {
          // now assume true unless we find a mismatch
          result = true;
          for (int i = 0; i < m_messages.size (); i++)
            {
              Ptr<NfpMessage> a = m_messages[i];
              Ptr<NfpMessage> b = other.m_messages[i];
              if (!a->Equals (*b))
                {
                  result = false;
                  break;
                }
            }
        }
    }
  return result;
}

std::ostream &
ns3::ccnx::operator << (std::ostream &os, const NfpPayload &payload)
{
  os << "{ Payload: " << *payload.m_routerNameCodec.GetHeader ();
  os << ", seq " << (uint32_t) payload.GetMessageSeqnum ();
  os << ", cap " << payload.m_capacity;
  os << ", remain " << payload.GetRemaining ();
  os << ", count " << payload.GetMessageCount ();
  os << " }";

  return os;
}
