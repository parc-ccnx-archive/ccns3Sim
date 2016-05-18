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

#ifndef CCNS3SIM_NFPADVERTISE_H
#define CCNS3SIM_NFPADVERTISE_H

#include "ns3/buffer.h"
#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-codec-name.h"

#include "ns3/nfp-message.h"

namespace ns3 {
namespace ccnx {

/**
 * \ingroup nfp-routing
 *
 * An advertisement inside an NfpMessage.  See NfpSchema.h for packet format.
 */
class NfpAdvertise : public NfpMessage
{
public:
  NfpAdvertise ();
  NfpAdvertise (Ptr<const CCNxName> anchorName, Ptr<const CCNxName> prefix, uint32_t anchorSeqnum, uint16_t distance);

  /**
   * Create a new advertisement based on an old one.  The distnace will be increased based on
   * the `linkcost` parameter.  This is typically used by a node to re-advertise a route with
   * increased distance due to the link cost to the nexthop.
   *
   * @param copy The original advertisement
   * @param linkcost The linkcost by which to increase the distance.
   */
  NfpAdvertise (const NfpAdvertise &copy, uint16_t linkcost);

  virtual ~NfpAdvertise ();

  Ptr<const CCNxName> GetAnchorName (void) const;
  Ptr<const CCNxName> GetPrefix (void) const;
  uint32_t GetAnchorSeqnum (void) const;
  uint16_t GetDistance (void) const;

  /**
   * Return the number of bytes required to serialize this header
   */
  virtual uint32_t GetSerializedSize (void) const;

  /**
   * Serialize the FixedHeader set via SetHeader() or Deserialize() in to
   * the given Buffer.
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * Deserialize the given buffer and set the internal CCNxFixedHeader.
   * You can then use GetHeader() to see the deserialized object.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * True if this represents an Advertisement
   */
  virtual bool IsAdvertise (void) const;

  /**
   * True if this represents a withdrawl
   */
  virtual bool IsWithdraw (void) const;

  /**
   * Determine if this object is equal to other.  For two objects to be equal, they
   * must be member-wise equal.
   */
  bool Equals (NfpAdvertise const & other) const;

  virtual bool Equals (NfpMessage const & other) const;

protected:
  CCNxCodecName m_anchorNameCodec;
  CCNxCodecName m_prefixCodec;

  uint32_t m_anchorSeqnum;
  uint16_t m_distance;
};

std::ostream & operator << (std::ostream & os, NfpAdvertise const & header);

}   // namespace ccnx
} // namespace ns3


#endif //CCNS3SIM_NFPADVERTISE_H
