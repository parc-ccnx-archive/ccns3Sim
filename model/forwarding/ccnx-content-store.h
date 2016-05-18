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


#ifndef CCNS3SIM_MODEL_FORWARDING_CCNX_CONTENT_STORE_H_
#define CCNS3SIM_MODEL_FORWARDING_CCNX_CONTENT_STORE_H_

#include "ns3/object.h"
#include "ns3/ccnx-packet.h"
#include "ns3/ccnx-forwarder-message.h"
#include "ns3/ccnx-connection-list.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-forwarder
 *
 * Abstract base class for a ContentStore implementation.
 */
class CCNxContentStore : public Object
{
public:
  /**
   * Returns the NS3 runtime type of the class
   * @return The NS3 runtime type
   */
  static TypeId GetTypeId (void);
  CCNxContentStore ();

  /**
   * Virtual destructor for inheritance
   */
  virtual ~CCNxContentStore ();

  /**
   * Try to match an Interest in the content store.
   * This is an asynchronous call to match a content object in the content store.  The
   * result of the match will be returned at a later time via the `MatchInterestCallback()`
   * Example:
   * @code
   * {
   *
   * }
   * @endcode
   *
   * @see MatchInterestCallback
   * *
   * @param interestPacket

   */
  virtual void MatchInterest (Ptr< CCNxForwarderMessage> message) = 0;

  /**
   * The `MatchInterest` is an asynchronous callback from the Content Store after it has processed
   * an Interest sent to `MatchInterest()`.
   *
   * The Content Store should ensure there is always one call to the callback for each call to `MatchInterest()`.
   *
   * @param Ptr<CCNxPacket> [in] The forwarder interest packet
   * @param Ptr<CCNxPacket> [out] The matching content object (may be empty)
   */
  typedef Callback<void, Ptr<CCNxForwarderMessage>  > MatchInterestCallback;

  /**
   * Configure the callback for `MatchInterest()`.  This must be set before using the Content Store.
   *
   * @param matchInterestCallback [in] The callback to use for `MatchInterest()`.
   */
  virtual void SetMatchInterestCallback (MatchInterestCallback matchInterestCallback) = 0;

  /**
    * Try to add a content object to  the content store. This is an asynchronous call.  The
    * result of the add will be returned at a later time via the `AddContentObjectCallback()`
    * Example:
    * @code
    * {
    *
    * }
    * @endcode
    *
    * @see AddContentObjectCallback
    * *
    * @param contentPacket to add
   */
  virtual void AddContentObject (Ptr< CCNxForwarderMessage> message, Ptr<CCNxConnectionList> egressConnections) = 0;

  /**
    * The `AddContentObject` is an asynchronous callback from the Content Store after it has processed
    * a content object sent to `AddContentObject()`.
    *
    * The Content Store should ensure there is always one call to the callback for each call to `AddContentObject()`.
    *
    * @param Ptr<CCNxPacket> [in] The forwarder content object packet
    * @param bool  [out] true if the packet was added
    */
   typedef Callback<void, Ptr< CCNxForwarderMessage>    > AddContentObjectCallback;

   /**
    * Configure the callback for `AddContentObject()`.  This must be set before using the Content Store.
    *
    * @param addContentObjectCallback [in] The callback to use for `AddContentObject()`.
    */
   virtual void SetAddContentObjectCallback (AddContentObjectCallback addContentObjectCallback) = 0;

  /**
   * Removes the given object from the content store
   *
   * This should be the same object (name, keyid, hash) as was added previously, or
   * found via `MatchInterest()`.
   *
   * @param object The content object packet to remove
   * @return true if removed
   * @return false If not removed (i.e. not found)
   */
  virtual bool DeleteContentObject (Ptr<CCNxPacket> cPacket) = 0;

  /**
   * Returns the number of content object packets in the content store.
   *
   * The size of the store is usually configured via a SetAttribute.
   *
   * @return The number of objects in the store.
   */
  virtual size_t GetObjectCount () const = 0;

  /**
   * Returns the maximum capacity (in content objects) of the content store.
   *
   * The size of the store is usually configured via a SetAttribute.
   *
   * @return The object capacity of the content store
   */
  virtual size_t GetObjectCapacity () const = 0;

};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_FORWARDING_CCNX_CONTENT_STORE_H_ */
