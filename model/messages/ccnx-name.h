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

#ifndef CCNS3_CCNXNAME_H
#define CCNS3_CCNXNAME_H

#include <string>
#include <vector>
#include <string>
#include <memory>

#include "ns3/simple-ref-count.h"
#include "ns3/output-stream-wrapper.h"

#include "ccnx-namesegment.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-messages
 *
 * A CCNxName.  A name is zero or more ordered CCNxNameSegments.  When written in human-readable form,
 * it uses the LCI URI representation.
 *
 * Names are constructed using the class CCNxNameBuilder.  Once built, a name is immutable.
 *
 * Each name segment has a type and a value.
 */
class CCNxName : public SimpleRefCount<CCNxName>
{
public:
  friend class CCNxNameBuilder;

  /**
   * Function to compare two smart pointers to CCNx names.  May be used in std::map as the
   * comparison function to properly order names.
   *
   * Example:
   * @code
   * {
   *   typedef std::map< Ptr<CCNxName>, Ptr<NfpAnchorAdvertisement>, CCNxName::isLessPtrCCNxName > AnchorMapType;
   * }
   * @endcode
   */
  struct isLessPtrCCNxName
  {
    bool operator() (Ptr<const CCNxName> a, Ptr<const CCNxName> b) const
    {
      return *a < *b;
    }
  };

  /**
   * Creates a name from the URI representation.
   * uri = "ccnx:/NAME=foo/VER=bar"
   */
  CCNxName (const std::string &uri);

  /**
   * Copies the given name.  The NameSegments are references to the same Ptr<CCNxNameSegment>.
   */
  CCNxName (const CCNxName &copy);

  virtual ~CCNxName ();

  /**
   * Returns the number of name segments in the name.
   */
  size_t GetSegmentCount () const;

  /**
   * Returns the specified name segment.  Will assert if the index is beyond the end
   * of the GetSegmentCount().
   */
  Ptr<const CCNxNameSegment> GetSegment (size_t index) const;

  /**
   * Determines if the name is equal to the other name.  For two names to be equal, they
   * must be the same length and each name segment must be of the same type and value.
   */
  bool Equals (const CCNxName &other) const;

  /**
   * Determines if the name sorts before the other name.
   *
   * Name A is less than name B if:
   *    |A| < |B| or
   *    (|A| = |B| and there exits a name segment S such that for all name segments
   *     R before S, A[R] = B[R] and A[S] < B[S]).
   */
  bool operator< (const CCNxName &other) const;

  /**
   * Return true if this name is a prefix of the other name.
   *
   * name 'A' is a prefix of name 'B' if every name component in A occurs in
   * B in the same order.
   * @param other
   * @return
   */
  bool IsPrefixOf (const CCNxName &other) const;

protected:
  /**
   * Creates a name with zero name segments.
   */
  CCNxName ();

  /**
   * The data structure used to store name segments.
   */
  typedef std::vector< Ptr<const CCNxNameSegment> > CCNxNameStorageType;

  /**
   * Parse a URI in to a name segment vector.
   */
  CCNxNameStorageType parse_uri (const std::string &uri);

  /**
   * Internal storage of name segments.
   */
  CCNxNameStorageType m_segments;
};

std::ostream &operator<< (std::ostream &os, const CCNxName &name);

}
}

#endif //CCNS3_CCNXNAME_H
