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

#ifndef CCNS3_CCNXPACKET_H
#define CCNS3_CCNXPACKET_H

#include <vector>

#include "ns3/packet.h"
#include "ns3/ccnx-fixedheader.h"
#include "ns3/ccnx-message.h"
#include "ns3/ccnx-validation.h"
#include "ns3/ccnx-hash-value.h"

#include "ns3/ccnx-codec-fixedheader.h"
#include "ns3/ccnx-codec-interest.h"
#include "ns3/ccnx-codec-contentobject.h"
#include "ns3/ccnx-codec-perhopheader.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-packet
 *
 * CCNx packet representation.  Includes codecs to serialize/deserialize.
 *
 * Class representation of a CCNx Packet, which is the tuple {FixedHeader, PerHopHeaders,
 * Message, ValidationAlg, ValidationSig}.
 *
 * In NS3, each part of the CCNx packet is written as an ns3::Header, so one ends up with at most
 * four ns3::Header per CCNx packet.  At minimum, there are two headers (FixedHeader, Message).
 *
 * The CCNPacket codecs are wire-format compatible with the CCNx specifications.  The main
 * interoperability limitation is that we use CCNxHashValue for KeyId and ContentObjectHash
 * to avoid using cryptographic functions in simulation.  We only use uint64_t, which the
 * user (applications) are responsible for assigning.
 *
 * The Packet carries a virtual uint64_t hash field that can be used to set the ContentObjectHash
 * value of the packet.  This is supposed to be the SHA256 of the (Message, ValidationAlg).  It is
 * carried as a PacketTag so it is not serialized as part of the packet payload.
 *
 * TODO: the virtual hash is not implemented yet.
 *
 * TODO: To access the FixedHeader you have to GetNs3Packet() to serialize it.  That's very expensive.
 *
 * You create a CCNxPacket by using one of the static CreateFromX() methods.
 * This is because there is a problem with overloading constructors in CCNxPacket because they are all flavors
 * of Ptr<foo>, which is ambiguous to the compiler.
 *
 * To pretty print a CCNxPacket, use CCNxPacket::operator <<, such as "std::cout << ccnxPacket".  If it is
 * a Ptr<CCNxPacket>, then use "std::count << *ccnxPacket".
 */
class CCNxPacket : public SimpleRefCount<CCNxPacket>
{
public:
  static Ptr<CCNxPacket> CreateFromMessage (Ptr<CCNxMessage> message);
  static Ptr<CCNxPacket> CreateFromMessage (Ptr<CCNxMessage> message, Ptr<CCNxValidation> validation);

  /**
   * Deserialize the packet and instantiate the CCNx objects.
   */
  static Ptr<CCNxPacket> CreateFromNs3Packet (Ptr<const Packet> packet);

  virtual ~CCNxPacket ();

  Ptr<CCNxFixedHeader> GetFixedHeader () const;

  Ptr<CCNxMessage> GetMessage () const;

  Ptr<CCNxValidation> GetValidation () const;

  CCNxPerHopHeader GetPerhopHeaders() const;

  void AddPerHopHeaderEntry (Ptr<CCNxPerHopHeaderEntry> perHopHeaderEntry);

  /**
   * This serializes in to the NS3 Packet format and returns the result.
   *
   * The first call creates a Packet and caches it.  Subsequent calls return
   * ns3::Packet::Copy() of it.
   */
  Ptr<Packet> CreateNs3Packet ();

  /**
   * Set a virtual ContentObjectHash on the packet.  The hash is carried along with the packet to
   * allow "calculating" the hash without using a SHA.  It is carried in an ns3::Packet as a PacketTag.
   */
  void SetContentObjectHash (CCNxHashValue hash);

  /**
   * Get a virtual ContentObjectHash on the packet.  The hash is carried along with the packet to
   * allow "calculating" the hash without using a SHA.  It is carried in an ns3::Packet as a PacketTag.
   */
  CCNxHashValue GetContentObjectHash (void) const;

protected:
  /**
   * Generic packet creation, use one of the static factories.
   */
  CCNxPacket ();


  Ptr<Packet> GenerateNs3Packet (void);

  Ptr<CCNxFixedHeader> GenerateFixedHeader (CCNxMessage::MessageType messageType = CCNxMessage::Interest) const;

  CCNxFixedHeaderType GetMessageTypeAsPacketType (CCNxMessage::MessageType messageType) const;

  uint32_t GetPerHopHeaderLength (void) const;

  uint8_t GetHopLimit (void) const;

  uint8_t GetReturnCode (void) const;

  size_t ComputePacketSize (void) const;

  virtual void DoDispose (void);

  /**
   * Set the fixed header, message, validation from the m_ns3Packet member.
   */
  void Deserialize ();

  /**
   * Trim m_ns3Packet to the length specified in the Fixed Header.
   */
  void TrimNs3Packet ();

  CCNxCodecFixedHeader m_codecFixedHeader;
  CCNxCodecInterest m_codecInterest;
  CCNxCodecContentObject m_codecContentObject;
  CCNxCodecPerHopHeader m_codecPerHopHeader;

  Ptr<CCNxMessage> m_message;
  Ptr<CCNxValidation> m_validation;
  Ptr<Packet> m_ns3Packet;
  CCNxHashValue m_hash;                              //<! virtual PacketTag for ContentObjectHash
};

std::ostream &operator<< (std::ostream &os, CCNxPacket const &packet);
}
}

#endif //CCNS3_CCNXPACKET_H
