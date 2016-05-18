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

#ifndef CCNS3_CCNXSTANDARDPIT_H
#define CCNS3_CCNXSTANDARDPIT_H

#include "ns3/ccnx-pit.h"

#include "ns3/nstime.h"
#include "ccnx-standard-pitEntry.h"
#include "ns3/ccnx-delay-queue.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-standard-forwarder
 *
 * The CCNx standard PIT class is the top level of the pit object hierarchy.
 *
 * The overall structure of the PIT is as follows. In the PIT there are three STL maps:
 *
 * 1. m_tableByName          { Name, PitEntry}
 * 2. m_tableByNameAndKeyId  { (Name, KeyIdRestriction), PitEntry}
 * 3. m_tableByHash          { ContentObjectHashRestriction, PitEntry }
 *
 * When we receive an Interest, it is looked up in the most restrictive table (the above order).  If a
 * PIT entry does not already exist, one is created in that table.
 *
 * When a Content Object arrives, it may match a PIT entry in any of the tables.  Therefore, we need to
 * look it up in all tables.
 *
 * Interest aggregation strategy:
 * - The first Interest for a {name, keyid, hash} is forwarded
 * - A second Interest for a {name, keyid, hash} from a different reverse path may be aggregated
 * - A second Interest for a {name, keyid, hash} from an existing Interest is forwarded
 * - The Interest Lifetime is like a subscription time.  A reverse path entry is removed once the lifetime
 *   is exceeded.
 * - Whan an Interest arrives or is aggregated, the Lifetime for that reverse hop is extended.  As a simplification,
 *   we only keep a single lifetime not per reverse hop.
 *
 * Expiration: When an interest is added, it's expiryTime is set = Now()+InterestTimeout. This interest is discarded
 * when a related interest or contentObject is being processed  if Now > expiryTime.
 *
 * There are three primary methods (see  @see Parent::Method for detailed description):
 *      ReceiveInterest, which processes a new interest and returns a verdict on whether it should be forwarded.
 *	SatisfyInterest, which processes a new content and returns a list of connector ids to which it should be forwarded.
 *	RemoveEntry, which deletes the entire map entry corresponding to a given interest.
 *
  * This class is derived from object with a GetTypeId method to enable run-time replacement.
 *
 * The CCNxStandardPit has a default processing delay of 1 usec + 1 nsec * name_bytes.  For example,
 * an Interest with a 100 byte name has a layer delay of 1.100 usec.  This is a relatively arbitrary
 * formula and arbitrary values for the delay.  It is meant mostly as an example of how one could
 * model delay in the PIT data structure.
 */
class CCNxStandardPit : public CCNxPit
{
public:
  //Map Names to PitEntrys
  typedef std::map < Ptr<const CCNxName>, Ptr<CCNxStandardPitEntry>, CCNxName::isLessPtrCCNxName >  PitByNameType;

  // Map Name and keyid restriction

  typedef std::pair < Ptr<const CCNxName>, Ptr<const CCNxHashValue> > NameAndKeyIdType;

  /**
   * ordering function for operating on a pair < Name, KeyId >.  The total order is defined
   * first on the Name, then on the Keyid.
   */
  struct isLessNameAndKeyIdType
  {
    bool operator() (const NameAndKeyIdType &a, const NameAndKeyIdType &b) const
    {
      bool result = false;

      CCNxName::isLessPtrCCNxName nameless;
      if ( nameless(a.first, b.first) ) {
	  result = true;
      } else if ( !nameless(b.first, a.first) ) {
	  // if b.name is not less than a.name, then the names must be equal

	  CCNxHashValue::isLessPtrCCNxHashValue hashless;
	  if ( hashless(a.second, b.second) ) {
	      // if a's keyid is less than b's keyid
	      result = true;
	  }
      }

      return result;
    }
  };

  typedef std::map < NameAndKeyIdType,  Ptr<CCNxStandardPitEntry>, isLessNameAndKeyIdType > PitByNameAndKeyIdType;

  //Map Hash values to PitEntrys
  typedef std::map < Ptr<const CCNxHashValue>, Ptr<CCNxStandardPitEntry>, CCNxHashValue::isLessPtrCCNxHashValue > PitByHashType;

