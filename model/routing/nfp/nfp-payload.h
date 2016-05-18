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

#ifndef CCNS3SIM_NFPPAYLOAD_H
#define CCNS3SIM_NFPPAYLOAD_H

#include <vector>

#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-codec-name.h"

#include "ns3/nfp-advertise.h"
#include "ns3/nfp-withdraw.h"

namespace ns3 {
namespace ccnx {

/**
 * \ingroup nfp-routing
 *
 * \brief NFP data is carried in the Payload field of a CCNxInterest.
 *
 * The payload of an Interest.
 */
class NfpPayload : public SimpleRefCount<NfpPayload>
{
public:
  /**
   * The empty constructor is used to make an object for deserialization.
   */
  NfpPayload ();

  /**
   * Create an Payload with only the Router Name and Message Sequence Number.
   * You need to Append advertisements and withdraw messages.
   *
   * The Payload will have a maximum size of capacity.  If you try to append
   * a message that serializes beyond that, it will cause an assert.  you should
   * always check the remaining capacity via GetRemaining().
   *
   * The RouterName and msgSeqnum count against the capacity.  The capacity
   * is the maximum total serialize size of the payload.
   *
   * NFP will only accept messages from RouterName that are non-decreasing
   * message sequence number.  Therefore, make sure you transmit the payloads
   * in their numerical order.
   *
   * @param capacity The maximum number of bytes
   * @param routerName The router's name
   * @param msgSeqnum The message sequence number.
   */
  NfpPayload (size_t capacity, Ptr<const CCNxName> routerName, uint16_t msgSeqnum);

  virtual ~NfpPayload ();

  /**
   * Returns the router's name in the header of the message.
   *
   * @return The router's name
   */
  Ptr<const CCNxName> GetRouterName (void) const;

  /**
   * Returns the message sequence number for the advertising RouterName.
   *
   * @return Message sequence number.
   */
  uint16_t GetMessageSeqnum (void) const;

  /**
   * Appends a message (Advertise or Withdraw) to the payload.
   *
   * You must ensure it will fit before calling this method.  Get the
   * remaining capacity via GetCapacity() and make sure it is no smaller
   * than the message's GetSerializeSize().  This method will assert if
   * you append something that would extend beyond the remaining
   * capacity.
   *
   * @param advertisement The message to add
   */
  void AppendMessage (Ptr<NfpMessage> message);

  /**
   * Returns the number of messages contained in this payload.
   *
   * @return Number of messages.
   */
  size_t GetMessageCount (void) const;

  /**
   * Returns the 'index' message from the payload.  Will assert if
   * index is not less than GetMessageCount().
   *
   * @param index The 0-based index of the message to get.
   * @return A smart pointer to the message.
   */
  Ptr<NfpMessage> GetMessage (size_t index) const;

  /**
   * Returns the amount of remaining capacity in the payload.
   *
   * @return Bytes remaining
   */
  size_t GetRemaining (void) const;

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
   * Determines if this message is equal to another message.
   *
   * For two messages to be equal they must (a) have same router name,
   * (b) have same msgSeqnum, (c) have the same number of elements in the
   * message array, and (d) each entry in the message array must be equal.
   *
   * @param other The other NfpPayload to compare against.
   * @return true when equal, false otherwise
   */
  bool Equals (NfpPayload const & other) const;

  friend std::ostream & operator << (std::ostream &os, const NfpPayload &payload);

protected:
  size_t m_capacity;
  size_t m_remaining;

  CCNxCodecName m_routerNameCodec;
  uint16_t m_msgSeqnum;

  uint32_t DeserializeRouterName (Buffer::Iterator const &start, Buffer::Iterator &current);
  uint32_t DeserializeMessageSeqnum (Buffer::Iterator const &start, Buffer::Iterator &current);
  uint32_t DeserializeAdvertise (Buffer::Iterator const &start, Buffer::Iterator &current);
  uint32_t DeserializeWithdraw (Buffer::Iterator const &start, Buffer::Iterator &current);

  typedef std::vector< Ptr<NfpMessage> > MessageListType;
  MessageListType m_messages;
};
}
}


#endif //CCNS3SIM_NFPPAYLOAD_H
