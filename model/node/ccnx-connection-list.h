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

#ifndef CCNS3SIM_MODEL_NODE_CCNX_CONNECTION_LIST_H_
#define CCNS3SIM_MODEL_NODE_CCNX_CONNECTION_LIST_H_

#include <list>
#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-connection.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-l3
 *
 * A memory managed list of `Ptr<CCNxConnection>`s.  This is so we don't
 * need to toss around a `std::list< Ptr<CCNxConnection> >`.
 *
 * Roughly resembles the same API as std::list<> but we do not have an iterator.
 */
class CCNxConnectionList : public SimpleRefCount<CCNxConnectionList>
{
public:
  CCNxConnectionList ();
  virtual ~CCNxConnectionList ();

  CCNxConnectionList (CCNxConnectionList &copy);

  size_t size () const;

  void clear ();

  void push_back (Ptr<CCNxConnection> connection);

  Ptr<CCNxConnection> front () const;

  void pop_front ();

  Ptr<CCNxConnection> back () const;

  typedef std::list< Ptr<CCNxConnection> > StorageType;


private:
  StorageType m_storage;


public:
  /**
   * Use allocator_type as a typedef
   */
  typedef std::allocator< Ptr<CCNxConnection> > allocator_type;

  /**
   * Class const_iterator defined to iterate over all the name segments of a name builder
   */
  class iterator
  {
public:
    /**
     * Use reference/pointer/iterator_category as a typedef
     */
    typedef Ptr<CCNxConnection> reference;
    typedef Ptr<CCNxConnection> pointer;
    typedef std::forward_iterator_tag iterator_category;

    /**
     * Construct a iterator given a pointer to the name segment
     */
    iterator (CCNxConnectionList::StorageType::iterator iter);

    /**
     * Copy Constructor to copy from one iterator to another
     */
    iterator (const CCNxConnectionList::iterator &);

    /**
     * Destructor
     */
    ~iterator ();

    /**
     * Define an equal-to operator for iterator
     */
    bool operator == (const iterator&) const;

    /**
     * Define a not-equal-to operator for iterator
     */
    bool operator != (const iterator&) const;

    /**
     * Define a prefix increment operator for iterator
     */
    iterator & operator++ ();

    /**
     * Define postfix increment operator for iterator
     */
    iterator operator ++ (int);

    /**
     * Define a dereference operator for iterator
     */
    reference operator* () const;

    /**
     * Define a pointer operator for iterator
     */
    pointer operator-> () const;

protected:
    StorageType::iterator m_iter;
  };

  /**
   * Define begin/end for const iterator
   */
  iterator begin ();
  iterator end ();

};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_NODE_CCNX_CONNECTION_LIST_H_ */
