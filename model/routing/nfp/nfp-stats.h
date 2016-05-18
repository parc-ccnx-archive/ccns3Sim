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

#ifndef CCNS3SIM_MODEL_ROUTING_NFP_NFP_STATS_H_
#define CCNS3SIM_MODEL_ROUTING_NFP_NFP_STATS_H_

#include <ostream>
#include <stdint.h>

namespace ns3
{
  namespace ccnx
  {

    class NfpStats
    {
    public:
      NfpStats ();

      NfpStats(const NfpStats &copy);

      NfpStats & operator += (const NfpStats &other);

      void SetNodeId(uint32_t nodeId);

      void IncrementPayloadsSent();
      void IncrementBytesSent(uint64_t value);
      void IncrementPayloadsReceived();
      void IncrementBytesReceived(uint64_t value);

      void IncrementHellosSent();
      uint64_t GetHellosSent() const;

      void IncrementAdvertiseOriginated();
      void IncrementAdvertiseSent();
      void IncrementAdvertiseReceived();
      void IncrementAdvertiseReceivedFeasible();

      void IncrementWithdrawOriginated();
      void IncrementWithdrawSent();
      void IncrementWithdrawReceived();

      uint64_t GetPayloadsSent() const;
      uint64_t GetBytesSent() const;
      uint64_t GetPayloadsReceived() const;
      uint64_t GetBytesReceived() const;

      uint64_t GetAdvertiseOriginated() const;
      uint64_t GetAdvertiseSent() const;
      uint64_t GetAdvertiseReceived() const;
      uint64_t GetAdvertiseReceivedFeasible() const;

      uint64_t GetWithdrawOriginated() const;
      uint64_t GetWithdrawSent() const;
      uint64_t GetWithdrawReceived() const;

      friend std::ostream & operator << (std::ostream &os, const NfpStats &stats);

    protected:
      uint32_t 	  m_nodeId;
      uint64_t    m_payloadsSent;           //<! number of NfpPayloads sent (all interfaces)
      uint64_t    m_bytesSent;              //<! total bytes of NfpPayload sent (over all interfaces)

      uint64_t    m_payloadsReceived;               //<! Number of NfpPayloads received
      uint64_t    m_bytesReceived;                  //<! Total bytes of NfpPayloads received

      uint64_t	  m_hellosSent;			    //<! Number of empty messages sent as hellos
      uint64_t    m_advertiseOriginated;            //<! Advertisements we originated
      uint64_t    m_advertiseSent;                  //<! Advertisements we sent (includes Originated)
      uint64_t    m_advertiseReceived;              //<! Advertisements we received
      uint64_t    m_advertiseReceivedFeasible;              //<! Advertisements we received and were feasible

      uint64_t    m_withdrawOriginated;            //<! Withdraws we originated
      uint64_t    m_withdrawSent;
      uint64_t    m_withdrawReceived;
      };


  } /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_ROUTING_NFP_NFP_STATS_H_ */
