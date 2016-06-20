# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ################################################################################
# #
# # PATENT NOTICE
# #
# # This software is distributed under the BSD 2-clause License (see LICENSE
# # file).  This BSD License does not make any patent claims and as such, does
# # not act as a patent grant.  The purpose of this section is for each contributor
# # to define their intentions with respect to intellectual property.
# #
# # Each contributor to this source code is encouraged to state their patent
# # claims and licensing mechanisms for any contributions made. At the end of
# # this section contributors may each make their own statements.  Contributor's
# # claims and grants only apply to the pieces (source code, programs, text,
# # media, etc) that they have contributed directly to this software.
# #
# # There is no guarantee that this section is complete, up to date or accurate. It
# # is up to the contributors to maintain their portion of this section and up to
# # the user of the software to verify any claims herein.
# #
# # Do not remove this header notification.  The contents of this section must be
# # present in all distributions of the software.  You may only modify your own
# # intellectual property statements.  Please provide contact information.
#
# - Palo Alto Research Center, Inc
# This software distribution does not grant any rights to patents owned by Palo
# Alto Research Center, Inc (PARC). Rights to these patents are available via
# various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
# intellectual property used by its contributions to this software. You may
# contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org

import re

def substTestForModel(x): 
    # Only do this for paths that begin with 'model/'
	if (x.find('model/', 0) == -1):
		return None

	else:
		# substitude 'test' for 'model' and add 'test_' before the file name
		# for example 'model/node/ccnx-route.cc' will become 'test/node/test_ccnx-route.cc'.
		
		y = x.replace('model', 'test')
		m = re.search('(.*/)+(.+)', y)
		filename = m.group(2)
		z = y.replace(filename, 'test_' + filename)
		return z
		
def isNotNone(x):
	return x is not None

