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

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/ccnx-route.h"

using namespace ns3;
using namespace ns3::ccnx;

CCNxRoute::CCNxRoute (Ptr<const CCNxName> prefix) : m_prefix (prefix)
{
  // empty
}

// This will copy the vector to a new vector. The Ptr<> entries will be copied (thus new refcount).
CCNxRoute::CCNxRoute (const CCNxRoute &copy) : m_prefix (copy.m_prefix), m_connectionsMap (copy.m_connectionsMap)
{
  // empty
}

CCNxRoute::~CCNxRoute ()
{
  // empty
}

void
CCNxRoute::AddNexthop (Ptr<const CCNxRouteEntry> nexthop)
{
  NS_ASSERT_MSG (m_prefix->Equals (*nexthop->GetPrefix ()), "Trying to add a nexthop with wrong prefix");


  // If the connection id is already in my database, only keep the one
  // with the minimum cost.

  ConnectionMapType::iterator i = m_connectionsMap.find (nexthop->GetConnection ());
  if (i != m_connectionsMap.end ())
    {
      if (i->second->GetCost () > nexthop->GetCost ())
        {
          i->second = nexthop;
        }
    }
  else
    {
      m_connectionsMap[nexthop->GetConnection ()] = nexthop;
    }
}

const CCNxRoute::NexthopListType
CCNxRoute::GetNexthops (void) const
{
  NexthopListType nexthops;
  for (ConnectionMapType::const_iterator i = m_connectionsMap.cbegin (); i != m_connectionsMap.cend (); ++i)
    {
      nexthops.push_back (i->second);
    }

  return nexthops;
}

void
CCNxRoute::AddRoute (Ptr<const CCNxRoute> otherRoute)
{
  // Add each entry, keeping the one with minimum distance
  for (ConnectionMapType::const_iterator i = otherRoute->m_connectionsMap.cbegin (); i != otherRoute->m_connectionsMap.cend (); ++i)
    {
      AddNexthop (i->second);
    }
}

size_t
CCNxRoute::RemoveNexthop (Ptr<CCNxConnection> connection)
{
  return m_connectionsMap.erase (connection);
}

bool
CCNxRoute::Equals (const CCNxRoute &other) const
{
  bool result = false;
  if (size () == other.size () && m_prefix->Equals (*other.m_prefix))
    {
      result = true;
      for (ConnectionMapType::const_iterator i = m_connectionsMap.begin (); i != m_connectionsMap.end (); ++i)
        {
          const Ptr<CCNxConnection> & connection = i->first;
          Ptr<const CCNxRouteEntry> entry = i->second;

          ConnectionMapType::const_iterator j = other.m_connectionsMap.find (connection);
          if (j == other.m_connectionsMap.cend ())
            {
              result = false;
              break;
            }
          else
            {
              if (!entry->Equals (*j->second))
                {
                  result = false;
                  break;
                }
            }
        }
    }
  return result;
}

// ==========
// STL Container style interface

void
CCNxRoute::clear (void)
{
  m_connectionsMap.clear ();
}

size_t
CCNxRoute::size (void) const
{
  return m_connectionsMap.size ();
}

bool
CCNxRoute::empty () const
{
  return m_connectionsMap.empty ();
}

CCNxRoute::const_iterator
CCNxRoute::begin () const
{
  return CCNxRoute::const_iterator (m_connectionsMap.begin ());
}

CCNxRoute::const_iterator
CCNxRoute::cbegin () const
{
  return CCNxRoute::const_iterator (m_connectionsMap.cbegin ());
}

CCNxRoute::const_iterator
CCNxRoute::end () const
{
  return CCNxRoute::const_iterator (m_connectionsMap.end ());
}

CCNxRoute::const_iterator
CCNxRoute::cend () const
{
  return CCNxRoute::const_iterator (m_connectionsMap.cend ());
}

CCNxRoute::const_iterator::const_iterator (CCNxRoute::ConnectionMapType::const_iterator iter) : m_iter (iter)
{
  // empty
}

CCNxRoute::const_iterator::const_iterator (const CCNxRoute::const_iterator& other) : m_iter (other.m_iter)
{
  // empty
}

CCNxRoute::const_iterator::~const_iterator ()
{
  // empty
}

CCNxRoute::const_iterator &
CCNxRoute::const_iterator::operator = (const const_iterator &other)
{
  m_iter = other.m_iter;
  return *this;
}

bool
CCNxRoute::const_iterator::operator == (const const_iterator & other) const
{
  return m_iter == other.m_iter;
}

bool
CCNxRoute::const_iterator::operator != (const const_iterator & other) const
{
  return m_iter != other.m_iter;
}

CCNxRoute::const_iterator &
CCNxRoute::const_iterator::operator++ ()
{
  m_iter++;
  return *this;
}

CCNxRoute::const_iterator::const_reference
CCNxRoute::const_iterator::operator* () const
{
  // We want to only return the Ptr<CCNxRoute>, so we look at the second part of m_iter
  return m_iter->second;
}

CCNxRoute::const_iterator::const_pointer
CCNxRoute::const_iterator::operator-> () const
{
  return m_iter->second;
}

std::ostream &
ns3::ccnx::operator << (std::ostream &os, const CCNxRoute &route)
{
  os << "{ Route " << *route.m_prefix << ", Nexthops : { ";
  for (CCNxRoute::const_iterator i = route.begin (); i != route.end (); ++i)
    {
      os << *(*i);
    }
  os << "} }";
  return os;
}