  /**
   *  Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  CCNxStandardPit ();
  virtual ~CCNxStandardPit ();

  virtual void ReceiveInterest (Ptr<CCNxForwarderMessage> message);

  virtual void SetReceiveInterestCallback (ReceiveInterestCallback receiveInterestCallback);

  virtual void SatisfyInterest (Ptr<CCNxForwarderMessage> message);

  virtual void SetSatisfyInterestCallback (SatisfyInterestCallback satisfyInterestCallback);

  virtual void RemoveEntry (Ptr<CCNxInterest> interest);

  int CountEntries ();

  virtual void Print (std::ostream &os) const;

  friend std::ostream &operator<< (std::ostream& os, Ptr<CCNxStandardPit> ccnxStandardPit);

protected:
  // ns3::Object::DoInitialize()
  virtual void DoInitialize ();


//  template  <typename MapType, typename MapKey, typename MapTypeIt > CCNxPit::Verdict GetVerdict(MapType a, MapKey b, Ptr<CCNxInterest> interest, Ptr<CCNxConnection> ingress);

  //internal method to clean the list of connections returned from SatisfyInterest
  Ptr<CCNxConnectionList>  RemoveDuplicates (Ptr<CCNxConnectionList> a, Ptr<CCNxConnectionList> b, Ptr<CCNxConnection> ingress);

private:

  /**
   * Create the key type to use in the ByNameAndKeyIdType std::map.
   *
   * Precondition: The interest has both a name and a KeyIdRestriction
   *
   * @param name [in] name part of the key
   * @param keyid [in] The keyid part of the key
   * @return The key to use in the map
   */
  NameAndKeyIdType CreateNameAndKeyIdKey(Ptr<const CCNxName> name, Ptr<CCNxHashValue> keyId);

  /**
   * Given an Interest packet, calculate it's expiry time.
   *
   * If the Interest has a lifetime, the expiry time is Now() + interestLifetime.  otherwise,
   * the expiry time is Now() + _defaultLifetime.
   *
   * @param interestPacket [in] The interest packet
   * @return The time at which the lifetime expires
   */
  Time CalculateInterestExpiryTime(Ptr<CCNxPacket> interestPacket);

  /**
   * Find the PIT entry that corresponds to the Interest.  It will match against the most
   * restrictive table (e.g. by hash, by keyid & name, by name).
   *
   * @param interest [in] The Interest to lookup
   * @return Ptr<CCNxStandardPitEntry>(0) if not found (NULL)
   * @return Non-null if matched
   */
  Ptr<CCNxStandardPitEntry> LookupPitEntry(Ptr<CCNxInterest> interest);

  /**
   * Find the PIT entry in the hash table.
   *
   * @param interest [in] The Interest to lookup
   * @return Ptr<CCNxStandardPitEntry>(0) if not found (NULL)
   * @return Non-null if matched
   */
  Ptr<CCNxStandardPitEntry> LookupPitEntryByHash(Ptr<CCNxHashValue> hashRestriction);

  /**
   * Find the PIT entry in the name & keyid table.
   *
   * @param interest [in] The Interest to lookup
   * @return Ptr<CCNxStandardPitEntry>(0) if not found (NULL)
   * @return Non-null if matched
   */
  Ptr<CCNxStandardPitEntry> LookupPitEntryByNameAndKeyId(Ptr<const CCNxName> name, Ptr<CCNxHashValue> keyId);

  /**
   * Find the PIT entry in the name table.
   *
   * @param interest [in] The Interest to lookup
   * @return Ptr<CCNxStandardPitEntry>(0) if not found (NULL)
   * @return Non-null if matched
   */
  Ptr<CCNxStandardPitEntry> LookupPitEntryByName(Ptr<const CCNxName> name);

  /**
   * Create a new PitEntry and save it in the appropriate tables
   *
   * @param interest [in] The interest being processed
   * @return non-null The new pitEntry, which has been saved in the appropriate tables
   */
  Ptr<CCNxStandardPitEntry> AddPitEntry(Ptr<CCNxInterest> interest);