def build(bld):
    module = bld.create_ns3_module('ccns3Sim', ['core', 'network', 'virtual-net-device'])
    module.includes = '.'
    module.source = [
        'applications/ccnx-application.cc',
        'applications/producer-consumer/ccnx-consumer.cc',
        'applications/producer-consumer/ccnx-producer.cc',
        'applications/producer-consumer/ccnx-producer-helper.cc',
        'applications/producer-consumer/ccnx-consumer-helper.cc',
        'applications/producer-consumer/ccnx-content-repository.cc',
        #
        'model/messages/ccnx-buffer.cc',
        'model/messages/ccnx-byte-array.cc',
        'model/messages/ccnx-contentobject.cc',
        'model/messages/ccnx-fixedheader.cc',
        'model/messages/ccnx-interest.cc',
        'model/messages/ccnx-message.cc',
        'model/messages/ccnx-name.cc',
        'model/messages/ccnx-name-builder.cc',
        'model/messages/ccnx-namesegment.cc',
        'model/messages/ccnx-time.cc',
        'model/messages/ccnx-perhopheaderentry.cc',
        'model/messages/ccnx-cachetime.cc',
        'model/messages/ccnx-interestlifetime.cc',
        'model/messages/ccnx-perhopheader.cc',
        # Crypto
        'model/crypto/ccnx-crypto.cc',
        'model/crypto/ccnx-crypto-suite.cc',
        'model/crypto/ccnx-hasher.cc',
        'model/crypto/ccnx-hashing-algorithm.cc',
        'model/crypto/ccnx-hash-value.cc',
        'model/crypto/ccnx-key.cc',
        'model/crypto/ccnx-keyid.cc',
        'model/crypto/ccnx-signature.cc',
        'model/crypto/ccnx-signing-algorithm.cc',
        'model/crypto/ccnx-signer.cc',
        'model/crypto/ccnx-verifier.cc',
        'model/crypto/hashers/ccnx-hasher-fnv1a.cc',
 
        # Validation
        'model/validation/ccnx-validation.cc',
        # node
        'model/node/ccnx-connection.cc',
        'model/node/ccnx-connection-list.cc',
        'model/node/ccnx-l3-interface.cc',
        'model/node/ccnx-l3-protocol.cc',
        'model/node/ccnx-l4-protocol.cc',
        'model/node/ccnx-route.cc',
        'model/node/ccnx-route-entry.cc',
        'model/node/standard/ccnx-connection-device.cc',
        'model/node/standard/ccnx-connection-l4.cc',
        'model/node/standard/ccnx-standard-layer3.cc',
        'model/node/standard/ccnx-standard-layer3-helper.cc',
        # forwarding
        'model/forwarding/ccnx-forwarder.cc',
        'model/forwarding/ccnx-forwarder-message.cc',
        'model/forwarding/ccnx-content-store.cc',
        'model/forwarding/ccnx-null-content-store-factory.cc',
        'model/forwarding/ccnx-fib.cc',
        'model/forwarding/ccnx-pit.cc',
        'model/forwarding/standard/ccnx-standard-content-store.cc',
        'model/forwarding/standard/ccnx-standard-content-store-lruList.cc',
        'model/forwarding/standard/ccnx-standard-content-store-entry.cc',
        'model/forwarding/standard/ccnx-standard-content-store-factory.cc',
        'model/forwarding/standard/ccnx-standard-fib.cc',
        'model/forwarding/standard/ccnx-standard-fib-factory.cc',
        'model/forwarding/standard/ccnx-standard-fibEntry.cc',
        'model/forwarding/standard/ccnx-standard-pit.cc',
        'model/forwarding/standard/ccnx-standard-pit-factory.cc',
        'model/forwarding/standard/ccnx-standard-pitEntry.cc',
        'model/forwarding/standard/ccnx-standard-forwarder.cc',
        'model/forwarding/standard/ccnx-standard-forwarder-helper.cc',
        'model/forwarding/standard/ccnx-standard-forwarder-work-item.cc',
        # routing
        'model/routing/nfp/nfp-advertise.cc',
        'model/routing/nfp/nfp-anchor-advertisement.cc',
        'model/routing/nfp/nfp-computation-cost.cc',
        'model/routing/nfp/nfp-link-cost.cc',
        'model/routing/nfp/nfp-neighbor.cc',
        'model/routing/nfp/nfp-neighbor-key.cc',
        'model/routing/nfp/nfp-payload.cc',
        'model/routing/nfp/nfp-prefix.cc',
        'model/routing/nfp/nfp-prefix-timer-entry.cc',
        'model/routing/nfp/nfp-prefix-timer-heap.cc',
        'model/routing/nfp/nfp-seqnum.cc',
        'model/routing/nfp/nfp-stats.cc',
        'model/routing/nfp/nfp-routing-helper.cc',
        'model/routing/nfp/nfp-routing-protocol.cc',
        'model/routing/nfp/nfp-withdraw.cc',
        'model/routing/nfp/nfp-workqueue.cc',
        'model/routing/nfp/nfp-workqueue-entry.cc',
        #
        'model/routing/static/ccnx-static-routing-helper.cc',
        'model/routing/static/ccnx-static-routing-protocol.cc',
        'model/routing/ccnx-routing-protocol.cc',
        # packets
        'model/packets/ccnx-packet.cc',
        'model/packets/standard/ccnx-codec-registry.cc',
        'model/packets/standard/ccnx-codec-contentobject.cc',
        'model/packets/standard/ccnx-codec-fixedheader.cc',
        'model/packets/standard/ccnx-codec-interest.cc',
        'model/packets/standard/ccnx-codec-name.cc',
        'model/packets/standard/ccnx-tlv.cc',
        'model/packets/standard/ccnx-codec-perhopheaderentry.cc',
        #'model/packets/standard/ccnx-codec-perhopheader.cc',
        'model/packets/standard/ccnx-codec-interestlifetime.cc',
        'model/packets/standard/ccnx-codec-cachetime.cc',
        # Portal
        'model/portal/ccnx-portal.cc',
        'model/portal/ccnx-portal-factory.cc',
        'model/portal/message/ccnx-message-portal.cc',
        'model/portal/message/ccnx-message-portalfactory.cc',
        # Helpers
        'helper/ccnx-ascii-trace-helper.cc',
        'helper/ccnx-routing-helper.cc',
        'helper/ccnx-layer3-helper.cc',
        'helper/ccnx-forwarding-helper.cc',
        'helper/ccnx-stack-helper.cc',      
        ]

	# Create the list of unit test names from the list of model/*.cc names.  you must have
	# a unit test file for every .cc file under model/.
    module_test = bld.create_ns3_module_test_library('ccns3Sim')
    module_test.source = filter(isNotNone, map(substTestForModel, module.source))
    
    module_test.source += [
    	'test/applications/producer-consumer/test_ccnx-content-repository.cc',
    	'test/node/test_ccnx-delay-queue.cc',
    ]

    headers = bld(features='ns3header')
    headers.module = 'ccns3Sim'
    headers.source = [
        'applications/ccnx-application.h',
        'applications/producer-consumer/ccnx-consumer.h',
        'applications/producer-consumer/ccnx-producer.h',
        'applications/producer-consumer/ccnx-consumer-helper.h',
        'applications/producer-consumer/ccnx-producer-helper.h',
        'applications/producer-consumer/ccnx-content-repository.h',
        'model/messages/ccnx-buffer.h',
        'model/messages/ccnx-byte-array.h',
        'model/messages/ccnx-contentobject.h',
        'model/messages/ccnx-fixedheader.h',
        'model/messages/ccnx-perhopheaderentry.h',
        'model/messages/ccnx-interestlifetime.h',
        'model/messages/ccnx-cachetime.h',
        'model/messages/ccnx-perhopheader.h',
        'model/messages/ccnx-interest.h',
        'model/messages/ccnx-message.h',
        'model/messages/ccnx-name.h',
        'model/messages/ccnx-name-builder.h',
        'model/messages/ccnx-namesegment.h',
        'model/messages/ccnx-time.h',
        # Crypto
        'model/crypto/ccnx-crypto.h',
        'model/crypto/ccnx-crypto-suite.h',
        'model/crypto/ccnx-hasher.h',
        'model/crypto/ccnx-hashing-algorithm.h',
        'model/crypto/ccnx-hash-value.h',
        'model/crypto/ccnx-key.h',
        'model/crypto/ccnx-keyid.h',
        'model/crypto/ccnx-signature.h',
        'model/crypto/ccnx-signing-algorithm.h',
        'model/crypto/ccnx-signer.h',
        'model/crypto/ccnx-verifier.h',
        'model/crypto/hashers/ccnx-hasher-fnv1a.h',
        # Validation
        'model/validation/ccnx-validation.h',
        # node
        'model/node/ccnx-connection.h',
        'model/node/ccnx-connection-list.h',
        'model/node/ccnx-delay-queue.h',
        'model/node/ccnx-l3-interface.h',
        'model/node/ccnx-l3-protocol.h',
        'model/node/ccnx-l4-protocol.h',
        'model/node/ccnx-route.h',
        'model/node/ccnx-route-entry.h',
        'model/node/standard/ccnx-connection-device.h',
        'model/node/standard/ccnx-connection-l4.h',
        'model/node/standard/ccnx-standard-layer3.h',
        'model/node/standard/ccnx-standard-layer3-helper.h',
        
        # forwarding
        'model/forwarding/ccnx-content-store.h',
        'model/forwarding/ccnx-null-content-store-factory.h',
        'model/forwarding/ccnx-pit.h',
        'model/forwarding/ccnx-fib.h',
        'model/forwarding/ccnx-forwarder.h',
        'model/forwarding/ccnx-forwarder-message.h',
        'model/forwarding/standard/ccnx-standard-content-store.h',
        'model/forwarding/standard/ccnx-standard-content-store-lruList.h',
        'model/forwarding/standard/ccnx-standard-content-store-entry.h',
        'model/forwarding/standard/ccnx-standard-content-store-factory.h',
        'model/forwarding/standard/ccnx-standard-fib.h',
        'model/forwarding/standard/ccnx-standard-fib-factory.h',
        'model/forwarding/standard/ccnx-standard-fibEntry.h',
        'model/forwarding/standard/ccnx-standard-forwarder.h',
        'model/forwarding/standard/ccnx-standard-forwarder-helper.h',
        'model/forwarding/standard/ccnx-standard-forwarder-work-item.h',
        'model/forwarding/standard/ccnx-standard-pit.h',
        'model/forwarding/standard/ccnx-standard-pit-factory.h',
        'model/forwarding/standard/ccnx-standard-pitEntry.h',
        # routing
        'model/routing/nfp/nfp-advertise.h',
        'model/routing/nfp/nfp-anchor-advertisement.h',
        'model/routing/nfp/nfp-computation-cost.h',
        'model/routing/nfp/nfp-link-cost.h',
        'model/routing/nfp/nfp-message.h',
        'model/routing/nfp/nfp-neighbor.h',
        'model/routing/nfp/nfp-neighbor-key.h',
        'model/routing/nfp/nfp-payload.h',
        'model/routing/nfp/nfp-prefix.h',
        'model/routing/nfp/nfp-prefix-timer-entry.h',
        'model/routing/nfp/nfp-prefix-timer-heap.h',
        'model/routing/nfp/nfp-routing-helper.h',
        'model/routing/nfp/nfp-routing-protocol.h',
        'model/routing/nfp/nfp-schema.h',
        'model/routing/nfp/nfp-seqnum.h',
        'model/routing/nfp/nfp-stats.h',
        'model/routing/nfp/nfp-withdraw.h',
        'model/routing/nfp/nfp-workqueue.h',
        'model/routing/nfp/nfp-workqueue-entry.h',
        'model/routing/static/ccnx-static-routing-helper.h',
        'model/routing/static/ccnx-static-routing-protocol.h',
        'model/routing/ccnx-routing-error.h',
        'model/routing/ccnx-routing-protocol.h',
        # packets
        'model/packets/ccnx-packet.h',
        'model/packets/ccnx-type-registry.h',
        'model/packets/standard/ccnx-codec-registry.h',
        'model/packets/standard/ccnx-codec-contentobject.h',
        'model/packets/standard/ccnx-codec-fixedheader.h',
        'model/packets/standard/ccnx-codec-interest.h',
        'model/packets/standard/ccnx-codec-name.h',
        'model/packets/standard/ccnx-schema-v1.h',
        'model/packets/standard/ccnx-tlv.h',
        'model/packets/standard/ccnx-codec-perhopheaderentry.h',
        #'model/packets/standard/ccnx-codec-perhopheader.h',
        'model/packets/standard/ccnx-codec-interestlifetime.h',
        'model/packets/standard/ccnx-codec-cachetime.h',
        # Portal
        'model/portal/ccnx-portal.h',
        'model/portal/ccnx-portal-factory.h',
        'model/portal/message/ccnx-message-portal.h',
        'model/portal/message/ccnx-message-portalfactory.h',
        # Helpers
        'helper/ccnx-ascii-trace-helper.h',
        'helper/ccnx-routing-helper.h',
        'helper/ccnx-layer3-helper.h',
        'helper/ccnx-forwarding-helper.h',
        'helper/ccnx-stack-helper.h'

        ]


    if bld.env['ENABLE_EXAMPLES']:
        bld.recurse('examples')

    bld.ns3_python_bindings()
