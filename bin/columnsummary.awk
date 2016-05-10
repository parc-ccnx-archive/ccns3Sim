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

# The input will be sorted in the final display order.
# Compute the average and stddev of coverage, per file.  Also compute
# the coverage based on total lines.
#
# Example Input
# ==============================
#   rta_Framework.c                        70.83% of  144 lines
#   rta_Framework_Commands.c               66.83% of  199 lines
#   rta_ProtocolStack.c                    66.31% of  279 lines
#   rta_Transport.c                         0.00% of  147 lines
# ==============================
#
# We should echo back all input lines in the input order, then print
# the statistifcs summary

BEGIN {
	coverage_sum = 0.0;
	coverage_avg = 0.0;
	coverage_var = 0.0;
	lines_total = 0;
	lines_covered = 0;
	row_count = 0;
    C_GREEN="\x1b[0;32m"
    C_RED="\x1b[0;31m"
    C_YELLOW="\x1b[1;33m"
    C_END="\x1b[0m"
    # color thresholds
    IS_GREEN = 90
    IS_YELLOW = 70
}

# Execute this block for every line
{
    percent = $2;
    lines = $3;

    if (percent > IS_GREEN) {
        my_color = C_GREEN
    } else if (percent > IS_YELLOW) {
        my_color = C_YELLOW
    } else {
        my_color = C_RED
    }

    # print the colorized line
    print my_color$0C_END

	# Coverage column is $2
	coverage = percent / 100.0;

	row_count++;

	# compute the variance of a running set of numbers
	# see wikipedia for the algorithm
	coverage_sum += coverage;
	old_average  = coverage_avg;
	old_var      = coverage_var;
	coverage_avg = old_average + (coverage - old_average) / row_count;
	coverage_var = old_var     + (coverage - old_average) * (coverage - coverage_avg);

	lines_total += lines;
	lines_covered += lines * coverage;
}

END {

	coverage_stdev = 0.0;
	if( row_count > 1 ) {
		coverage_stdev = sqrt(coverage_var / (row_count-1) );
	}

	avg_covered = 0.0;
	if (lines_total > 0) {
		avg_covered = lines_covered / lines_total;
	}

	printf("coverage avg %0.3f std %0.3f lines total %d covered %d avg %.3f file count %d\n",
		coverage_avg,
		coverage_stdev,
		lines_total,
		lines_covered,
		avg_covered,
        row_count);

}

