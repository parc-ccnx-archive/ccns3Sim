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

#ifndef CCNX_MODEL_ROUTING_NFP_NFP_NEIGHBOR_H_
#define CCNX_MODEL_ROUTING_NFP_NFP_NEIGHBOR_H_

#include <stdint.h>
#include "ns3/nstime.h"
#include "ns3/timer.h"
#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-name.h"
#include "ns3/nfp-neighbor-key.h"
#include "ns3/nfp-computation-cost.h"

namespace ns3 {
namespace ccnx {

/**
 * \ingroup nfp-routing
 *
 * An NfpNeighbor tracks peers in the routing protocol.  A neighbor is identified by
 * a CCNxName, which we assume is unique in the system.  Each neigbor tracks an expiry
 * time for when the record is no longer valid.  The expiry time is extended each time
 * it receives a hello (ReceiveHello).
 *
 * Hello messages are only valid if they are in-order to prior messages, as per normal
 * sequece numbers (see NfpSeqnum).
 *
 * A neighbor keeps its own timer for when it expires.  If it expires, it will call
 * a user-provided callback with the name of the neighbor.  The timer is updated lazily.  That is,
 * we set it once, then receive updates, then the timer expires.  Assuming we've received updates, we'll
 * set the timer to the next timeout based on the last update.  If we've not received any updates,
 * then the timeout was for real.
 *
 * A neighbor can be either Up or Down.  When a hello is first received, the neighbor becomes Up.
 * It stays Up until its timeout expires, at which time it is Down.  In Down, we run another timer.
 * If that timer expires, then the neighbor can be removed.  We do this so we don't forget the Message Seqnum
 * for the neighbor for a very long time to prevent out-of-order packets from restarting the neighbor.
 *
 * State Machine:
 * @code
 *  STATE     EVENT      STATE
 *      INIT ->                                -> Up, update m_expiryTime, StartTimer
 *      Up   -> ReceiveHello                   -> Up, update m_expiryTime
 *      Up   -> TimeoutExpired && !IsExpired() -> Up, StartTimer
 *      Up   -> TimeoutExpired && IsExpired()  -> Down, StartTimer
 *      Down -> ReceiveHello                   -> Up, update m_expiryTime
 *      Down -> TimeoutExpired && !IsExpired() -> Down, StartTimer
 *      Down -> TimeoutExpired && IsExpired()  -> Dead
 *      Dead -> should never have an event (NfpRoutingProtocol should delete us)
 * @endcode
 */
class NfpNeighbor : public SimpleRefCount<NfpNeighbor>
{
public:
  /**
   * Function prototype for the callback to the NfpRoutingProtocol when this neighbor changes its state.
   */
  typedef Callback< void, Ptr<NfpNeighborKey> > StateChangeCallback;

  /**
   * Creates a new record for the neighbor `name`.  The value of `neighborTimeout` will
   * be used to determine if the record has expired -- that is, if we have not received
   * a hello message within the timeout.
   *
   * A Neighbor is identified by the pair (Name, Connection).  Neither can be updated once created.
   * This means that one maintains a table of (Name, Connection) -> NfpNeighbor.
   *
   * When a neighbor is created by this constructor, it is assumed to be UP.  There will
   * not be a stateChangeCallback for the INIT->UP transition.
   *
   * @param name The name of the neighbor
   * @param connection The ingress connection  of the neighbor
   * @param messageSeqnum The initial sequence number for the neighbor
   * @param neighorTimeout The timeout used to compute if the neighbor is expired
   * @param stateChangeCallback Called whenever the state of the neighbor changes with name of neighbor
   */
  NfpNeighbor (Ptr<const CCNxName> neighborName, Ptr<CCNxConnection> connection, uint16_t messageSeqnum,
               Time neighborTimeout, StateChangeCallback stateChangeCallback);

  virtual ~NfpNeighbor ();

  /**
   * Returns the name of the neighbor
   * @return The name
   */
  Ptr<const CCNxName> GetName (void) const;

  /**
   * Determins if `msgSeqnum` is in-order to the record.  See NfpSeqnum.
   *
   * @param msgSeqnum
   * @return true if a message with msgSeqnum is in-order, false otherwise
   */
  bool IsHelloInOrder (uint16_t messageSeqnum) const;

  /**
   * Receives a hello message.
   *
   * @param msgSeqnum The messages sequence number.
   * @return True if the record is updated (i.e. IsHelloInOrder is true), false if not accepted.
   */
  bool ReceiveHello (uint16_t messageSeqnum);

