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

#include "ccnx-routing-helper.h"
#include "ns3/node-list.h"
#include "ns3/simulator.h"

using namespace ns3;
using namespace ns3::ccnx;

CCNxRoutingHelper::~CCNxRoutingHelper ()
{
  // empty
}


void
CCNxRoutingHelper::PrintRoutingTableAllNodesWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream)
{
  for (int i = 0; i < NodeList::GetNNodes (); ++i)
    {
      PrintRoutingTableWithDelay (printDelay, stream, NodeList::GetNode (i));
    }
}

void
CCNxRoutingHelper::PrintRoutingTableAllNodesWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream)
{
  for (int i = 0; i < NodeList::GetNNodes (); ++i)
    {
      PrintRoutingTableWithInterval (printInterval, stream, NodeList::GetNode (i));
    }
}

void
CCNxRoutingHelper::PrintRoutingTableWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream, Ptr<Node> node)
{
  Simulator::Schedule (printDelay, &CCNxRoutingHelper::PrintRoutingTable, stream, node);
}

void
CCNxRoutingHelper::PrintRoutingTableWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream, Ptr<Node> node)
{
  Simulator::Schedule (printInterval, &CCNxRoutingHelper::PrintRoutingTableInterval, stream, node, printInterval);
}

void
CCNxRoutingHelper::PrintNeighborTableAllNodesWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream)
{
  for (int i = 0; i < NodeList::GetNNodes (); ++i)
    {
      PrintNeighborTableWithDelay (printDelay, stream, NodeList::GetNode (i));
    }
}

void
CCNxRoutingHelper::PrintNeighborTableAllNodesWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream)
{
  for (int i = 0; i < NodeList::GetNNodes (); ++i)
    {
      PrintNeighborTableWithInterval (printInterval, stream, NodeList::GetNode (i));
    }
}

void
CCNxRoutingHelper::PrintNeighborTableWithDelay (Time printDelay, Ptr<OutputStreamWrapper> stream, Ptr<Node> node)
{
  Simulator::Schedule (printDelay, &CCNxRoutingHelper::PrintNeighborTable, stream, node);
}

void
CCNxRoutingHelper::PrintNeighborTableWithInterval (Time printInterval, Ptr<OutputStreamWrapper> stream, Ptr<Node> node)
{
  Simulator::Schedule (printInterval, &CCNxRoutingHelper::PrintNeighborTableInterval, stream, node, printInterval);
}

// == The methods that actually do the work

void
CCNxRoutingHelper::PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Ptr<Node> node)
{
  // This will fetch whatever is bound to the node as a derived class of CCNxRoutingProtocol.
  Ptr<CCNxRoutingProtocol> routing = node->GetObject<CCNxRoutingProtocol> ();
  if (routing)
    {
      routing->PrintRoutingTable (stream);
    }
  else
    {
      *stream->GetStream () << "No CCNxRoutingProtocol bound to node " << node->GetId () << std::endl;
    }
}

void
CCNxRoutingHelper::PrintRoutingTableInterval (Ptr<OutputStreamWrapper> stream, Ptr<Node> node, Time printInterval)
{
  PrintRoutingTable (stream, node);
  Simulator::Schedule (printInterval, &CCNxRoutingHelper::PrintRoutingTableInterval, stream, node, printInterval);
}

void
CCNxRoutingHelper::PrintNeighborTable (Ptr<OutputStreamWrapper> stream, Ptr<Node> node)
{
  // This will fetch whatever is bound to the node as a derived class of CCNxRoutingProtocol.
  Ptr<CCNxRoutingProtocol> routing = node->GetObject<CCNxRoutingProtocol> ();
  if (routing)
    {
      routing->PrintNeighborTable (stream);
    }
  else
    {
      *stream->GetStream () << "No CCNxRoutingProtocol bound to node " << node->GetId () << std::endl;
    }
}

void
CCNxRoutingHelper::PrintNeighborTableInterval (Ptr<OutputStreamWrapper> stream, Ptr<Node> node, Time printInterval)
{
  PrintNeighborTable (stream, node);
  Simulator::Schedule (printInterval, &CCNxRoutingHelper::PrintNeighborTableInterval, stream, node, printInterval);
}
