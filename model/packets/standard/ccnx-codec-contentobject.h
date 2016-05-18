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

#ifndef CCNS3SIM_CCNXCODECCONTENTOBJECT_H
#define CCNS3SIM_CCNXCODECCONTENTOBJECT_H

#include "ns3/header.h"
#include "ns3/ccnx-contentobject.h"
#include "ns3/ccnx-codec-name.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-packet
 *
 * Codec for reading/writing a CCNxContentObject message
 */
class CCNxCodecContentObject : public Header
{
public:
  /**
   * Return the ns3::Object type
   * @return The RTTI of this object
   */
  static TypeId GetTypeId (void);

  virtual TypeId GetInstanceTypeId (void) const;

  // virtual from Header
  /**
   * Computes the byte length of the encoded TLV.  Does not do
   * any encoding (it's const).
   */
  virtual uint32_t GetSerializedSize (void) const;

  /**
   * Serializes this object into the Buffer::Iterator.  it is the responsibility
   * of the caller to ensure there is at least GetSerializedSize() bytes available.
   *
   * @param [out] output The buffer position to begin writing.
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * Reads from the Buffer::Iterator and creates an object instantiation of the buffer.
   *
   * The buffer should point to the beginning of the T_OBJECT TLV.
   *
   * @param [in] input The buffer to read from
   * @return The number of bytes processed.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * Display this codec's state to the provided output stream.
   *
   * @param [in] os The output stream to write to
   */
  virtual void Print (std::ostream &os) const;

  // subclass
  CCNxCodecContentObject ();

  virtual ~CCNxCodecContentObject ();

  /**
   * Gets the Content Object's pointer.  Could be from Deserialize() or from
   * SetHeader().
   */
  Ptr<CCNxContentObject> GetHeader () const;

  /**
   * Sets the Content Object to the given value.  Used when serializing.
   */
  void SetHeader (Ptr<CCNxContentObject> content);

  /**
   * Maps the enum payload type to the schema value.
   */
  static uint8_t SerializePayloadTypeToSchemaValue (CCNxContentObjectPayloadType type);

  /**
   * Maps the schema value to the enum payload type.
   */
  static CCNxContentObjectPayloadType DeserializeSchemaValueToPayloadType (uint8_t type);

private:
  /**
   * The Content Object to serialize (from SetHeader) or the Content Object we got from
   * Deserialize().
   */
  Ptr<CCNxContentObject> m_content;
  CCNxCodecName m_nameCodec;

  /**
   * The start iterator points to the T_NAME Value.  It will be rewinded 4
   * bytes and passed to m_nameCodec for processing.  Side effect is that m_nameCodec
   * will be updated with the Content Object's name.
   */
  Ptr<const CCNxName>  DeserializeName (Buffer::Iterator &start, uint16_t length);

  /**
   * start points to the T_PAYLDTYPE Value.  Reads 1 byte.
   */
  CCNxContentObjectPayloadType DeserializePayloadType (Buffer::Iterator &start, uint16_t length);

  /**
   * start points to the T_EXPIRY Value.  Reads 8 bytes of U64 in network
   * byte order and returns it in host order.
   */
  Ptr<CCNxTime> DeserializeExpiryTime (Buffer::Iterator &start, uint16_t length);

  /**
   * start points to the T_PAYLOAD Value.  Reads the whole payload.
   */
  Ptr<CCNxBuffer> DeserializePayload (Buffer::Iterator &start, uint16_t length);
};
}
}

#endif //CCNS3SIM_CCNXCODECCONTENTOBJECT_H
