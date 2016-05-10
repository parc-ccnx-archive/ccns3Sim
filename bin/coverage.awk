#!/usr/bin/awk -f
#
# Copyright (c) 2014-2016, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Patent rights are not granted under this agreement. Patent rights are
#       available under FRAND terms.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# @author Marc Mosko, Palo Alto Research Center (PARC)
# @copyright 2014-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
#

# Process the output of gcov.  The idea is that we have to reduce 3 lines of output to a
# single line
#
# EXAMPLE INPUT
# =====================================================
# brewer:test mmosko$ gcov test_metis_UdpListener.c
# File 'test_metis_UdpListener.c'
# Lines executed:94.53% of 128
# test_metis_UdpListener.c:creating 'test_metis_UdpListener.c.gcov'
#
# File '/usr/include/libkern/i386/_OSByteOrder.h'
# Lines executed:100.00% of 2
# /usr/include/libkern/i386/_OSByteOrder.h:creating '_OSByteOrder.h.gcov'
#
# File './../metis_UdpListener.c'
# Lines executed:70.94% of 117
# ./../metis_UdpListener.c:creating 'metis_UdpListener.c.gcov'
# =====================================================
#
# For each tuple (^File, ^Lines, .*) we want to extract the file name from the
# first line, then the % coverage and number of lines from the second line.
# We also want to filter out the test_* files and the system files (i.e. _OSByteOrder.h)
# We do not sort anything, that is the job of whoever calls us
#
# EXAMPLE OUTPUT
# =====================================================
# metis_UdpListener.c                  70.94% of  117 lines
# =====================================================
#
# The 37 space justification is based on the longest filename we have.


BEGIN {
	FS="[\': ]+"
	print_line = 0;
}

/^File/ {
	# token $2 is the file with a bunch of crap infront, e.g. "./../cpi_InterfaceIPTunnel.c"
	# so replace the longest prefix up to a "/" with empty string
	sub(/.*\//,"",$2);

	# Skip filenames that begin with underscore _ or with "test"
	if( match($2,/^((test)|(_))/) == 0 ) {
		printf("%-37s ", $2);
		print_line = 1;
	} else {
		print_line = 0;
	}
}

/^Lines/ {
	if( print_line > 0 ) {
		printf("%7s of %4d lines\n", $3, $5);
	}
	print_line = 0;
}

