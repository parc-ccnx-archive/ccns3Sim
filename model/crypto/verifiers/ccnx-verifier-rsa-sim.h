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
 * # is up to the contributors to maintain their section in this file up to date
 * # and up to the user of the software to verify any claims herein.
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

#ifndef CCNS3SIM_MODEL_CRYPTO_VERIFIERS_CCNX_VERIFIER_RSA_SIM_H_
#define CCNS3SIM_MODEL_CRYPTO_VERIFIERS_CCNX_VERIFIER_RSA_SIM_H_

#include "ns3/ccnx-verifier.h"
#include "ns3/ccnx-key.h"

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-crypto
 *
 */
class CCNxVerifierRsaSim : public CCNxVerifier
{
  /**
   * These can only be created from the factory.  The constructor is private.
   */
  friend class CCNxVerifierRsaFactory;

public:
  static TypeId GetTypeId ();

  /**
   * Do not call this directly.  Use the CCNxVerifierRsaFactory.
   *
   * We would normally make this protected, but the way NS3 uses ns3::CreateObject<>() does not
   * allow us to make the constructor private in a way that only the factory can call.
   *
   * @param privateKey
   * @param publicKey
   */
  CCNxVerifierRsaSim ();

  virtual ~CCNxVerifierRsaSim ();

  /**
   * Adds a key to the verifier.  It will go in a dictionary for use by Verify().  Note that this does not
   * imply trust.  It will only imply that Verify() was able to mechanically verify the signature.
   *
   * @param [in] key A key to use for verification.
   * @return true If the key was added (unique KeyId)
   * @return false If they key was not added (duplicate KeyId)
   */
  virtual bool AddKey (Ptr<const CCNxKey> key);

  /**
   * Returns the TypeId of the implementation class
   * @return
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * Get the hash function to use to compute the signature.
   * Run all the bytes to be signed through this hasher, then call SignDigest()
   * on the output of the hasher.
   *
   * @return
   */
  virtual Ptr<CCNxHasher> GetSignatureHasher ();

  /**
   * Typical usage: Receive a packet with a CryptoSuite.  Get a CCNxVerifier that corresponds to that cryptosuite
   * (e.g. CCNxValidationRsaSha256) and get a verifier out of it (a derived class of this class).
   * Hash the protected region of the packet with that hasher.  Then, call this function to verify that
   * the signature in the packet actually works.
   *
   * @param [in] keyid The KeyId to use (either from the packet or derived some other way)
   * @param [in] computedDigest The digest we locally computed
   * @param [in] packetCryptoSuite The CryptoSuite specified by the packet
   * @param [in] packetSignature The signature in the packet to verify
   * @return
   */
  virtual bool Verify (Ptr<CCNxHashValue> keyid, Ptr<CCNxHashValue> computedDigest,
                       CCNxCryptoSuite packetCryptoSuite,
                       Ptr<CCNxSignature> packetSignature);

protected:
  Ptr<const CCNxKey> m_publicKey;

};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_CRYPTO_VERIFIERS_CCNX_VERIFIER_RSA_SIM_H_ */
