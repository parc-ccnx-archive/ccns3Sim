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

#ifndef CCNS3_NAMESEGMENT_H
#define CCNS3_NAMESEGMENT_H

#include <memory>
#include <string>

#include "ns3/ptr.h"
#include "ns3/simple-ref-count.h"

namespace ns3 {
namespace ccnx {

typedef enum
{
  CCNxNameSegment_Name,
  CCNxNameSegment_Version,
  CCNxNameSegment_Chunk,
  CCNxNameSegment_App0,
  CCNxNameSegment_App1,
  CCNxNameSegment_App2,
  CCNxNameSegment_App3,
  CCNxNameSegment_App4
} CCNxNameSegmentType;

/**
 * @ingroup ccnx-messages
 *
 * Represents a single name segment.  Each name segment has both a label (equivalent to
 * its TLV type) and value (the actual octet string).
 *
 * For example, the name ccnx:/name=foo/ver=bar has two name segments.  The first name segment
 * has label NAME and value foo, the second has label VER and value bar.
 *
 * It is mandatory to include the labels in the LCI URI, you cannot use implicit labels.
 *
 * The label NAME is equivalent to T_NAMESEG_NAME and VER is equivalent to T_NAMESEG_VERSION.
 *
 * @see CCNxSchemaV1
 */
class CCNxNameSegment : public SimpleRefCount<CCNxNameSegment>
{
public:
  /**
   * Create a name segment from a string in the form "label=value".  you must
   * specify the label.
   */
  CCNxNameSegment (const std::string &s);
  CCNxNameSegment (CCNxNameSegmentType type, const char *c_str);
  CCNxNameSegment (CCNxNameSegmentType type, const std::string &value);
  CCNxNameSegment (const CCNxNameSegment &copy);

  bool Equals (const CCNxNameSegment &other) const;
  int Compare (const CCNxNameSegment &other) const;

  CCNxNameSegmentType GetType () const;
  const std::string & GetValue () const;

  static CCNxNameSegmentType TypeFromString (const std::string &s);
  static std::string TypeFromEnum (CCNxNameSegmentType type);

protected:
  CCNxNameSegmentType m_type;
  std::string m_value;

};

std::ostream& operator<< (std::ostream& os, CCNxNameSegment const& segment);

} // namespace ccnx
} // namespace ns3

#endif //CCNS3_NAMESEGMENT_H
