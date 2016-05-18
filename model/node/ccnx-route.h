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


#ifndef CCNS3SIM_MODEL_NODE_CCNX_ROUTE_H_
#define CCNS3SIM_MODEL_NODE_CCNX_ROUTE_H_

#include <map>
#include <vector>
#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-connection.h"
#include "ns3/ccnx-route-entry.h"
#include "ns3/ccnx-name.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-l3
 *
 * Represents a route to a prefix.  It is zero or more CCNxRouteEntry.
 *
 * CCNxRoute behaves like an STL container for CCNxRouteEntry.  It provides a const_iterator and cbegin() and cend().
 */
class CCNxRoute : public SimpleRefCount<CCNxRoute>
{
public:
  /**
   * Create a CCNxRoute for the given prefix.  The set of nexthops will be empty and may be set
   * by calling `AddNexthop()` and `AddRoute()`.
   * @param [in] prefix The name prefix this route represents.
   */
  CCNxRoute (Ptr<const CCNxName> prefix);

  /**
   * Copy an existing CCNxRoute.
   * @param [in] copy The original route to copy.
   */
  CCNxRoute (const CCNxRoute &copy);


  virtual ~CCNxRoute ();

  /**
   * If the same nexthop (connection) is added more than once, we will only keep
   * the entry with the minimum distance.
   *
   * PRECONDITION: The prefix in nexthop must be the same as our prefix.
   *
   * @param nexthop
   */
  void AddNexthop (Ptr<const CCNxRouteEntry> nexthop);

  /**
   * Add all the entries in otherRoute to this route.
   *
   * PRECONDITION: The prefix in otherRoute must be the same as our prefix.
   *
   * @param otherRoute The other route to add to this one.
   */
  void AddRoute (Ptr<const CCNxRoute> otherRoute);

  /**
   * Remove the specified connection ID from the set of nexthops
   *
   * @param [in] connection The connection  to remove.
   * @return The number of entries removed
   */
  size_t RemoveNexthop (Ptr<CCNxConnection> connection);


  typedef std::vector< Ptr<const CCNxRouteEntry> > NexthopListType;

  /**
   * Returns a vector of all the nexthop entries.
   * @return
   */
  const NexthopListType GetNexthops (void) const;

  /**
   * Determines if two CCNxRoute are the same.  They are the same if their prefix are the same,
   * they have the same number of CCNxRouteEntry, and every CCNxRouteEntry in this object is also
   * in the other object.
   *
   * @param [in] other The other CCNxRoute to test against
   * @return true if identical routes
   * @return false if there is a difference between them
   */
  bool Equals (const CCNxRoute &other) const;

  /**
   * Function to compare two smart pointers to connectionIds.  May be used in std::map as the
   * comparison function to properly order connections.
   *
   * Example:
   * @code
   * {
   *     typedef std::map<Ptr<CCNxConnection>, Ptr<const CCNxRouteEntry>, isLessPtrCCNxConnection > ConnectionMapType;
   * }
   * @endcode
   */

  struct isLessPtrCCNxConnection
  {
    bool operator() (Ptr<const CCNxConnection> a, Ptr<const CCNxConnection> b) const
    {
      return a->GetConnectionId () < b->GetConnectionId ();
    }
  };

  typedef std::map<Ptr<CCNxConnection>, Ptr<const CCNxRouteEntry>, isLessPtrCCNxConnection > ConnectionMapType;

  friend std::ostream & operator << (std::ostream &os, const CCNxRoute &route);

private:
  Ptr<const CCNxName> m_prefix;

  ConnectionMapType m_connectionsMap;

public:
  // We are not actually a template class, so fake out the "T" parameter
  typedef Ptr<CCNxRouteEntry> T;
  typedef std::allocator<T> allocator_type;

  class const_iterator
  {
public:
    friend class CCNxRoute;

    typedef Ptr<const CCNxRouteEntry> const_reference;
    typedef Ptr<const CCNxRouteEntry> const_pointer;
    typedef std::forward_iterator_tag iterator_category;

    const_iterator (ConnectionMapType::const_iterator iter);
    const_iterator (const CCNxRoute::const_iterator&);
    ~const_iterator ();

    const_iterator& operator= (const const_iterator&);
    bool operator== (const const_iterator&) const;
    bool operator!= (const const_iterator&) const;

    const_iterator& operator++ ();

    const_reference operator* () const;
    const_pointer operator-> () const;

private:
    ConnectionMapType::const_iterator m_iter;
  };

  const_iterator begin () const;
  const_iterator cbegin () const;
  const_iterator end () const;
  const_iterator cend () const;

  /**
   * The number of route entries
   * @return The number of route entries.
   */
  size_t size () const;

  /**
   * Determines if the CCNxRoute has any next hops.
   * @return true If there is at least 1 nexthop
   * @return false If there are no nexthops.
   */
  bool empty () const;

  /**
   * Remove all nexthops
   */
  void clear ();


};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_NODE_CCNX_ROUTE_H_ */
