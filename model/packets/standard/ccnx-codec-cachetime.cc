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
#include "ccnx-codec-cachetime.h"

using namespace ns3;
using namespace ns3::ccnx;


NS_LOG_COMPONENT_DEFINE ("CCNxCodecCachetime");

NS_OBJECT_ENSURE_REGISTERED (CCNxCodecCachetime);

static bool _registered = false;
static void
RegisterCodec()
{
    if (!_registered) {
	_registered = true;
	Ptr<CCNxCodecCachetime> codec = CreateObject<CCNxCodecCachetime>();
	CCNxCodecRegistry::PerHopRegisterCodec(CCNxCachetime::GetTLVType(), codec);
    }
}

TypeId
CCNxCodecCachetime::GetTypeId (void)
{
    RegisterCodec();
    static TypeId tid = TypeId ("ns3::ccnx::CCNxCodecCachetime")
    .SetParent<CCNxCodecPerHopHeaderEntry> ()
    .SetGroupName ("CCNx")
    .AddConstructor<CCNxCodecCachetime>();
    return tid;
}

TypeId
CCNxCodecCachetime::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

CCNxCodecCachetime::CCNxCodecCachetime ()
{
  // empty
}

CCNxCodecCachetime::~CCNxCodecCachetime ()
{
  // empty
}

Ptr<CCNxPerHopHeaderEntry>
CCNxCodecCachetime::Deserialize (Buffer::Iterator *inputIterator, size_t *bytesRead)
{
    NS_LOG_FUNCTION (this << &inputIterator);
    NS_ASSERT_MSG (inputIterator->GetSize () >= CCNxTlv::GetTLSize(), "Need to have at least 4 bytes to read");
    Buffer::Iterator *iterator = inputIterator;

    uint32_t numBytes = 0;
    uint16_t type = CCNxTlv::ReadType (*iterator);
    uint16_t length = CCNxTlv::ReadLength (*iterator);
    numBytes += CCNxTlv::GetTLSize();

    NS_LOG_DEBUG ("Type " << type << " length " << length << " bytesRead " << numBytes);

    NS_ASSERT_MSG (length >= 8, "Must be at least 8 bytes in Cache time");

    uint64_t value = iterator->ReadNtohU64 ();

    NS_LOG_DEBUG ("De-serialized Cache time" << value);

    Ptr<CCNxTime> time = Create<CCNxTime>(value);
    NS_ASSERT_MSG ((time), "Unable to create CCNxTime");

    Ptr<CCNxCachetime> cachetime = Create<CCNxCachetime> (time);
    NS_ASSERT_MSG ((cachetime), "Unable to create CCNxCachetime");

    numBytes += length;

    *bytesRead = numBytes;
    return cachetime;
}

uint32_t
CCNxCodecCachetime::GetSerializedSize (Ptr<CCNxPerHopHeaderEntry> perhopEntry)
{
    Ptr<CCNxCachetime> cachetime = DynamicCast<CCNxCachetime, CCNxPerHopHeaderEntry> (perhopEntry);
    Ptr<CCNxTime> time = cachetime->GetCachetime();
    NS_ASSERT_MSG (time, "Time can not be NULL");
    uint32_t length = CCNxTlv::GetTLSize () + 8; //Cache time is 8 bytes fixed
    return length;
}

void
CCNxCodecCachetime::Serialize (Ptr<CCNxPerHopHeaderEntry> perhopEntry, Buffer::Iterator *outputIterator)
{
    NS_LOG_FUNCTION (this << &outputIterator);

    uint16_t bytes = (uint16_t) GetSerializedSize (perhopEntry);
    NS_ASSERT_MSG (bytes >= CCNxTlv::GetTLSize(), "Serialized size must be at least 4 bytes");

    // -4 because it includes the T_CACHE_TIME TLV.
    CCNxTlv::WriteTypeLength (*outputIterator, CCNxSchemaV1::T_CACHE_TIME, bytes - CCNxTlv::GetTLSize ());

    Ptr<CCNxCachetime> cachetime = DynamicCast<CCNxCachetime, CCNxPerHopHeaderEntry> (perhopEntry);
    Ptr<CCNxTime> time = cachetime->GetCachetime();
    NS_ASSERT_MSG (time, "Time can not be NULL");
    outputIterator->WriteHtonU64 (time->getTime ());
}

void
CCNxCodecCachetime::Print (Ptr<CCNxPerHopHeaderEntry> perhopEntry, std::ostream &os) const
{
  if (perhopEntry)
  {
      Ptr<CCNxCachetime> cachetime = DynamicCast<CCNxCachetime, CCNxPerHopHeaderEntry> (perhopEntry);
      cachetime->Print(os);
  }
  else
  {
      os << "\nNo cache time header";
  }
}

