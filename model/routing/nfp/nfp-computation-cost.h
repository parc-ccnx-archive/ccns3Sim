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

#ifndef CCNS3SIM_MODEL_ROUTING_NFP_NFP_COMPUTATION_COST_H_
#define CCNS3SIM_MODEL_ROUTING_NFP_NFP_COMPUTATION_COST_H_

#include <ostream>
#include <stdint.h>

namespace ns3
{
  namespace ccnx
  {

    /**
     * @ingroup nfp-routing
     *
     * Computational cost is measured in the number of events and the number of loop iterations.
     *
     * An event is something like packet received or timer callback.
     */
    class NfpComputationCost
    {
    public:
      NfpComputationCost ();
      NfpComputationCost (const NfpComputationCost &copy);
      virtual ~NfpComputationCost ();

      void IncrementEvents();
      void IncrementLoopIterations();
      void IncrementLoopIterations(uint64_t count);

      /**
       * Add one computation cost to this computation cost
       *
       * @param other [in] The other one to add
       * @return This result of the summation
       */
      NfpComputationCost operator + (const NfpComputationCost &other) const;

      /**
       * Add one computation cost to this computation cost
       *
       * @param other [in] The other one to add
       * @return This computation cost
       */
      NfpComputationCost & operator += (const NfpComputationCost &other);

      /**
       * Two objects are equal if they have the same event count and loop iteration count
       *
       * @param other
       * @return
       */
      bool operator == (const NfpComputationCost &other) const;

      bool operator != (const NfpComputationCost &other) const;

      uint64_t GetEvents() const;
      uint64_t GetLoopIterations() const;
      uint64_t GetTotalCost() const;

      friend std::ostream & operator << (std::ostream &os, const NfpComputationCost &cost);

    private:
      uint64_t	m_events;		/*< The number of events its callback */
      uint64_t	m_loopIterations;	/*< The number of transit through any control loop */
    };

  } /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_ROUTING_NFP_NFP_COMPUTATION_COST_H_ */
