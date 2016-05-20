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


#ifndef CCNS3SIM_CCNXSTANDARDFIBENTRY_H
#define CCNS3SIM_CCNXSTANDARDFIBENTRY_H

#include <ns3/ccnx-connection.h>
#include "ns3/simple-ref-count.h"



namespace ns3 {
namespace ccnx {





/**
 * @ingroup ccnx-standard-forwarder
 *
 * The CCNx standard FIB entry class contains a fib entry and methods for the fib class to call.
 * This standard Fib Entry contains  a vector of the next hop connections for matching route prefix.
 * A lookup on an entry returns the vector of connections.
 * This class is derived from object to support run-time replacement with a more complex class.
 *
 *
 */

class CCNxStandardFibEntry : public ns3::SimpleRefCount<CCNxStandardFibEntry>
{


public:
  CCNxStandardFibEntry ();
  virtual ~CCNxStandardFibEntry ();

  /*
   * vector of connections. using vector instead of list  - simpler
   */
  typedef  std::vector< Ptr<CCNxConnection> > ConnectionsVecType;

  /**
   * RemoveConnection - remove a connection (if present) from the entry.
   *
   * @param [in] connection
   * @return = number of connections left in m_fibEntry.
   * @return = 0 if the m_fibEntry now contains no information and can be deleted.
   * @return = -1 if the link is not in the m_fibEntry.
   *
   * Example:
   * @code
   * {
   *
   *     typedef std::map<Ptr<const CCNxName>, Ptr<CCNxStandardFibEntry> , CCNxName::comparePtrCCNxName >  FibType;
   *     FibType fib;
   *     Ptr<CCNxStandardFibEntry> m_fibEntry = fib[ccnxName];
   *     if (m_fibEntry != NULL) {
   *		int linkCnt = m_fibEntry->RemoveConnection(connection);
   *                    if (linkCnt == 0)
   *	                fib.erase(ccnxName);
   *	}
   *
   * }
   * @endcode
   *
   * @see ccnxFib_Release
   */

  virtual std::pair<bool,int> RemoveConnection (Ptr<CCNxConnection> & connection);


  /**
   * AddConnection - Add a connection to the entry.
   *
   * @param [in] connection
   * @return = number of connections in m_fibEntry after adding this one.
   *
   * Example:
   * @code
   * {
   *
   *     typedef std::map<Ptr<const CCNxName>, Ptr<CCNxStandardFibEntry> , CCNxName::comparePtrCCNxName >  FibType;
   *     FibType fib;
   *     Ptr<CCNxStandardFibEntry> m_fibEntry = fib[ccnxName];
   *     if (m_fibEntry != NULL) {
   *		m_fibEntry->AddConnection(connection);
   *		}
   *     else {
   *		fib[ccnxName] = Create<CCNxStandardFibEntry> ();
   *		fib[ccnxName]->AddConnection(connection);
   *	     }
   *
   * }
   * @endcode
   *
   * @see ccnxFib_Release
   */
  virtual int AddConnection ( Ptr<CCNxConnection> & connection);





  /**
   * GetConnections - return all connections for the fib entry
   *
   * @param [in]
   * @return =  connection ids vector
   *
   * Example:
   * @code
   * {
   *
   *     typedef std::map<Ptr<const CCNxName>, Ptr<CCNxStandardFibEntry> , CCNxName::comparePtrCCNxName >  FibType;
   *     FibType fib;
   *
   *     FibType::iterator bestMatch;
   *     FibType::iterator bestMatch = fib.find(ccnxNameCopy);
   *     connectionsVec = bestMatch->second->GetConnections();
   * }
   * @endcode
   *
   * @see ccnxFib_Release
   */
  virtual ConnectionsVecType GetConnections ();

  friend std::ostream &operator<< (std::ostream &os, CCNxStandardFibEntry  &ccnxStandardFibEntry);

private:
  ConnectionsVecType m_connectionsVec;



};          //class
}    //namespace
} //namespace







#endif //CCNS3SIM_CCNXSTANDARDFIBENTRY_H
