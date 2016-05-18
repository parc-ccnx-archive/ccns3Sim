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


#ifndef CCNS3_CCNXINTEREST_H
#define CCNS3_CCNXINTEREST_H

#include <stdint.h>
#include "ns3/buffer.h"
#include "ns3/ccnx-message.h"
#include "ns3/ccnx-hash-value.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-messages
 *
 * Class representation of an Interest message.
 *
 * The KeyIdRestriction and HashRestriction are both represented as CCNxHashValue.  Use the value "0"
 * for no restriction.  They will be serialized to 32 bytes for size correctness?, if they are not 0.
 *
 * The CCNxValidation carries a CCNxHashValue uint32_t keyid.
 *
 * The CCNxPacket carries a virtual hash which is carried as a PacketTag, so it is not
 * actually serialized as part of the payload.
 */
class CCNxInterest : public CCNxMessage
{
public:
  static TypeId GetTypeId (void);

  /**
   * Create an Interest.
   *
   * @param payload     The payload in the Interest.
   * @param keyIdRest   The KeyIdRestriction.
   * @param hashRest    The ObjectHashRestriction.
   *
   * Example
   * @code
   * Ptr<CCNxInterest> interest = Create<CCNxInterest>(name, payload, (Ptr<CCNxHashValue>) 0, (Ptr<CCNxHashValue>) 0);
   * @endcode
   */
  CCNxInterest (Ptr<const CCNxName> name, Ptr<CCNxBuffer> payload, Ptr<CCNxHashValue> keyIdRest, Ptr<CCNxHashValue> hashRest);

  /**
   * Create an Interest without a KeyId or Hash restriction.
   */
  CCNxInterest (Ptr<const CCNxName> name, Ptr<CCNxBuffer> payload);

  /**
   * Create an Interest with name only.
   */
  CCNxInterest (Ptr<const CCNxName> name);

  /**
   * Returns the KeyId Restriction associated with this Interest.
   *
   * If the KeyId Restriction is not set, the Ptr will evaluate to false (i.e. operator !()).
   */
  Ptr<CCNxHashValue> GetKeyidRestriction () const;

  /**
   * Determines if there is a KeyId Restriction.
   * @return true if the keyid restriction is not null (Ptr<CCNxHashValue>(0)).
   * @return true if the keyid restriction is null.
   */
  bool HasKeyidRestriction () const;

  /**
   * Returns the ObjectHash Restriction associated with this Interest.
   *
   * If the ObjectHash Restriction is not set, the Ptr will evaluate to false (i.e. operator !()).
   */
  Ptr<CCNxHashValue> GetHashRestriction () const;

  /**
   * Determines if there is a Hash Restriction.
   * @return true if the Hash restriction is not null (Ptr<CCNxHashValue>(0)).
   * @return true if the Hash restriction is null.
   */
  bool HasHashRestriction () const;


  /**
   * Determines if the given Interest is equivalent to this Interest.
   *
   * Two interests are equivalent if the tuples {name, keyid restriction, hash restriction}
   * are exactly equal.
   */
  bool Equals (const Ptr<CCNxInterest> other) const;

  /**
   * Determines if the given Interest is equivalent to this Interest.
   *
   * Two interests are equivalent if the tuples {name, keyid restriction, hash restriction}
   * are exactly equal.
   */
  bool Equals (CCNxInterest const &other) const;

  /**
   * Inherited from CCNxMessage.
   *
   * Returns CCNxMessage::MessageType::Interest.
   */
  virtual enum MessageType GetMessageType (void) const;

  /**
   * Outputs a string like this:
   *
   * { Interest ccnx:/name=foo, payloadSize X, KeyId Y, Hash Z }
   *
   * Where X is the bytes of payload in the interest, Y is the uint64_t value
   * of the CCNxHashValue, and Z is the uint64_t value of the CCNxHashValue.
   * If there is no Y or Z in the Interest (they are Ptr<CCNxHashValue>(0)), then
   * the value 0 is used.
   *
   * @param [in] os ostream object
   * @param [in] interest CCNxInterest object
   * @return ostream object
   */
  friend std::ostream &operator<< (std::ostream &os, CCNxInterest const &interest);

protected:
  Ptr<CCNxHashValue> m_keyidRestriction;
  Ptr<CCNxHashValue> m_hashRestriction;
};

} // namespace ccnx
} // namespace ns3

#endif //CCNS3_CCNXINTEREST_H
