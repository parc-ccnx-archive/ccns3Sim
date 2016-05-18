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

#include <iomanip>
#include "nfp-stats.h"
#include "ns3/nstime.h"
#include "ns3/log.h"

using namespace ns3;
using namespace ns3::ccnx;

NfpStats::NfpStats ()
  : m_nodeId(0),
    m_payloadsSent (0), m_bytesSent (0), m_payloadsReceived (0), m_bytesReceived (0), m_hellosSent(0),
    m_advertiseOriginated (0), m_advertiseSent (0), m_advertiseReceived (0), m_advertiseReceivedFeasible (0),
    m_withdrawOriginated(0), m_withdrawSent (0), m_withdrawReceived (0)
{
}

NfpStats::NfpStats(const NfpStats &copy)
  : m_nodeId(copy.m_nodeId),
    m_payloadsSent (copy.m_payloadsSent), m_bytesSent (copy.m_bytesSent), m_payloadsReceived (copy.m_payloadsReceived),
    m_bytesReceived (copy.m_bytesReceived), m_hellosSent (copy.m_hellosSent),
    m_advertiseOriginated (copy.m_advertiseOriginated), m_advertiseSent (copy.m_advertiseSent), m_advertiseReceived (copy.m_advertiseReceived),
    m_advertiseReceivedFeasible (copy.m_advertiseReceivedFeasible),
    m_withdrawOriginated(copy.m_withdrawOriginated), m_withdrawSent (copy.m_withdrawSent), m_withdrawReceived (copy.m_withdrawReceived)
{

}

void
NfpStats::SetNodeId(uint32_t nodeId)
{
  m_nodeId = nodeId;
}

NfpStats &
NfpStats::operator += (const NfpStats &other)
{
  m_payloadsSent += other.m_payloadsSent;
  m_bytesSent += other.m_bytesSent;
  m_payloadsReceived += other.m_payloadsReceived;
  m_bytesReceived += other.m_bytesReceived;
  m_hellosSent += other.m_hellosSent;

  m_advertiseOriginated += other.m_advertiseOriginated;
  m_advertiseSent += other.m_advertiseSent;
  m_advertiseReceived += other.m_advertiseReceived;
  m_advertiseReceivedFeasible += other.m_advertiseReceivedFeasible;

  m_withdrawOriginated+= other.m_withdrawOriginated;
  m_withdrawSent += other.m_withdrawSent;
  m_withdrawReceived += other.m_withdrawReceived;
  return *this;
}

void NfpStats::IncrementPayloadsSent()
{
  m_payloadsSent++;
}

void NfpStats::IncrementBytesSent(uint64_t value)
{
  m_bytesSent += value;
}

void NfpStats::IncrementPayloadsReceived()
{
  m_payloadsReceived++;
}

void NfpStats::IncrementBytesReceived(uint64_t value)
{
  m_bytesReceived += value;
}


void NfpStats::IncrementAdvertiseOriginated()
{
  m_advertiseOriginated++;
}

void NfpStats::IncrementAdvertiseSent()
{
  m_advertiseSent++;
}

void NfpStats::IncrementAdvertiseReceived()
{
  m_advertiseReceived++;
}

void NfpStats::IncrementAdvertiseReceivedFeasible()
{
  m_advertiseReceivedFeasible++;
}


void NfpStats::IncrementWithdrawOriginated()
{
  m_withdrawOriginated++;
}

void NfpStats::IncrementWithdrawSent()
{
  m_withdrawSent++;
}

void NfpStats::IncrementWithdrawReceived()
{
  m_withdrawReceived++;
}


uint64_t NfpStats::GetPayloadsSent() const
{
  return m_payloadsSent;
}

uint64_t NfpStats::GetBytesSent() const
{
  return m_bytesSent;
}

uint64_t NfpStats::GetPayloadsReceived() const
{
  return m_payloadsReceived;
}

uint64_t NfpStats::GetBytesReceived() const
{
  return m_bytesReceived;
}


uint64_t NfpStats::GetAdvertiseOriginated() const
{
  return m_advertiseOriginated;
}

uint64_t NfpStats::GetAdvertiseSent() const
{
  return m_advertiseSent;
}

uint64_t NfpStats::GetAdvertiseReceived() const
{
  return m_advertiseReceived;
}

uint64_t NfpStats::GetAdvertiseReceivedFeasible() const
{
  return m_advertiseReceivedFeasible;
}


uint64_t NfpStats::GetWithdrawOriginated() const
{
  return m_withdrawOriginated;
}

uint64_t NfpStats::GetWithdrawSent() const
{
  return m_withdrawSent;
}

uint64_t NfpStats::GetWithdrawReceived() const
{
  return m_withdrawReceived;
}

void NfpStats::IncrementHellosSent()
{
  m_hellosSent++;
}

uint64_t
NfpStats::GetHellosSent() const
{
  return m_hellosSent;
}

std::ostream &
ns3::ccnx::operator << (std::ostream &os, const NfpStats &stats)
{
  ns3::LogTimePrinter timePrinter = ns3::LogGetTimePrinter ();
  (*timePrinter)(os);
  os << std::setw (5) << stats.m_nodeId;
  os << " Routing stats";
  os << " payloads recv " << stats.m_payloadsReceived << " sent " << stats.m_payloadsSent;
  os << " bytes recv " << stats.m_bytesReceived << " sent " << stats.m_bytesSent;
  os << " hellos sent " << stats.m_hellosSent << std::endl;

  (*timePrinter)(os);
  os << std::setw (5) << stats.m_nodeId;
  os << " Routing stats";
  os << " advertise orig " << stats.m_advertiseOriginated;
  os << " recv " << stats.m_advertiseReceived;
  os << " sent " << stats.m_advertiseSent;
  os << " feasible " << stats.m_advertiseReceivedFeasible << std::endl;

  (*timePrinter)(os);
  os << std::setw (5) << stats.m_nodeId;
  os << " Routing stats";
  os << " withdraw orig " << stats.m_withdrawOriginated;
  os << " recv " << stats.m_withdrawReceived;
  os << " sent " << stats.m_withdrawSent << std::endl;

  return os;
}

