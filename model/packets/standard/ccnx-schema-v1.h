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

#ifndef CCNS3SIM_CCNXSCHEMAV1_H
#define CCNS3SIM_CCNXSCHEMAV1_H

#include <stdint.h>

namespace ns3 {
namespace ccnx {

/**
 * @ingroup ccnx-packet
 *
 * This class defines the TLV Type values for the V1 schema.
 *
 * These values come from the ICNRG research group document irtf-icnrg-ccnx-messages-01.txt.
 */
class CCNxSchemaV1
{
public:
  // hop-by-hop headers
  static const uint16_t T_INT_LIFE = 0x0001;
  static const uint16_t T_CACHE_TIME = 0x0002;
  static const uint16_t T_MSG_HASH = 0x0003;

  // top-level TLVs
  static const uint16_t T_INTEREST = 0x0001;
  static const uint16_t T_OBJECT = 0x0002;
  static const uint16_t T_VALALG = 0x0003;
  static const uint16_t T_VALSIG = 0x0004;

  // Message body
  static const uint16_t T_NAME = 0x0000;
  static const uint16_t T_PAYLOAD = 0x0001;
  static const uint16_t T_KEYID_REST = 0x0002;
  static const uint16_t T_HASH_REST = 0x0003;
  static const uint16_t T_PAYLDTYPE = 0x0005;
  static const uint16_t T_EXPIRY = 0x0006;

  // name segments
  static const uint16_t T_NAMESEG_NAME = 0x0001;
  static const uint16_t T_NAMESEG_IPID = 0x0002;
  static const uint16_t T_NAMESEG_CHUNK = 0x0010;
  static const uint16_t T_NAMESEG_VERSION = 0x0013;

  static const uint16_t T_NAMESEG_APP0 = 0x1000;
  static const uint16_t T_NAMESEG_APP1 = 0x1001;
  static const uint16_t T_NAMESEG_APP2 = 0x1002;
  static const uint16_t T_NAMESEG_APP3 = 0x1003;
  static const uint16_t T_NAMESEG_APP4 = 0x1004;

  // Payload type
  static const uint8_t T_PAYLOADTYPE_DATA = 0x00;
  static const uint8_t T_PAYLOADTYPE_KEY = 0x01;
  static const uint8_t T_PAYLOADTYPE_LINK = 0x02;
  static const uint8_t T_PAYLOADTYPE_MANIFEST = 0x3;

  // Validation fields
  static const uint16_t T_CRC32C = 0x0002;
  static const uint16_t T_HMAC_SHA256 = 0x0003;
  static const uint16_t T_RSA_SHA256 = 0x0006;

  static const uint16_t T_KEYID = 0x0009;
  static const uint16_t T_PUBLICKEY = 0x000B;
  static const uint16_t T_SIGTIME = 0x000F;
};
}
}
#endif //CCNS3SIM_CCNXSCHEMAV1_H
