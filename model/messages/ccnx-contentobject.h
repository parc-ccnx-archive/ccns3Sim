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

#ifndef CCNS3_CCNXCONTENTOBJECT_H
#define CCNS3_CCNXCONTENTOBJECT_H

#include "ns3/buffer.h"
#include "ns3/ccnx-message.h"
#include "ns3/ccnx-time.h"

namespace ns3 {
namespace ccnx {

typedef enum
{
  CCNxContentObjectPayloadType_Data,
  CCNxContentObjectPayloadType_Key,
  CCNxContentObjectPayloadType_Link,
  CCNxContentObjectPayloadType_Manifest,
} CCNxContentObjectPayloadType;

/**
 * @ingroup ccnx-messages
 *
 * Class representation of a ContentObject.  The payload is not actually serialized.  If you need
 * to actually carry data bytes... not there yet.
 *
 * The keyid is part of the Validation.
 */
class CCNxContentObject : public CCNxMessage
{
public:
  static TypeId GetTypeId (void);

  /**
   * Create a Content Object.
   *
   * @param payload     The payload in the Content Object.
   * @param payloadType The payloadType of the Content Object.
   * @param expiryTime  The expire time is the time at which the payload expires.
   *
   * Example
   * @code
   * Ptr<CCNxContentObject> contentObj = Create<CCNxInterest>(name, payload, 0,
   *                                     (Ptr<CCNxTime>) 0);
   * @endcode
   */
  CCNxContentObject (Ptr<const CCNxName> name, Ptr<CCNxBuffer> payload, CCNxContentObjectPayloadType payloadType, Ptr<CCNxTime> expiryTime);

  /**
   * Create a Content Object without payloadType and expiryTime
   */
  CCNxContentObject (Ptr<const CCNxName> name, Ptr<CCNxBuffer> payload);

  /**
   * Create a Content Object with name only.
   */
  CCNxContentObject (Ptr<const CCNxName> name);

  /**
   * Returns the expiry time associated with the payload of this Content Object.
   *
   * If the expiry time is not set, the Ptr will evaluate to 0 (i.e. operator !()).
   */
  Ptr<CCNxTime>   GetExpiryTime () const;

  /**
   * Returns the PayloadType associated with this Content Object.
   *
   * The default value of PayloadType is 0 (CCNxContentObjectPayloadType_Data)
   */
  CCNxContentObjectPayloadType GetPayloadType () const;

  /**
   * Determines if the given Content Object is equivalent to this Content Object.
   *
   * Two Content Objects are equivalent if the tuples {name, payload, expiry time}
   * are exactly equal.
   */
  bool Equals (const Ptr<CCNxContentObject> other) const;

  /**
   * Determines if the given Content Object is equivalent to this Content Object.
   *
   * Two Content Objects are equivalent if the tuples {name, payload, expiry time}
   * are exactly equal.
   */
  bool Equals (CCNxContentObject const &other) const;

  /**
  * Inherited from CCNxMessage.
  *
  * Returns CCNxMessage::MessageType::ContentObject.
  */
  virtual enum MessageType GetMessageType (void) const;

  /**
   * Outputs a string like this:
   *
   * { Content Object ccnx:/name=foo, payloadSize X, payloadType Y, expiryTime Z }
   *
   * Where X is the bytes of payload in the content object, Y is the payload type
   * CCNxContentObjectPayloadType, and Z is the uint64_t value of the CCNxTime.
   * If there is no Y or Z in the Content Object, then the value 0 is used
   *
   * @param [in] os ostream object
   * @param [in] content CCNxContentObject object
   * @return ostream object
   */
  friend std::ostream &operator<< (std::ostream &os, CCNxContentObject const &content);

protected:
  CCNxContentObjectPayloadType m_payloadType;
  Ptr<CCNxTime>   m_expiryTime;
};

}
}

#endif //CCNS3_CCNXCONTENTOBJECT_H
