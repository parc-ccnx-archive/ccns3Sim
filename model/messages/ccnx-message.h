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

#ifndef CCNS3_CCNXMESSAGE_H
#define CCNS3_CCNXMESSAGE_H

#include "ns3/object.h"
#include "ns3/ccnx-name.h"
#include "ns3/buffer.h"
#include "ns3/ccnx-buffer.h"

namespace ns3  {
namespace ccnx {

/**
 * @ingroup ccnx-messages
 *
 * CCNxMessage is the core of all CCNx messages.  Interest and ContentObject are derived from it.
 *
 * Abstract base class for all CCNx messages.  One can determine the implementation type
 * by calling GetMessageType().
 *
 * A CCNxMessage may have either a physical payload, a virtual payload (just a size), or no payload.
 * A physical payload means there are bytes in a buffer that will be serialized/deserialized with the packet.
 * A virtual payload size is just a length for payload without any actual bytes being serialized.
 * No payload means the physical payload is null (Ptr<CCNxBuffer>(0)) and the virtual payload size is 0.
 * A CCNxMessage cannot have both a physical payload and a virtual payload size, at most one can be set.
 *
 * Implementation classes: CCNxInterest, CCNxContentObject.
 */
class CCNxMessage : public Object
{
public:
  static TypeId GetTypeId (void);

  /**
   * Base class constructor of fields that all CCNxMessages may have.
   *
   * The name could be empty (an empty Ptr object).
   */
  CCNxMessage (Ptr<const CCNxName> name);

  /**
   * Create a message that has an actual payload
   */
  CCNxMessage (Ptr<const CCNxName> name, Ptr<CCNxBuffer> payload);

  /**
   * Create a CCNxMessage without an actual payload field, but pretend it has one
   * with the given virtual payload length (in bytes).
   *
   * @param [in] name The CCNxName in the message
   * @param [in] VirtualPayloadLength The number of virtual payload bytes to carry
   */
  CCNxMessage (Ptr<const CCNxName> name, size_t VirtualPayloadLength);

  /**
   * Destructor for CCN Message
   */
  ~CCNxMessage ();

  /**
   * Returns the name the message was created with.
   */
  Ptr<const CCNxName> GetName () const;

  /**
   * If CCNxMessage::HasPayload() is true, returns the size of the payload CCNxBuffer.  If it
   * is false, return the virtual payload length.
   *
   * @return The number of bytes of virtual or real payload.
   */
  size_t GetPayloadSize () const;

  /**
   * Returns the payload field.  If the packet has a virtual payload, this call will return
   * Ptr<CCnxBuffer>(0).
   *
   * @return non-null The actual payload
   * @return null If no payload or only virtual payload (NULL = Ptr<CCNxBuffer>(0)).
   */
  Ptr<CCNxBuffer> GetPayload () const;

  /**
   * Returns whether message has a payload or not.  Will return true if constructed with a non-null payload
   * or if created with a virtual payload of positive size.
   *
   * @return true if the packet has a physical or virtual payload.
   * @return false if the physical payload is null and the virtual payload is size 0.
   */
  bool HasPayload () const;

  enum MessageType
  {
    Interest,
    ContentObject
  };

  /**
   * Each concrete class implements this to indicate the type of
   * message the base class pointer represents.  One downcasts like this:
   *
   * Ptr<CCNxInterest> interest = DynamicCast<CCNxInterest, CCNxMessage>(m_message);
   */
  virtual enum MessageType GetMessageType (void) const = 0;

  /**
   * Outputs a string like this:
   *
   * { Message ccnx:/name=foo, payloadSize X }
   *
   * Where X is the bytes of payload in the message
   *
   * @param [in] os ostream object
   * @param [in] message CCNxMessage object
   * @return ostream object
   */
  friend std::ostream &operator<< (std::ostream &os, CCNxMessage const &message);

protected:
  Ptr<const CCNxName> m_name;
  Ptr<CCNxBuffer> m_payload;
  size_t m_virtualPayloadLength;
};

}  // namespace ccnx
}  // namespace ns3


#endif //CCNS3_CCNXMESSAGE_H
