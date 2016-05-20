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

#include <stdint.h>
#include "ns3/log.h"
#include "nfp-advertise.h"
#include "ns3/ccnx-tlv.h"
#include "ns3/nfp-schema.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("NfpAdvertise");

NfpAdvertise::NfpAdvertise ()
{
  m_anchorSeqnum = 0;
  m_distance = 0;
}

NfpAdvertise::NfpAdvertise (Ptr<const CCNxName> anchorName, Ptr<const CCNxName> prefix, uint32_t anchorSeqnum, uint16_t distance)
{
  NS_LOG_FUNCTION (this << anchorName << prefix << anchorSeqnum << distance);

  m_anchorNameCodec.SetHeader (anchorName);
  m_prefixCodec.SetHeader (prefix);
  m_anchorSeqnum = anchorSeqnum;
  m_distance = distance;
}

NfpAdvertise::NfpAdvertise (NfpAdvertise const &copy, uint16_t linkcost)
{
  NS_LOG_FUNCTION (this << copy << linkcost);
  m_anchorNameCodec.SetHeader (copy.GetAnchorName ());
  m_prefixCodec.SetHeader (copy.GetPrefix ());
  m_anchorSeqnum = copy.GetAnchorSeqnum ();

  uint32_t newDistance = (uint32_t) copy.GetDistance () + linkcost;
  NS_ASSERT_MSG (newDistance <= std::numeric_limits<uint16_t>::max (), "distance + linkcost exceed 16-bits");
  m_distance = (uint16_t) newDistance;
}

NfpAdvertise::~NfpAdvertise ()
{
  // empty;
}

Ptr<const CCNxName>
NfpAdvertise::GetAnchorName (void) const
{
  return m_anchorNameCodec.GetHeader ();
}

Ptr<const CCNxName>
NfpAdvertise::GetPrefix (void) const
{
  return m_prefixCodec.GetHeader ();
}

uint32_t
NfpAdvertise::GetAnchorSeqnum (void) const
{
  return m_anchorSeqnum;
}

uint16_t
NfpAdvertise::GetDistance (void) const
{
  return m_distance;
}

uint32_t
NfpAdvertise::GetSerializedSize (void) const
{
  uint32_t size = CCNxTlv::GetTLSize ();

  size += m_anchorNameCodec.GetSerializedSize ();
  size += m_prefixCodec.GetSerializedSize ();
  size += CCNxTlv::GetTLSize ();
  size += sizeof(uint32_t) + sizeof(uint16_t);
  return size;
}

void
NfpAdvertise::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  uint32_t totalSize = GetSerializedSize ();
  NS_ASSERT_MSG (totalSize <= 0xFFFF, "Total size too large");

  uint16_t innerSize = (uint16_t) totalSize - CCNxTlv::GetTLSize ();

  CCNxTlv::WriteTypeLength (start, NfpSchema::T_ADV, innerSize);
  m_anchorNameCodec.Serialize (start);
  start.Next (m_anchorNameCodec.GetSerializedSize ());

  m_prefixCodec.Serialize (start);
  start.Next (m_prefixCodec.GetSerializedSize ());

  CCNxTlv::WriteTypeLength (start, NfpSchema::T_ADV_DATA, 6);
  start.WriteHtonU32 (m_anchorSeqnum);
  start.WriteHtonU16 (m_distance);
}

uint32_t
NfpAdvertise::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  NS_ASSERT_MSG (start.GetSize () >= CCNxTlv::GetTLSize (), "Must have at least 4 bytes in buffer");

  // don't count the outer T_WITHDRAW bytes until after the while() loop so we
  // can compare directly against outerLength
  uint32_t bytesRead = 0;
  uint16_t outerType = CCNxTlv::ReadType (start);
  uint16_t outerLength = CCNxTlv::ReadLength (start);

  NS_ASSERT_MSG (outerType == NfpSchema::T_ADV, "Outer type must be T_ADV");

  uint32_t anchorSize = m_anchorNameCodec.Deserialize (start);
  bytesRead += anchorSize;
  start.Next (anchorSize);

  uint32_t prefixSize = m_prefixCodec.Deserialize (start);
  bytesRead += prefixSize;
  start.Next (prefixSize);

  uint16_t innerType = CCNxTlv::ReadType (start);
  uint16_t innerLength = CCNxTlv::ReadLength (start);
  NS_ASSERT_MSG (innerType == NfpSchema::T_ADV_DATA, "Inner type must be T_ADV_DATA");
  NS_ASSERT_MSG (innerLength == 6, "Inner type must be length 6");

  m_anchorSeqnum = start.ReadNtohU32 ();
  m_distance = start.ReadNtohU16 ();

  bytesRead += CCNxTlv::GetTLSize () + innerLength;

  NS_ASSERT_MSG (bytesRead == outerLength, "Did not consume all bytes");

  // Add in the initial 4 bytes from the T_WITHDRAW bytes
  return bytesRead + CCNxTlv::GetTLSize ();
}

bool
NfpAdvertise::IsAdvertise (void) const
{
  return true;
}

bool
NfpAdvertise::IsWithdraw (void) const
{
  return false;
}

bool
NfpAdvertise::Equals (NfpAdvertise const &other) const
{
  bool result = false;

  if (m_anchorSeqnum == other.m_anchorSeqnum && m_distance == other.m_distance)
    {
      Ptr<const CCNxName> a;
      Ptr<const CCNxName> b;

      a = m_anchorNameCodec.GetHeader ();
      b = other.m_anchorNameCodec.GetHeader ();

      if (a && b)
        {
          if (a->Equals (*b))
            {
              a = m_prefixCodec.GetHeader ();
              b = other.m_prefixCodec.GetHeader ();

              if (a && b)
                {
                  if (a->Equals (*b))
                    {
                      result = true;
                    }
                }
            }
        }
    }

  return result;
}

bool
NfpAdvertise::Equals (NfpMessage const & other) const
{
  bool result = false;
  if (other.IsAdvertise ())
    {
      const NfpAdvertise *a = dynamic_cast<const NfpAdvertise *> (&other);
      result = Equals (*a);
    }
  return result;
}

std::ostream &
ns3::ccnx::operator << (std::ostream & os, NfpAdvertise const & header)
{
  os << "{ Adv: ";
  os << *header.GetAnchorName () << ", ";
  os << *header.GetPrefix () << ", seqno : ";
  os << header.GetAnchorSeqnum () << ", dist : ";
  os << header.GetDistance () << " }";
  return os;
}
