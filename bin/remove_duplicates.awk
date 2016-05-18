#!/usr/bin/awk -f
#
# Copyright (c) 2014-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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

# process the coverage.awk script output.  coverage.awk is called per
# directory, so there might be duplicate keys because some source modules are included
# in more than one test, often as an auxiliary module to get at some inner code for testing.
# We need to remove duplicates and only report the higest percentage coverage
#
# Example Input
# ==============================
#   rta_ConnectionTable.c                  98.85% of   87 lines
#   rta_Framework.c                        70.83% of  144 lines
#   rta_Framework.c                        76.39% of  144 lines
#   rta_Framework_Commands.c               37.69% of  199 lines
#   rta_Framework_Commands.c               65.33% of  199 lines
#   rta_Framework_Commands.c               66.83% of  199 lines
#   rta_ProtocolStack.c                     2.87% of  279 lines
#   rta_ProtocolStack.c                    63.80% of  279 lines
#   rta_ProtocolStack.c                    66.31% of  279 lines
#   rta_Transport.c                         0.00% of  147 lines
# ==============================
#
# The input will not necessarily be sorted.
# The output does not need to be sorted.

BEGIN {
}

# Execute this block for every line
{
    key = $1;
    percent = $2 + 0;
    lines = $4;

    #printf("key %s percent %d lines %s (%s)\n", key, percent, lines, percent_key[key]);

    # Save each new key, but only update existing keys if we have a larger percentage
    if (have_key[key] == 0 || percent_key[key] < percent) {
        have_key[key] = key;
        percent_key[key] = percent;
        lines_key[key] = lines;
    }
}

END {
    for(key in have_key) {
        printf("%-38s %8s %5d\n",  key, percent_key[key], lines_key[key]);
    }
}

