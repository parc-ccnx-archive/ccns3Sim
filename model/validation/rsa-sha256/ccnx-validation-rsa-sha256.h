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

#ifndef CCNS3SIM_MODEL_VALIDATION_VALIDATION_RSA_SHA256_H_
#define CCNS3SIM_MODEL_VALIDATION_VALIDATION_RSA_SHA256_H_

#include "ns3/ccnx-validation.h"
#include "ns3/ccnx-key.h"
namespace ns3 {
namespace ccnx {

class CCNxValidationRsaSha256 : public CCNxValidation
{
public:
  static TypeId GetTypeId (void);

  /**
   * Creates a CCNxValidation based on RSA-SHA256 with a new keypair.
   * If CCNxCrypto::SetMode() is Simulated, then there are no real keys used.  If it is
   * set to Implemented, then we use an actual crypto system.
   *
   * The function is currently hard-coded to use a 2048 bit key.
   *
   * TODO: Allow multiple key sizes
   * @return
   */
  static Ptr<CCNxValidationRsaSha256> CreateWithNewKeypair ();

  CCNxValidationRsaSha256 (Ptr<const CCNxKey> privateKey, Ptr<const CCNxKey> publicKey);

  virtual ~CCNxValidationRsaSha256 ();

  /**
   * Returns the TypeId of the concrete class implementing CCNxValidation.  In this
   * case, it will be "ns3::ccnx::CCNxValidationRsaSha256".
   * @return
   */
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Returns a signer that uses this validator's keypair
   * @return
   */
  virtual Ptr<CCNxSigner> CreateSigner ();

  /**
   * Returns a verifier that uses this validator's keypair.
   * @return
   */
  virtual Ptr<CCNxVerifier> CreateVerifier ();

private:
  Ptr<const CCNxKey> m_privateKey;
  Ptr<const CCNxKey> m_publicKey;
};

}   /* namespace ccnx */
} /* namespace ns3 */

#endif /* CCNS3SIM_MODEL_VALIDATION_VALIDATION_RSA_SHA256_H_ */
