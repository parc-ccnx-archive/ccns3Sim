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
#include "nfp-withdraw.h"
#include "ns3/ccnx-tlv.h"
#include "ns3/nfp-schema.h"

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("NfpWithdraw");

NfpWithdraw::NfpWithdraw ()
{
  // empty
}

NfpWithdraw::NfpWithdraw (Ptr<const CCNxName> anchorName, Ptr<const CCNxName> prefix)
{
  NS_LOG_FUNCTION (this << anchorName << prefix);
  m_anchorNameCodec.SetHeader (anchorName);
  m_prefixCodec.SetHeader (prefix);
}

NfpWithdraw::~NfpWithdraw ()
{
  // empty;
}

Ptr<const CCNxName>
NfpWithdraw::GetAnchorName (void) const
{
  return m_anchorNameCodec.GetHeader ();
}

Ptr<const CCNxName>
NfpWithdraw::GetPrefix (void) const
{
  return m_prefixCodec.GetHeader ();
}

uint32_t
NfpWithdraw::GetSerializedSize (void) const
{
  uint32_t size = CCNxTlv::GetTLSize () + m_anchorNameCodec.GetSerializedSize () + m_prefixCodec.GetSerializedSize ();
  return size;
}

void
NfpWithdraw::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  uint16_t totalSize = GetSerializedSize ();
  uint16_t innerSize = totalSize - CCNxTlv::GetTLSize ();

  CCNxTlv::WriteTypeLength (start, NfpSchema::T_WITHDRAW, innerSize);
  m_anchorNameCodec.Serialize (start);
  start.Next (m_anchorNameCodec.GetSerializedSize ());

  m_prefixCodec.Serialize (start);
  start.Next (m_prefixCodec.GetSerializedSize ());
}

uint32_t
NfpWithdraw::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  NS_ASSERT_MSG (start.GetSize () >= CCNxTlv::GetTLSize (), "Must have at least 4 bytes in buffer");

  // don't count the outer T_WITHDRAW bytes until after the while() loop so we
  // can compare directly against outerLength
  uint32_t bytesRead = 0;
  uint16_t outerType = CCNxTlv::ReadType (start);
  uint16_t outerLength = CCNxTlv::ReadLength (start);

  NS_ASSERT_MSG (outerType == NfpSchema::T_WITHDRAW, "Outer type must be T_WITHDRAW");

  uint32_t anchorSize = m_anchorNameCodec.Deserialize (start);
  bytesRead += anchorSize;
  start.Next (anchorSize);

  uint32_t prefixSize = m_prefixCodec.Deserialize (start);
  bytesRead += prefixSize;
  start.Next (prefixSize);

  NS_ASSERT_MSG (bytesRead == outerLength, "Did not consume all bytes");

  // Add in the initial 4 bytes from the T_WITHDRAW bytes
  return bytesRead + CCNxTlv::GetTLSize ();
}

bool
NfpWithdraw::IsAdvertise (void) const
{
  return false;
}

bool
NfpWithdraw::IsWithdraw (void) const
{
  return true;
}

bool
NfpWithdraw::Equals (NfpWithdraw const &other) const
{
  bool result = false;

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

  return result;
}

bool
ns3::ccnx::operator == (const Ptr<const NfpWithdraw> a, const Ptr<const NfpWithdraw> b)
{
  bool result = false;
  if (a && b)
    {
      result = a->Equals (*b);
    }
  return result;
}

bool
NfpWithdraw::Equals (NfpMessage const & other) const
{
  bool result = false;
  if (other.IsWithdraw ())
    {
      const NfpWithdraw *a = dynamic_cast<const NfpWithdraw *> (&other);
      result = Equals (*a);
    }
  return result;
}

std::ostream &
ns3::ccnx::operator << (std::ostream & os, NfpWithdraw const & header)
{
  os << "{ Wdr: ";
  os << *header.GetAnchorName () << ", ";
  os << *header.GetPrefix () << " }";
  return os;
}
