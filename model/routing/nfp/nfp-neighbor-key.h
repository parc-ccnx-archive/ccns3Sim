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

#ifndef CCNS3SIM_MODEL_ROUTING_NFP_NFP_NEIGHBOR_KEY_H_
#define CCNS3SIM_MODEL_ROUTING_NFP_NFP_NEIGHBOR_KEY_H_

#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-connection.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup nfp-routing
 *
 * A neighbor is identified by the pair (Name, Connection), where Name is the neighbor's name
 * advertised in an NfpMessage and Connection is from our local forwarder representing the nexthop
 * address of the neighbor.
 *
 * This class encapsulates the (Name, Connection) pair to facilitate using it in a std::map.
 */
class NfpNeighborKey : public SimpleRefCount<NfpNeighborKey>
{
public:
  NfpNeighborKey (Ptr<const CCNxName> name, Ptr<CCNxConnection> connection);
  virtual ~NfpNeighborKey ();

  /**
   * Returns the neighbor's name
   *
   * @return The neighbor name
   */
  Ptr<const CCNxName> GetName (void) const;

  /**
   * Returns the connection ID of the neighbor.  A neighbor could be reachable over
   * multiple connection IDs, in which case there would be multiple of these NfpNeighborKey objects.
   *
   * @return The neighbor's connection ID.
   */
  const Ptr<CCNxConnection> GetConnection () const;

  /**
   * Sorts neighbor keys, such as for use in a std::map.
   *
   * The sorting is done first on Connection (because that's a simple type), then
   * on the CCNxName.  Because no two CCNxNeighbors should share the same connection id
   * (unless they are in a multicast group), all the sorting will be done on the simple type.
   *
   * @param [in] other The other NfpNeighborKey to compare to
   * @return true if this object sorts before the other object
   * @return false if this object does not sort before the other object
   *
   * Example:
   * @code
   * {
   *   NfpNeighborKey a(Create<CCNxName>("ccnx:/name=foo"), 1);
   *   NfpNeighborKey b(Create<CCNxName>("ccnx:/name=foo"), 2);
   *   bool success = a.IsLess(b);
   *   NS_ASSERT_MSG(success, "A should be less than B");
   * }
   * @endcode
   */
  bool IsLess (NfpNeighborKey const &other) const;

  /**
   * Checks that the pointer is not null, then compares with the other NfpNeighborKey.
   * If the other pointer is null, will return false.
   *
   * @param [in] other The other NfpNeighborKey to compare to
   * @return true if this object sorts before the other object
   * @return false if this object does not sort before the other object
   */
  bool IsLess (Ptr<NfpNeighborKey> other) const;


  friend std::ostream & operator << (std::ostream &os, const NfpNeighborKey &neighborKey);

  /**
   * Function to compare two smart pointers to NfpNeighborKey.  May be used in std::map as the
   * comparison function to properly order names.
   *
   * Example:
   * @code
   * {
   *   typedef std::map< Ptr<NfpNeighborKey>, Ptr<NfpNeighbor>, NfpNeighbor::isLessPtrNfpNeighborKey > NeighborMapType;
   * }
   * @endcode
   */
  struct isLessPtrNfpNeighborKey
  {
    bool operator() (const Ptr<NfpNeighborKey> a, const Ptr<NfpNeighborKey> b) const
    {
      return a->IsLess (b);
    }
  };

private:
  Ptr<const CCNxName> m_name;
  Ptr<CCNxConnection> m_connection;
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_ROUTING_NFP_NFP_NEIGHBOR_KEY_H_ */