  /**
   * Returns true if the neighbors expiry time is in the past
   *
   * @return if the record has expired, false otherwise
   */
  bool IsExpired (void) const;

  /**
   * Returns the time at which this connection will expire.
   * @return
   */
  Time GetExpiryTime (void) const;

  /**
   * Returns the current message seqnumce number of the neighbor
   * @return Last in-order message sequence number.
   */
  uint16_t GetMessageSeqnum (void) const;

  /**
   * returns the connection id for the neighbor.
   * @return A L3 Connection.
   */
  Ptr<CCNxConnection> GetConnection (void) const;

  /**
   * Determines if our state is UP.
   *
   * An UP neighbor can be a valid successor in our routing table.
   *
   * @return true if up
   * @return false if not up
   */
  bool IsStateUp (void) const;

  /**
   * Determines if our state is DOWN
   *
   * A DOWN neighbor should have its successor entries in the routing table removed.
   * A DOWN neighbor should not be deleted until it is DEAD.  This is to prevent losing
   * state about the message sequence number until we're sure all our advertised routes via
   * that successor are gone.
   *
   * @return true if down
   * @return false if not down
   */
  bool IsStateDown (void) const;

  /**
   * Determines if our state is DEAD
   *
   * A DEAD neighbor should be deleted.  No longer necessary to maintain state.
   *
   * @return true if dead
   * @return false if not dead
   */
  bool IsStateDead (void) const;

  /**
   * Friend the output operator so it can access our internal state
   *
   * @param os The stream to write to
   * @param neighbor The neighbor to dump
   * @return The input ostream
   */
  friend std::ostream & operator << (std::ostream &os, const NfpNeighbor &neighbor);

  /**
   * The data type used for a Neighbor ID
   */
  typedef uint32_t NeighborIdType;

  /**
   * Return this neighbors unique Neighbor ID.
   * @return
   */
  const NeighborIdType GetNeighborId (void) const;

  /**
   * Returns the key associated with this neighbor.
   *
   * Used in the callback to the NfpRoutingProtocol when this neighbor changes.
   * @return
   */
  Ptr<NfpNeighborKey> GetNeighborKey (void) const;

  NfpComputationCost GetComputationCost() const;

protected:
  Ptr<NfpNeighborKey> m_neighborKey;

  /**
   * We use a simple counter for neighbor IDs and assume they will not roll over 4 billion.
   */
  static NeighborIdType m_nextNeighborId;

  /**
   * Get a unique neighbor ID
   * @return a unique neighbor Id
   */
  static NeighborIdType GetUniqueNeighborId (void);

  /**
   * The neighbor ID of this instance
   */
  const NeighborIdType m_neighborId;

  /**
   * The name of the neighbor
   */
  const Ptr<const CCNxName> m_name;

  /**
   * The timeout between UP -> DOWN and DOWN -> DEAD
   */
  Time m_neighborTimeout;

  /**
   * The time at which we make our next timer-based state change.
   */
  Time m_expiry;

  /**
   * The callback to use if the neighbor expires
   */
  StateChangeCallback m_stateChangeCallback;

  typedef enum
  {
    StateUp,
    StateDown,
    StateDead
  } NeighborState;

  NeighborState m_state;

  /**
   * Set for our expiry time, at which point we will call the timeout callback assuming
   * we've not been updated.
   */
  Timer m_timer;

  /**
   * Called by m_timer when it expires
   */
  void TimeoutExpired (void);

  /**
   * Timeout expired while we are in the UP state
   */
  void UpStateTimeout (void);

  /**
   * Timeout expired while we are in the DOWN state
   */
  void DownStateTimeout (void);

  /**
   * The connection of the neighbor
   */
  const Ptr<CCNxConnection> m_connection;

  /**
   * The most recent in-order message seqnum we've received.
   */
  uint16_t m_messageSeqnum;

  /**
   * Computes the expiry time given the current simulation time and m_neighborTimeout.
   * @return
   */
  Time ComputeExpiry (void) const;

  /**
   * Update our expiry timer based on the current expiry time.
   */
  void SetTimer (void);

  /**
   * Log the neighbors state to LOG_LEVEL_DEBUG
   */
  void LogState (void) const;

  NfpComputationCost m_computationCost;
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNX_MODEL_ROUTING_NFP_NFP_NEIGHBOR_H_ */
