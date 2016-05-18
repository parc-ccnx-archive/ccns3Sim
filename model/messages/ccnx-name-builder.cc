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
#include "ccnx-name-builder.h"

using namespace ns3;
using namespace ns3::ccnx;

CCNxNameBuilder::CCNxNameBuilder ()
{
  // empty
}

CCNxNameBuilder::CCNxNameBuilder (std::string uri)
{
  CCNxName name (uri);
  for (int i = 0; i < name.GetSegmentCount (); ++i)
    {
      m_segments.push_back (name.GetSegment (i));
    }
}


CCNxNameBuilder::CCNxNameBuilder (const CCNxNameBuilder &copy) :
  m_segments (copy.m_segments)
{
  // empty
}

CCNxNameBuilder::CCNxNameBuilder (const CCNxName &copy)
{
  for (int i = 0; i < copy.GetSegmentCount (); ++i)
    {
      m_segments.push_back (copy.GetSegment (i));
    }
}

CCNxNameBuilder::~CCNxNameBuilder ()
{

}

//template CCNxName<>(CCNxNameBuilder::const_iterator, CCNxNameBuilder::const_iterator);

Ptr<const CCNxName>
CCNxNameBuilder::CreateName ()
{
  // TODO: we should cache this.

  // Call the protected default constructor
  Ptr<CCNxName> name = Ptr<CCNxName> (new CCNxName (), false);
  name->m_segments.insert (name->m_segments.begin (), this->m_segments.cbegin (), this->m_segments.cend ());
  return name;
}

// ============

void
CCNxNameBuilder::Append (Ptr<const CCNxNameSegment> segment)
{
  m_segments.push_back (segment);
}

void
CCNxNameBuilder::Trim ()
{
  if (!m_segments.empty ())
    {
      m_segments.pop_back ();
    }
}

static bool
equalsPredicate (Ptr<const CCNxNameSegment> a, Ptr<const CCNxNameSegment> b)
{
  return a->Equals (*b);
}

bool
CCNxNameBuilder::operator == (const CCNxNameBuilder &other) const
{
  bool result = false;
  if (size () == other.size ())
    {
      result = std::equal (begin (), end (), other.begin (), equalsPredicate);
    }
  return result;
}

// ============
// Container-like interface

size_t
CCNxNameBuilder::size () const
{
  return m_segments.size ();
}

/**
 * True if there are no name segments.
 * @return
 */
bool
CCNxNameBuilder::empty () const
{
  return m_segments.empty ();
}

/**
 * Remove all name segments
 */
void
CCNxNameBuilder::clear ()
{
  m_segments.clear ();
}

/**
 * Allow random access to a name segment
 */
Ptr<const CCNxNameSegment>
CCNxNameBuilder::operator [] (const size_t index) const
{
  NS_ASSERT_MSG (index < m_segments.size (), "Index " << index << " beyond end of storage " << m_segments.size ());
  return m_segments[index];
}

CCNxNameBuilder::const_iterator
CCNxNameBuilder::begin () const
{
  return const_iterator (m_segments.begin ());
}

CCNxNameBuilder::const_iterator
CCNxNameBuilder::cbegin () const
{
  return const_iterator (m_segments.cbegin ());
}

CCNxNameBuilder::const_iterator
CCNxNameBuilder::end () const
{
  return const_iterator (m_segments.end ());
}

CCNxNameBuilder::const_iterator
CCNxNameBuilder::cend () const
{
  return const_iterator (m_segments.cend ());
}


// =============
// Iterator

CCNxNameBuilder::const_iterator::const_iterator (CCNxNameBuilder::SegmentArrayType::const_iterator iter) :
  m_iter (iter)
{
  // empty
}

CCNxNameBuilder::const_iterator::const_iterator (const CCNxNameBuilder::const_iterator& copy) :
  m_iter (copy.m_iter)
{
  // empty
}

CCNxNameBuilder::const_iterator::~const_iterator ()
{
  // empty
}

bool
CCNxNameBuilder::const_iterator::operator == (const const_iterator& other) const
{
  return m_iter == other.m_iter;
}

bool
CCNxNameBuilder::const_iterator::operator != (const const_iterator& other) const
{
  return m_iter != other.m_iter;
}

CCNxNameBuilder::const_iterator&
CCNxNameBuilder::const_iterator::operator++ ()
{
  ++m_iter;
  return *this;
}

CCNxNameBuilder::const_iterator::const_reference
CCNxNameBuilder::const_iterator::operator* () const
{
  // return a "Ptr<const CCNxNameSegment>"
  return *m_iter;
}

CCNxNameBuilder::const_iterator::const_pointer
CCNxNameBuilder::const_iterator::operator-> () const
{
  // return a "Ptr<const CCNxNameSegment>"
  return *m_iter;
}

