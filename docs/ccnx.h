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

#ifndef CCNX_H
#define CCNX_H

/**
 * \defgroup ccnx CCNx 1.0 Protocol
 *
 * \brief CCNx protocols (layer3, layer4, applications, routing)
 *
 * The CCNx 1.0 protocol is defined by the IRTF ICNRG Research Group documents:
 *  irtf-icnrg-ccnx-semantics-01.txt
 *  irtf-icnrg-ccnx-messages-01.txt
 *
 *  Marc Mosko (marc.mosko@parc.com)
 *
 * \section Code Overview
 *
 * @section Code modularity
 *
 * The CCNx code in NS3 is designed around the NS3 run time type identification system
 * (`ns3::Object`), so it is easy for a user to write their own implementations of
 * major function blocks and substitute them at run time via the simulation script
 * without needing to change any code in the ccns3Sim distribution.
 *
 * ... FINISH ...
 *
 * @section Modeling processing delays
 *
 * @image html delay-queue-4.png
 *
 * The CCNx code model includes processing delays at several stages in the packet pipeline.  This
 * allows fairly accurate simulation of processing delays due to the different data structures.
 * Each of these modules uses an asynchronous message passing API and callback to allow
 * arbitrary message delays based on the internal implementation details of each module.
 *
 * In the list below, we describe the modules where adding delay is already wired in and
 * how that delay is modeled.  It is straight forward for a user to change the values
 * of the delay equation from the configuration script or to implement their own data structures
 * and use their own delay formulas.
 *
 *    - `CCNxStandardLayer3`: This module is the layer 3 switching fabric for CCNx.  There is
 *      not much computation here, and the layer delay is usually a small constant.
 *
 *    - `CCNxStandardForwarder`: This module is the forwarder, which is responsible for all the
 *      high-cost data lookup.  The layer delay here is usually a small constant and the data
 *      structure delays are in each data structure.
 *
 *    - `CCNxStandardPit`: This module represents the Pending Interest Table.  The delay here
 *      is modeled as a linear function of the `CCNxName` length.
 *
 *    - `CCNxStandardFib`: This module represents the Forwarding Information Base.  The delay here
 *      is modeled as a linear function of the number of name components.
 *
 *    - `CCNxStandardContentStore`: This module represents the in-forwarder content store (cache).
 *      The delay here is modeled as a linear function of the `CCNxName` length.
 *
 *    - `CCNxMessagePortal`: This module is the Layer 4 protocol that interfaces to a user
 *      application.  The layer delay is a linear function of the total packet length.
 *
 * @see CCNxStandardLayer3, CCNxStandardForwarder, CCNxPit, CCNxFib, CCNxContentStore, CCNxMessagePortal
 *
 * \subsection Layer 3 Protocol
 *
 *
 * \subsection Layer 4 Protocols (Portals)
 *
 * \subsection CCNx Message representations
 *
 * \subsection CCNx Packet representation
 *
 * \subsection Routing model
 *
 * \subsection Forwarder model
 *
 * \subsection Applications
 *
 */

#endif
