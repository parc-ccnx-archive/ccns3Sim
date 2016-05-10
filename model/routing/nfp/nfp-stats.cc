/*
 * nfp-stats.cc
 *
 *  Created on: May 10, 2016
 *      Author: mmosko
 */

#include "nfp-stats.h"

using namespace ns3;
using namespace ns3::ccnx;

NfpStats::NfpStats (uint32_t nodeId)
  : m_nodeId(nodeId),
    m_payloadsSent (0), m_bytesSent (0), m_payloadsReceived (0), m_bytesReceived (0),
    m_advertiseOriginated (0), m_advertiseSent (0), m_advertiseReceived (0), m_advertiseReceivedFeasible (0),
    m_withdrawOriginated(0), m_withdrawSent (0), m_withdrawReceived (0)
{
}

NfpStats::NfpStats(const NfpStats &copy)
  : m_nodeId(copy.m_nodeId),
    m_payloadsSent (copy.m_payloadsSent), m_bytesSent (copy.m_bytesSent), m_payloadsReceived (copy.m_payloadsReceived),
    m_bytesReceived (copy.m_bytesReceived),
    m_advertiseOriginated (copy.m_advertiseOriginated), m_advertiseSent (copy.m_advertiseSent), m_advertiseReceived (copy.m_advertiseReceived),
    m_advertiseReceivedFeasible (copy.m_advertiseReceivedFeasible),
    m_withdrawOriginated(copy.m_withdrawOriginated), m_withdrawSent (copy.m_withdrawSent), m_withdrawReceived (copy.m_withdrawReceived)
{

}

NfpStats &
NfpStats::operator += (const NfpStats &other)
{
  m_payloadsSent += other.m_payloadsSent;
  m_bytesSent += other.m_bytesSent;
  m_payloadsReceived += other.m_payloadsReceived;
  m_bytesReceived += other.m_bytesReceived;

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


void NfpStats::IncrementAdvertiseOriginiated()
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


uint64_t NfpStats::GetAdvertiseOriginiated() const
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


std::ostream &
ns3::ccnx::operator << (std::ostream &os, const NfpStats &stats)
{
  ns3::LogTimePrinter timePrinter = ns3::LogGetTimePrinter ();
  (*timePrinter)(os);
  os << std::setw (5) << stats.m_nodeId;
  os << " Routing stats";
  os << " payloads recv " << stats.m_payloadsReceived << " sent " << stats.m_payloadsSent;
  os << " bytes recv " << stats.m_bytesReceived << " sent " << stats.m_bytesSent << std::endl;

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

