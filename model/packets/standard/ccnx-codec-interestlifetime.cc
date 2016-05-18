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
#include "ns3/ccnx-schema-v1.h"
#include "ccnx-codec-registry.h"
#include "ccnx-codec-perhopheader.h"
#include "ccnx-codec-interestlifetime.h"

using namespace ns3;
using namespace ns3::ccnx;


NS_LOG_COMPONENT_DEFINE ("CCNxCodecInterestLifetime");

NS_OBJECT_ENSURE_REGISTERED (CCNxCodecInterestLifetime);

static bool _registered = false;
static void
RegisterCodec()
{
    if (!_registered) {
	_registered = true;
	    Ptr<CCNxCodecInterestLifetime> codec = CreateObject<CCNxCodecInterestLifetime>();
	CCNxCodecRegistry::PerHopRegisterCodec(CCNxInterestLifetime::GetTLVType(), codec);
    }
}

TypeId
CCNxCodecInterestLifetime::GetTypeId (void)
{
    RegisterCodec();
    static TypeId tid = TypeId ("ns3::ccnx::CCNxCodecInterestLifetime")
    .SetParent<CCNxCodecPerHopHeaderEntry> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxCodecInterestLifetime>();
    return tid;
}

TypeId
CCNxCodecInterestLifetime::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

CCNxCodecInterestLifetime::CCNxCodecInterestLifetime ()
{
  // empty
}

CCNxCodecInterestLifetime::~CCNxCodecInterestLifetime ()
{
  // empty
}

Ptr<CCNxPerHopHeaderEntry>
CCNxCodecInterestLifetime::Deserialize (Buffer::Iterator inputIterator, size_t *bytesRead)
{
    NS_LOG_FUNCTION (this << &inputIterator);
    NS_ASSERT_MSG (inputIterator.GetSize () >= CCNxTlv::GetTLSize(), "Need to have at least 4 bytes to read");
    Buffer::Iterator iterator = inputIterator;

    uint32_t numBytes = 0;
    uint16_t type = CCNxTlv::ReadType (iterator);
    uint16_t length = CCNxTlv::ReadLength (iterator);
    numBytes += CCNxTlv::GetTLSize();

    NS_LOG_DEBUG ("Type " << type << " length " << length << " bytesRead " << numBytes);

    uint64_t value = 0;
    uint16_t tempLen = length;
    while (tempLen > 0)
    {
	    value |= iterator.ReadU8();
	    value <<= 8;
	    tempLen--;
    }

    NS_LOG_DEBUG ("De-serialized Interest Lifetime" << value);

    Ptr<CCNxTime> time = Create<CCNxTime>(value);
    NS_ASSERT_MSG ((time), "Unable to create CCNxTime");

    Ptr<CCNxInterestLifetime> interestLifetime = Create<CCNxInterestLifetime> (time);
    NS_ASSERT_MSG ((interestLifetime), "Unable to create CCNxInterestLifetime");

    numBytes += length;

    *bytesRead = numBytes;
    return interestLifetime;
}

static uint32_t CCNxCodecTlvEncoder_ComputeVarIntLength(uint64_t value)
{
    unsigned length = 8;
    if (value <= 0x00000000000000FFULL) {
        length = 1;
    } else if (value <= 0x000000000000FFFFULL) {
        length = 2;
    } else if (value <= 0x0000000000FFFFFFULL) {
        length = 3;
    } else if (value <= 0x00000000FFFFFFFFULL) {
        length = 4;
    } else if (value <= 0x000000FFFFFFFFFFULL) {
        length = 5;
    } else if (value <= 0x0000FFFFFFFFFFFFULL) {
        length = 6;
    } else if (value <= 0x00FFFFFFFFFFFFFFULL) {
        length = 7;
    }

    return length;
}

uint32_t
CCNxCodecInterestLifetime::GetSerializedSize (Ptr<CCNxPerHopHeaderEntry> perhopEntry)
{
    Ptr<CCNxInterestLifetime> interestLifetime = DynamicCast<CCNxInterestLifetime, CCNxPerHopHeaderEntry> (perhopEntry);
    Ptr<CCNxTime> time = interestLifetime->GetInterestLifetime();
    NS_ASSERT_MSG (time, "Time can not be NULL");
    uint32_t length = CCNxTlv::GetTLSize () + CCNxCodecTlvEncoder_ComputeVarIntLength(time->getTime());
    return length;
}

void
CCNxCodecInterestLifetime::Serialize (Ptr<CCNxPerHopHeaderEntry> perhopEntry, Buffer::Iterator outputIterator)
{
    NS_LOG_FUNCTION (this << &outputIterator);

    uint16_t bytes = (uint16_t) GetSerializedSize (perhopEntry);
    NS_ASSERT_MSG (bytes >= CCNxTlv::GetTLSize(), "Serialized size must be at least 4 bytes");

    // -4 because it includes the T_INTEREST TLV.
    CCNxTlv::WriteTypeLength (outputIterator, CCNxSchemaV1::T_INT_LIFE, bytes - CCNxTlv::GetTLSize ());

    Ptr<CCNxInterestLifetime> interestLifetime = DynamicCast<CCNxInterestLifetime, CCNxPerHopHeaderEntry> (perhopEntry);
    Ptr<CCNxTime> time = interestLifetime->GetInterestLifetime();
    NS_ASSERT_MSG (time, "Time can not be NULL");
    uint64_t value = time->getTime();

    bool mustContinue = false;
    for (int byte = 7; byte >= 0; byte--) {
    	uint8_t b = (value >> (byte * 8)) & 0xFF;
    	if (b != 0 || byte == 0 || mustContinue) {
    		outputIterator.WriteU8 (b);
			mustContinue = true;
        }
    }
}

void
CCNxCodecInterestLifetime::Print (Ptr<CCNxPerHopHeaderEntry> perhopEntry, std::ostream &os) const
{
  if (perhopEntry)
  {
      Ptr<CCNxInterestLifetime> interestLifetime = DynamicCast<CCNxInterestLifetime, CCNxPerHopHeaderEntry> (perhopEntry);
      os << interestLifetime->Print(os);
  }
  else
  {
      os << "No interest lifetime header";
  }
}

