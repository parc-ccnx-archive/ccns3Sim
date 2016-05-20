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

#ifndef CCNS3SIM_CCNXHASHVALUE_H
#define CCNS3SIM_CCNXHASHVALUE_H

#include "ns3/ptr.h"
#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-byte-array.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-crypto
 *
 * Represents a hash value.
 *
 * We use a uint64_t for all hash values, even if they are things like SHA256.
 * In general, we fake all crypto routines.
 */
class CCNxHashValue : public SimpleRefCount<CCNxHashValue>
{
public:
  /**
   * Function to compare two smart pointers to CCNx hash values.  May be used in std::map as the
   * comparison function to properly order names.
   *
   * Example:
   * @code
   * {
   *   typedef std::map< Ptr<const CCNxHashValue>, Ptr<CCNxPitEntry>, CCNxHashValue::isLessPtrCCNxHashValue > NamelessPitType;
   * }
   * @endcode
   */
  struct isLessPtrCCNxHashValue
  {
    bool operator() (Ptr<const CCNxHashValue> a, Ptr<const CCNxHashValue> b) const
    {
      return *a < *b;
    }
  };

  /**
   * For purposes of simulation, we only store the first 8 bytes of a hash value.
   * An application, in reality, does not even need to calculate real hashes.  it can
   * assign unique values as it sees fit.  The rest of the hash is padded with 0's out to
   * the length.
   *
   * This constructor assumes the hash is 32 bytes (e.g. SHA-256)
   *
   * @param [in] value The value to initialize this object to.
   */
  CCNxHashValue (uint64_t value);

  /**
   * For purposes of simulation, we only store the first 8 bytes of a hash value.
   * An application, in reality, does not even need to calculate real hashes.  it can
   * assign unique values as it sees fit.  The rest of the hash is padded with 0's out to
   * the length.
   *
   * @param [in] value The value to initialize this object to.
   * @param [in] length The simulated length of the hash value (e.g. 32 for SHA-256)
   */
  CCNxHashValue (uint64_t value, size_t length);

  /**
   * Create a HashValue from a byte array.  The hash length will be whatever the value's length is.
   *
   * @param [in] value The value to initialize this object to.
   */
  CCNxHashValue (Ptr<const CCNxByteArray> value);

  /**
   * For purposes of simulation, it is usually sufficient to treat hash values as
   * a uint64_t.  This function returns the first 8 bytes of the hash value.
   *
   * @return The first 8 bytes of the hash value as uint64_t.
   */
  Ptr<const CCNxByteArray> GetValue (void) const;

  /**
   * Represent the hash value as a CCNxBuffer.  Useful for encoding.
   * @return An allocated CCNxBuffer
   */
  Ptr<CCNxBuffer> CreateBuffer () const;

  /**
   * Determines if this CCNxHashValue is equal to the other value.
   *
   * @param [in] other The other hash value to compare against
   * @return true if equal
   * @return false if not equal
   */
  bool operator == (const CCNxHashValue &other) const;

  /**
   * Determines if the value of this Hash is less than the other hash.
   *
   * @param [in] other The other hash value to compare to
   * @return true if this hash value is less than the other hash value
   * @return false if this hash value is not less than the other hash value
   */
  bool operator < (const CCNxHashValue &other) const;

private:
  void InitializeFromInteger (uint64_t value, size_t length);
  Ptr<const CCNxByteArray> m_value;
};

bool operator == (const Ptr<const CCNxHashValue> a, const Ptr<const CCNxHashValue> b);
}
}

#endif //CCNS3SIM_CCNXHASHTYPE_H
