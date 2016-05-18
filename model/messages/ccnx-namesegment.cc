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

#include <algorithm>
#include <string.h>
#include <ctype.h>
#include "ns3/log.h"
#include "ccnx-namesegment.h"

/*
 * C++ makes it way too hard to print long hex strings to an output stream.
 * We us this 256 element array to print a uint8_t in hex.
 */
static const char * hexStrings[] = {
  "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
  "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
  "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
  "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
  "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
  "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
  "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
  "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
  "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
  "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
  "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
  "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
  "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
  "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df",
  "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
  "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff",
};

using namespace ns3;
using namespace ns3::ccnx;

NS_LOG_COMPONENT_DEFINE ("CCNxNameSegment");

struct label_str
{
  CCNxNameSegmentType type;
  std::string label;
  bool sentinel;
} _LABEL_STR[] =
{
  { CCNxNameSegment_Name, std::string ("NAME"), false },
  { CCNxNameSegment_Version, std::string ("VER"), false },
  { CCNxNameSegment_Chunk, std::string ("CHUNK"), false },
  { CCNxNameSegment_App0, std::string ("APP0"), false },
  { CCNxNameSegment_App1, std::string ("APP1"), false },
  { CCNxNameSegment_App2, std::string ("APP2"), false },
  { CCNxNameSegment_App3, std::string ("APP3"), false },
  { CCNxNameSegment_App4, std::string ("APP4"), false },
  { CCNxNameSegment_Name, std::string (), true }
};

CCNxNameSegment::CCNxNameSegment (CCNxNameSegmentType type, const std::string &value)
  : m_type (type), m_value (std::string (value))
{
  // empty
}

CCNxNameSegment::CCNxNameSegment (const CCNxNameSegment &copy)
  : m_type (copy.GetType ()), m_value (copy.GetValue ())
{
  // empty
}

CCNxNameSegment::CCNxNameSegment (CCNxNameSegmentType type, const char *c_str)
  : m_type (type), m_value (std::string (c_str))
{
  // empty
}

CCNxNameSegmentType
CCNxNameSegment::GetType () const
{
  return m_type;
}

const std::string &
CCNxNameSegment::GetValue () const
{
  return m_value;
}

bool
CCNxNameSegment::Equals (const CCNxNameSegment &other) const
{
  std::shared_ptr<std::string> value;
  bool result = false;
  // need to dereference the getValue because its returning a shared_ptr.
  if ((this->GetType () == other.GetType ())
      && (this->GetValue () == other.GetValue ()))
    {
      result = true;
    }

  return result;
}

int
CCNxNameSegment::Compare (const CCNxNameSegment &other) const
{
  int result = 0;
  if (m_type < other.m_type)
    {
      result = -1;
    }
  else if (m_type > other.m_type)
    {
      result = +1;
    }
  else
    {
      result = m_value.compare (other.m_value);
    }
  return result;
}

CCNxNameSegment::CCNxNameSegment (const std::string &s)
{
  // TODO: Should assume "NAME=" if we cannot find a label.
  size_t pos = s.find ("=");
  if (pos > 0)
    {
      std::string type = s.substr (0, pos);

      m_type = TypeFromString (type);
      m_value = s.substr (pos + 1, s.size ());

      NS_LOG_DEBUG (
        "parse '" << s << "' to type '" << m_type << "' and value '" << m_value << "'");
    }
  else
    {
      NS_LOG_ERROR ("Count not parse name segment, missing the 'label=' portion: " << s);
      // TODO: change to an NS assert
      std::terminate ();
    }
}

CCNxNameSegmentType
CCNxNameSegment::TypeFromString (const std::string &s)
{
  for (int i = 0; !_LABEL_STR[i].sentinel; i++)
    {

      if (0 == strcasecmp (_LABEL_STR[i].label.c_str (), s.c_str ()))
        {
          return _LABEL_STR[i].type;
        }
    }

  NS_LOG_ERROR ("Could not find type from string " << s);
  std::terminate ();
}

std::string
CCNxNameSegment::TypeFromEnum (CCNxNameSegmentType type)
{
  for (int i = 0; !_LABEL_STR[i].sentinel; i++)
    {
      if (_LABEL_STR[i].type == type)
        {
          return _LABEL_STR[i].label;
        }
    }

  NS_LOG_ERROR ("Could not find type " << type);
  std::terminate ();
}

static bool
CheckNotPrintable (const char ch)
{
  return !isprint (ch);
}

static bool
IsPrintable (std::string const &str)
{
  // If any of the characters in the string are not printable ascii, return false.
  // Scan each character of the string and if CheckNotPrintable is true, find_if will return
  // an iterator to that location (which is not cend()).  If we reach cend() then all
  // characters are printable.
  bool result = std::find_if (str.cbegin (), str.cend (), CheckNotPrintable) == str.cend ();
  return result;
}

std::ostream &
ns3::ccnx::operator << (std::ostream &os, CCNxNameSegment const &segment)
{
  os << CCNxNameSegment::TypeFromEnum (segment.GetType ()) << "=";

  if (IsPrintable (segment.GetValue ()))
    {
      os << segment.GetValue ();
    }
  else
    {
      os << "0x";
      const uint8_t *p = (const uint8_t *) segment.GetValue ().c_str ();
      for (int i = 0; i < segment.GetValue().size(); ++i) {
	  unsigned byte = (unsigned) p[i];
          os << ::hexStrings[byte];
      }
    }
  return os;
}
