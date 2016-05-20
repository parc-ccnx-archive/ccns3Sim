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

#include <ns3/assert.h>
#include <ns3/log.h>

#include "ccnx-name.h"

#include <exception>

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxName");

CCNxName::CCNxName ()
{
}

CCNxName::CCNxName (const std::string &uri)
{
  m_segments = parse_uri (uri);
}

CCNxName::CCNxName (const CCNxName &copy)
{
  // allocates a new vector, but uses the same Ptr<NameSegment> inside the vector.
  m_segments = copy.m_segments;
}

CCNxName::~CCNxName ()
{

}

size_t
CCNxName::GetSegmentCount () const
{
  return m_segments.size ();
}

Ptr<const CCNxNameSegment>
CCNxName::GetSegment (size_t index) const
{
  NS_ASSERT_MSG (index < GetSegmentCount (), "Index must be less than GetSegmentCount()");

  return m_segments[index];
}


bool
CCNxName::Equals (const CCNxName &other) const
{
  bool result = false;
  if (m_segments.size () == other.m_segments.size ())
    {
      result = true;
      for (int i = 0; i < m_segments.size (); i++)
        {
          Ptr <const CCNxNameSegment> a = GetSegment (i);
          Ptr <const CCNxNameSegment> b = other.GetSegment (i);

          if (!a->Equals (*b))
            {
              result = false;
              break;
            }
        }
    }
  return result;
}

bool
CCNxName::operator < (const CCNxName &other) const
{
  bool isLess = false;

  if (GetSegmentCount () < other.GetSegmentCount ())
    {
      isLess = true;
    }
  else if (GetSegmentCount () == other.GetSegmentCount ())
    {
      for (int i = 0; i < GetSegmentCount (); i++)
        {
          Ptr<const CCNxNameSegment> a = GetSegment (i);
          Ptr<const CCNxNameSegment> b = other.GetSegment (i);

          int cmp = a->Compare (*b);

          if ( cmp < 0 )
            {
              isLess = true;
              break;
            }
          else if (cmp == 0)
            {
              continue;
            }
          else
            {
              // false
              break;
            }
        }
    }

  return isLess;
}

bool
CCNxName::IsPrefixOf (const CCNxName &other) const
{
  bool result = false;
  if (m_segments.size () <= other.m_segments.size ())
    {
      // assume its true and look for a negative
      result = true;
      for (int i = 0; i < GetSegmentCount (); ++i)
        {
          Ptr<const CCNxNameSegment> a = GetSegment (i);
          Ptr<const CCNxNameSegment> b = other.GetSegment (i);

          if (a->Compare (*b) != 0)
            {
              result = false;
              break;
            }
        }
    }
  return result;
}

CCNxName::CCNxNameStorageType
CCNxName::parse_uri (const std::string &uri)
{
  NS_LOG_FUNCTION (this << uri);

  CCNxNameStorageType result;

  const std::string schema ("ccnx:/");
  const int schemaOffset = schema.length ();

  std::string start = uri.substr (0, 6);

  if (schema == start)
    {
      size_t start = schemaOffset;
      bool finished = false;
      while (!finished)
        {
          size_t pos = uri.find ("/", start);

          std::string segment = uri.substr (start, pos - start);
          Ptr<CCNxNameSegment> seg = Create<CCNxNameSegment> (segment);
          result.push_back (seg);

          start = pos + 1;

          if (pos > uri.length ())
            {
              finished = true;
            }
        }
    }
  else
    {
      NS_ASSERT_MSG (false, "URI must start with 'ccnx:/'");
      std::terminate ();
    }

  return result;
}

std::ostream&
ns3::ccnx::operator<< (std::ostream& os, CCNxName const& name)
{
  size_t count = name.GetSegmentCount ();
  os << "ccnx:";

  for (size_t i = 0; i < count; i++)
    {
      os << "/" << *(name.GetSegment (i));
    }
  return os;
}

