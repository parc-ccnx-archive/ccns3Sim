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

#ifndef CCNS3_CCNXFIXEDHEADER_H
#define CCNS3_CCNXFIXEDHEADER_H

#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/simulator.h"
#include "ns3/simple-ref-count.h"

namespace ns3 {
namespace ccnx {

typedef enum
{
  CCNxFixedHeaderType_Interest,
  CCNxFixedHeaderType_Object,
  CCNxFixedHeaderType_InterestReturn,
} CCNxFixedHeaderType;

/**
 * @ingroup ccnx-messages
 *
 * Represents the CCNx Fixed Header.
 */
class CCNxFixedHeader : public SimpleRefCount<CCNxFixedHeader>
{
public:
  /**
   * Create a CCNx Fixed Header.
   *
   * @param ver          The version associated with this fixed header.
   * @param packetType   The PacketType associated with this fixed header.
   * @param packetLength The PacketLength associated with this fixed header.
   * @param hopLimit     The HeaderLength associated with this fixed header.
   * @param returnCode   The ReturnCode associated with this fixed header.
   * @param headerLength The HeaderLength associated with this fixed header.
   *
   * Example
   * @code
   * Ptr<CCNxFixedHeader> fixedHeader = Create<CCNxFixedHeader>(ver, packetType, packetLength, hopLimit, returnCode,
   *                                    headerLength);
   * @endcode
   */
  CCNxFixedHeader (uint8_t ver, CCNxFixedHeaderType packetType, uint16_t packetLength,
                   uint8_t hopLimit, uint8_t returnCode, uint8_t headerLength);

  /**
   * Returns the version associated with this fixed header.
   */
  uint8_t GetVersion () const;

  /**
   * Returns the PacketType associated with this fixed header.
   */
  CCNxFixedHeaderType GetPacketType () const;

  /**
   * Returns the PacketLength associated with this fixed header.
   *
   * The PacketLength is the total size of the packet, including all headers.
   *
   * @return
   */
  uint16_t GetPacketLength () const;

  /**
   * Returns the HeaderLength associated with this fixed header.
   *
   * The HeaderLength is the size of all headers -- i.e. the number of bytes from the
   * start of the packet to the CCNxMessage (T_INTEREST or T_OBJECT).
   */
  uint8_t GetHeaderLength () const;

  /**
   * Returns the HopLimit associated with this fixed header.
   *
   * The HopLimit is the number of router hops still left in the packet's lifetime.
   * This field only applies to CCNxFixedHeaderType_Interest
   */
  uint8_t GetHopLimit () const;

  /**
   * Returns the ReturnCode associated with this fixed header.
   *
   * The ReturnCode is the reason that caused the InterestReturn to be sent.
   * This field only applies to CCNxFixedHeaderType_InterestReturn.
   */
  uint8_t GetReturnCode () const;

  /**
   * Sets the PacketLength field.
   *
   * No validation is done on the input.
   */
  void SetPacketLength (uint16_t length);

  /**
   * Sets the HeaderLength.  The HeaderLength should always be at least 8 (to account
   * for the FixedHeader) and must be less than the PacketLength.
   *
   * No input validation is done on this field.
   */
  void SetHeaderLength (uint16_t length);

  /**
   * Sets the HopLimit of this packet.  It only applies to Interest packets.
   *
   * No input validation is done on this field.
   */
  void SetHopLimit (uint8_t limit);

  /**
   * Determines if two FixedHeaders are equal.  All fields must be identical.
   */
  bool Equals (const Ptr<CCNxFixedHeader> other) const;

  /**
   * Determines if two FixedHeaders are equal.  All fields must be identical.
   */
  bool Equals (CCNxFixedHeader const &other) const;

  /**
   * Outputs a string like this:
   *
   * { Version V, PacketType A, PacketLength B, HopLimit C, ReturnCode D, HeaderLength E}
   *
   * Where V is the version of the packet, A is the packetType, B is the packet length
   * in uint16_t, C is the hop limit of the interest in uint8_t, D is the return Code in
   * uint8_t and E is the header length in uint8_t
   *
   * @param [in] os ostream object
   * @param [in] header CCNxFixedHeader object
   * @return ostream object
   */
  friend std::ostream & operator << (std::ostream & os, CCNxFixedHeader const & header);

protected:
  uint8_t m_version;
  CCNxFixedHeaderType m_packetType;
  uint16_t m_packetLength;
  uint8_t m_hopLimit;
  uint8_t m_returnCode;
  uint8_t m_headerLength;
};
std::ostream & operator << (std::ostream & os, CCNxFixedHeader const & header);

} // namespace ccnx
} // namespace ns3

#endif //CCNS3_CCNXFIXEDHEADER_H