  /**
   * Create a pitEntry in the ByHash table
   *
   * @param interest [in] The interest being processed
   * @return non-null The new pitEntry saved in the Hash table
   */
  Ptr<CCNxStandardPitEntry> AddPitEntryByHash(Ptr<CCNxInterest> interest);

  /**
   * Create a pitEntry in the ByName table
   *
   * @param interest [in] The interest being processed
   * @return non-null The new pitEntry, which has been saved in the Name table
   */
  Ptr<CCNxStandardPitEntry> AddPitEntryByName(Ptr<CCNxInterest> interest);

  /**
   * Create a pitEntry in the ByNameAndKeyId table
   *
   * @param interest [in] The interest being processed
   * @return non-null The new pitEntry, which has been saved in the Name table
   */
  Ptr<CCNxStandardPitEntry> AddPitEntryByNameAndKeyId(Ptr<CCNxInterest> interest);

  // === REMOVE

  /**
   * Remove the PIT entry in the hash table.
   *
   * @param hashRestriction [in] The Interest to remove
   */
  void RemovePitEntryByHash(Ptr<CCNxHashValue> hashRestriction);

  /**
   * Remove the PIT entry in the name and keyid table.
   *
   * @param name [in] The Interest name to remove
   * @param keyId [in] The Interest keyId to remove
   */
  void RemovePitEntryByNameAndKeyId(Ptr<const CCNxName> name, Ptr<CCNxHashValue> keyId);

  /**
   * Remove the PIT entry in the name table
   *
   * @param name [in] The Interest name to remove
   */
  void RemovePitEntryByName(Ptr<const CCNxName> name);

  // ===

  /**
   * After input delay, service a ReceiveInterest request and send the
   * result to the ReceiveInterestCallback.
   *
   * @param item [in] The work item being serviced
   */
  void ServiceReceiveInterest (Ptr<CCNxForwarderMessage> item);

  /**
   * After input delay, service a SatisfyInterest request and send the
   * result to the SatisfyInterestCallback.
   *
   * @param item [in] The work item being serviced
   */
  void ServiceSatisfyInterest (Ptr<CCNxForwarderMessage> item);


  PitByNameType m_tableByName;
  PitByNameAndKeyIdType m_tableByNameAndKeyId;
  PitByHashType m_tableByHash;

  /*
   * The default interest lifetime - how long an interest will be valid after arrival at a node.
   * A longer lifetime raises likelihood of aggregation, while a shorter one raises likelihood of content arriving too late and being dropped.
   */
  Time m_defaultLifetime;

  ReceiveInterestCallback m_receiveInterestCallback;
  SatisfyInterestCallback m_satisfyInterestCallback;

  /**
   * The storage type of the CCNxDelayQueue
   */
  typedef CCNxDelayQueue<CCNxForwarderMessage> DelayQueueType;

  /**
   * Input queue used to simulate processing delay
   */
  Ptr<DelayQueueType> m_inputQueue;

  /**
   * Callback from delay queue to compute the service time of a work item
   *
   * @param item [in] The work item being serviced
   * @return The service time of the work item
   */
  Time GetServiceTime (Ptr<CCNxForwarderMessage> item);

  /**
   * Callback from delay queue after a work item has waited its service time
   *
   * @param item [in] The work item to service
   */
  void ServiceInputQueue (Ptr<CCNxForwarderMessage> item);

  /**
   * The layer delay is:
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * nameBytes\f$
   *
   * It is set via the attribute "LayerDelayConstant".  The default is 1 usec.
   */
  Time m_layerDelayConstant;

  /**
   * The layer delay is:
   *
   * \f$time = m\_layerDelayConstant + m\_layerDelaySlope * nameBytes\f$
   *
   * It is set via the attribute "LayerDelaySlope".  The default is 0.
   */
  Time m_layerDelaySlope;

  /**
   * The number of parallel servers processing the input delay queue.
   * The more servers there are, the lower the amortized delay is.
   *
   * This value is set via the attribute "LayerDelayServers".  The default is 1.
   */
  unsigned m_layerDelayServers;
};

}
}




#endif //CCNS3_CCNXPIT_H
