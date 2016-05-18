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

#ifndef CCNS3SIM_CCNXCODECPERHOPHEADER_H
#define CCNS3SIM_CCNXCODECPERHOPHEADER_H

#include "ns3/header.h"
#include "ns3/ccnx-perhopheader.h"
#include "ns3/ccnx-codec-perhopheaderentry.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-packet
 *
 * Codec for reading/writing CCNx per hop headers.
 * This represents complete block of all per hop headers
 *
 */
class CCNxCodecPerHopHeader : public Header
{
public:
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
   * @param [in] output The buffer position to begin writing.
   */
  virtual void Serialize (Buffer::Iterator output) const;

  /**
   * Because the per-hop headers are simply a list of TLVs, we do not know how much to read.
   *
   * @param [in] length
   */
   void SetDeserializeLength(uint32_t length);

  /**
   * Reads from the Buffer::Iterator and creates an object instantiation of the buffer.
   *
   * The buffer should point to the beginning of the T_OBJECT TLV.
   *
   * @param [in] input The buffer to read from
   * @return The number of bytes processed.
   */
  virtual uint32_t Deserialize (Buffer::Iterator input);

  /**
   * Display this codec's state to the provided output stream.
   *
   * @param [in] os The output stream to write to
   */
  virtual void Print (std::ostream &os) const;

  /**
   * Constructor for CCNxCodecPerHopHeader
   */
  CCNxCodecPerHopHeader ();

  /**
   * Destructor for CCNxCodecPerHopHeader
   */
  virtual ~CCNxCodecPerHopHeader ();

  /**
   * Returns the internal CCNxPerHopHeader
   */
  Ptr<CCNxPerHopHeader> GetHeader () const;

protected:
  Ptr<CCNxPerHopHeader> m_perHopHeader;
  /**
   * The number of bytes to Deserialize, based on `SetDeserializeLength()`
   */
  uint32_t m_deserializeLength;
};

} // namespace ccnx
} // namespace ns3


#endif //CCNS3SIM_CCNXCODECPERHOPHEADER_H
