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

#ifndef CCNS3SIM_MODEL_MESSAGES_CCNX_NAME_BUILDER_H_
#define CCNS3SIM_MODEL_MESSAGES_CCNX_NAME_BUILDER_H_

#include <vector>
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-namesegment.h"

namespace ns3 {
namespace ccnx {

/**
   * A CCNxName is an immutable object, so it is constructed and modified using a CCNxNameBuilder.
   * A name builder allows the user to add and remove name segments.  It also supports an STL-style
   * const_iterator over those name segments.
   *
   * Example: (This code sample will return "ccnx:/name=foo/name=bar/name=car")
   * @code
   * CCNxNameBuilder test;
   * test.Append(Create<const CCNxNameSegment>("name=foo"));
   * test.Append(Create<const CCNxNameSegment>("name=bar"));
   * test.Append(Create<const CCNxNameSegment>("name=car"));
   *
   * Ptr<const CCNxName> name = builder.CreateName();
   * @endcode
   */
class CCNxNameBuilder
{
public:
  /**
   * Construct an empty name builder.  Corresponds to ccnx:/.
   */
  CCNxNameBuilder ();

  /**
   * Construct a name builder that begins initialized to the
   * given URI.
   *
   * @param [in] uri The starting point for the name builder.
   */
  CCNxNameBuilder (std::string uri);

  /**
   * Copy another name builder.
   * @param [in] copy Another name builder
   */
  CCNxNameBuilder (const CCNxNameBuilder &copy);

  /**
   * Construct a name builder that begins initialized to the
   * given name.
   *
   * @param [in] copy Name to use as starting point
   */
  CCNxNameBuilder (const CCNxName &copy);

  /**
   * Destructor
   */
  virtual ~CCNxNameBuilder ();

  /**
   * Creates a name out of the various name segments of a name builder
   *
   * Example: (This code sample will return "ccnx:/name=foo/name=bar/name=car")
   * @code
   * CCNxNameBuilder test;
   * test.Append(Create<const CCNxNameSegment>("name=foo"));
   * test.Append(Create<const CCNxNameSegment>("name=bar"));
   * test.Append(Create<const CCNxNameSegment>("name=car"));
   *
   * Ptr<const CCNxName> name = builder.CreateName();
   * @endcode
   */
  Ptr<const CCNxName> CreateName ();

  /**
   * Append a name segment to the name.
   * @param [in] segment Stores a ref count of the segment
   */
  void Append (Ptr<const CCNxNameSegment> segment);

  /**
   * Trims the right-most name segment.  If called on an empty CCNxNameBuilder, it has no effect.
   */
  void Trim ();

  /**
   * Container-like interface
   */

  /**
   * The number of name segments
   * @return
   */
  size_t size () const;

  /**
   * Checks if there are name segments or not.
   * @return true if there are no name segments, else false
   */
  bool empty () const;

  /**
   * Removes all name segments
   */
  void clear ();

  /**
   * Allow random access to a name segment
   */
  Ptr<const CCNxNameSegment> operator [] (const size_t index) const;

  /**
   * Test if this name builder is the same as the other name builder.
   *
   * Two name builders are the same if they have the same number of name segments and
   * in order, each name segment is equal.
   *
   * @param [in] other The other CCNxNameBuilder to test against
   * @return true if equal
   * @return false if not equal
   */
  bool operator == (const CCNxNameBuilder &other) const;

  /**
   * Use SegmentArrayType as a typedef
   */
  typedef std::vector< Ptr<const CCNxNameSegment> > SegmentArrayType;

private:
  SegmentArrayType m_segments;

public:
  /**
   * Use allocator_type as a typedef
   */
  typedef std::allocator< Ptr<const CCNxNameSegment> > allocator_type;

  /**
   * Class const_iterator defined to iterate over all the name segments of a name builder
   */
  class const_iterator
  {
public:
    /**
     * Use const_reference/const_pointer/iterator_category as a typedef
     */
    typedef Ptr<const CCNxNameSegment> const_reference;
    typedef Ptr<const CCNxNameSegment> const_pointer;
    typedef std::forward_iterator_tag iterator_category;

    /**
     * Construct a const_iterator given a pointer to the name segment
     */
    const_iterator (CCNxNameBuilder::SegmentArrayType::const_iterator iter);

    /**
     * Copy Constructor to copy from one const_iterator to another
     */
    const_iterator (const CCNxNameBuilder::const_iterator &);

    /**
     * Destructor
     */
    ~const_iterator ();

    /**
     * Define an equal-to operator for const iterator
     */
    bool operator== (const const_iterator&) const;

    /**
     * Define a not-equal-to operator for const iterator
     */
    bool operator!= (const const_iterator&) const;

    /**
     * Define an increment operator for const iterator
     */
    const_iterator& operator++ ();

    /**
     * Define a dereference operator for const iterator
     */
    const_reference operator* () const;

    /**
     * Define a pointer operator for const iterator
     */
    const_pointer operator-> () const;

protected:
    SegmentArrayType::const_iterator m_iter;
  };

  /**
   * Define begin/end for const iterator
   */
  const_iterator begin () const;
  const_iterator cbegin () const;
  const_iterator end () const;
  const_iterator cend () const;
};

} /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_MESSAGES_CCNX_NAME_BUILDER_H_ */
