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

#ifndef CCNS3SIM_MODEL_MESSAGES_CCNX_BYTE_ARRAY_H_
#define CCNS3SIM_MODEL_MESSAGES_CCNX_BYTE_ARRAY_H_

#include <vector>

#include "ns3/simple-ref-count.h"
#include "ns3/ccnx-buffer.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-messages
 *
 * Used to store sequences of bytes.  In C++, it is a simple wrapper around std::vector<uint8_t>.
 * This class just insulates us against changes of that backend.
 *
 * The byte array is a const class.  Once created it is immutable.
 */
class CCNxByteArray : public SimpleRefCount<CCNxByteArray>
{
public:
  CCNxByteArray (size_t length, const uint8_t *bytes);

  /**
   * Create the byte array from a vector of values.  The values are copied.
   * @param [in] value The input values.
   */
  CCNxByteArray (const std::vector< uint8_t > &value);

  virtual ~CCNxByteArray ();

  /**
   * The byte length of the array
   * @return The number of bytes in the array
   */
  size_t size () const;

  /**
   * Index operator
   *
   * Will throw exception if index is out of range.
   *
   * @param index The byte position to return, must be 0 <= index < size()
   * @return The byte value at index
   */
  const uint8_t operator [] (size_t index) const;

  /**
   * Represents the byte array as a CCNxBuffer
   *
   * @return A CCNxBuffer wraped around the byte array
   */
  Ptr<CCNxBuffer> CreateBuffer () const;

  /**
   * Two byte arrays are equal if they are the same length and byte-for-byte the same.
   *
   * lexicographically compares two byte arrays.
   *
   * @param other The byte array to compare against
   * @return true If same length and byte-for-byte equal
   * @return false Otherwise
   */
  bool operator == (const CCNxByteArray &other) const;

  /**
   * Two byte arrays are not equal if they are the different lengths or at least one byte differs.
   *
   * lexicographically compares two byte arrays.
   *
   * @param other The byte array to compare against
   * @return true If they are not equal
   * @return false If they are equal
   */
  bool operator != (const CCNxByteArray &other) const;

  /**
   * lexicographically compares two byte arrays.
   *
   * @param other The other byte array to compare with
   * @return true if this byte array is less than the other byte array
   * @return false If this byte arrays is not less then the other
   */
  bool operator < (const CCNxByteArray &other) const;

  /**
   * Output operator to hex print the byte array
   *
   * @param os The output stream
   * @param array The byte array to dump (must be non-null)
   * @return The updated output stream
   */
  friend std::ostream & operator << (std::ostream &os, CCNxByteArray const &array);

  typedef std::vector<uint8_t> StorageType;
  typedef StorageType::const_iterator IteratorType;

protected:
  StorageType m_bytes;

public:
  typedef std::allocator< uint8_t > allocator_type;

  /**
   * A const forward iterator for a byte array.
   *
   * Allows walking through the bytes via a C++ iterator
   */
  class const_iterator
  {
public:
    friend class CCNxByteArray;

    typedef const uint8_t const_reference;
    typedef const uint8_t * const_pointer;
    typedef std::forward_iterator_tag iterator_category;

    const_iterator (CCNxByteArray::IteratorType iter);
    const_iterator (const CCNxByteArray::const_iterator &);
    ~const_iterator ();

    const_iterator& operator= (const const_iterator&);
    bool operator== (const const_iterator&) const;
    bool operator!= (const const_iterator&) const;

    const_iterator& operator++ ();

    const_reference operator* () const;
    const_pointer operator-> () const;

protected:
    CCNxByteArray::IteratorType m_iter;
  };

  /**
   * Returns a forward const iterator representing the start of the array
   * @return An iterator
   */
  const_iterator begin () const;

  /**
   * Returns a forward const iterator representing the start of the array
   * @return An iterator
   */
  const_iterator cbegin () const;

  /**
   * Returns a forward const iterator representing the end of the array
   * @return An iterator
   */
  const_iterator end () const;

  /**
   * Returns a forward const iterator representing the end of the array
   * @return An iterator
   */
  const_iterator cend () const;

};

std::ostream & operator << (std::ostream &os, CCNxByteArray const &array);

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_MESSAGES_CCNX_BYTE_ARRAY_H_ */
