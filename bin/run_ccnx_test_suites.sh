#!/bin/bash
#
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

clean_exit()
{
    echo "Exiting..."
    exit -1
}
trap clean_exit SIGINT SIGTERM

RUN_DIR=$(dirname "$0")
export RUN_DIR

CCNX_DBG_DIR_NAME=ccnx-debug
CCNX_DBG_LOG="./$CCNX_DBG_DIR_NAME/CCNX_TEST_SUITES.log"
COVERAGE_REPORT="./$CCNX_DBG_DIR_NAME/CCNX_TEST_SUITES_COVERAGE.log"
SIMPLE_GCOV_REPORT="./$CCNX_DBG_DIR_NAME/CCNX_TEST_SUITES_SIMPLE_GCOV_REPORT.log"
FILTERED_SIMPLE_GCOV_REPORT="./$CCNX_DBG_DIR_NAME/FILTERED_CCNX_TEST_SUITES_SIMPLE_GCOV_REPORT.log"
# ================================================================
# README
# ================================================================
# This script can be used to run the unit test suites for the ccnx code.
#
# You can provide the argument 'coverage' to add HTML coverage reports
# generated from lcov/genhtml. A coverage report will be run for each
# test suite and stored in a directory with the name of the test suite
# under the directory $CCNX_DBG_DIR_NAME
#
# NOTE: To utilize the coverage option, you need to have code that was
#       compiled for gcov (e.g. ./ccnx.sh rebuild gcov).
#
# Typically, this file is symbolically linked in the same directory that
# contains the top-level waf file, but can also be run from anywhere up the
# three directory levels below the directory that contains the top-level waf
# file (NOTE: it is assumed that there is only one waf file in the project).
#
# FIXME:: this script is not yet complete, but works to some extent.
#

#BANNER="================================================================"

# Determine the directory of the source file.
READLINK=$(readlink "$0") > /dev/null 2>&1
if [ $? == 1  ]; then
    if [ "$RUN_DIR" == "./bin"  ]; then
        THIS_DIR="${PWD}/bin"
    elif [ "$(basename "$PWD")" == "." ] || [ "$(basename "$PWD")" == "bin"  ]; then
        THIS_DIR="${PWD%/*}/bin"
    else
        THIS_DIR=$RUN_DIR
    fi
else
    THIS_DIR=$(dirname "$READLINK")
fi
export CCNX_BIN_DIR=$THIS_DIR
# shellcheck source=./build-functions.sh
source "${THIS_DIR}/build-functions.sh"

# ================================================================
# remove_non_ccnx_gcda_files()
# ================================================================
remove_non_ccnx_gcda_files()
{
    GCDA_FILES=$(/usr/bin/find . -name '*.gcda')
    for f in $GCDA_FILES; do
        # for now, just remove non-ccnx files ... this will be done prior to a
        # coverage report
        echo "$f" | grep -i "$REPO_DIR_NAME" > /dev/null
#        if [ $? == 0 ]; then
#            echo "Skipping $REPO_DIR_NAME file [$f]..."
#        else
        if [ $? == 1 ]; then
            if [ -f "$f" ]; then
                /bin/rm -f "$f"
                if [ $? != 0 ]; then
                    echo "ERROR! problem deleting [$f]"
                    exit 1
                fi
            else
                echo "WARNING! [$f] not found!"
            fi
        fi
    done
}
# ================================================================
# check_for_lcov()
# ================================================================
check_for_lcov()
{
    which lcov > /dev/null
    if [ $? != 0 ]; then
        echo "ERROR! lcov is not available. Please install lcov then rerun..."
        exit 1
    fi
    if [ "$(uname -s)" == "Linux" ]; then
        LCOV=/usr/bin/lcov
        GENHTML=/usr/bin/genhtml
    else
        LCOV=/usr/local/bin/lcov
        GENHTML=/usr/local/bin/genhtml
    fi
}

# ================================================================
# show_help()
# ================================================================
# note: colors are exported from build-functions.sh
show_help()
{
    A_CCNX="${C_WHITE}ccnx${C_END}"
    A_NS3="${C_WHITE}ns3${C_END}"
    A_LIST="${C_WHITE}list${C_END}"
    A_COV="${C_WHITE}coverage${C_END}"
    A_LCOV="${C_WHITE}lcov${C_END}"
    A_DET="${C_WHITE}detailed${C_END}"
    A_FULL="${C_WHITE}full${C_END}"
    A_SUITE="${C_WHITE}suite${C_END}"
    A_DBG="${C_WHITE}debug${C_END}"
    A_QUIET="${C_WHITE}quiet${C_END}"
    A_NOTE="${C_WHITE}NOTE${C_END}"

    B_ALL="${C_WHITE}all${C_END}"
    B_REPO_DIR_NAME="${C_BLUE}${REPO_DIR_NAME}${C_END}"
    echo -e "

usage: $C_WHITE$0$C_END [$A_CCNX] [$A_COV] [$A_DET|$A_LCOV] [$A_NS3] [$A_FULL] [$A_LIST] [$A_QUIET] [$A_SUITE TestSuiteName] [$A_DBG TestSuiteName]

    $A_CCNX        Run all $B_REPO_DIR_NAME testcases that are available

    $A_COV    Include coverage reporting. Can alternatively provide
                the $A_DET or $A_LCOV argument to do lcov reporting
                in addition to a simple summary of coverage.

                $A_NOTE: In order to run with coverage, the code needs to be
                      compiled with coverage enabled, which means that
                      the waf configuration needs to have --enable-gcov.
                      An easy way to clean, configure, and build the code
                      with coverage is to do the following:
                      ${C_WHITE}./ccnx.sh rebuild gcov${C_END}

    $A_DET    Do the detailed coverage reporting (i.e. both lcov
                and simple reporting). This overrides the $A_COV
                argument. Can alternatively provide the argument $A_LCOV.

                $A_NOTE: The code needs to be compiled with coverage enabled. See
                      the note for $A_COV above for more information.

    $A_LCOV        Use lcov for coverage reporting. Can alternatively provide
                the equivalent argument $A_DET.

                $A_NOTE: The code needs to be compiled with coverage enabled. See
                      the note for $A_COV above for more information.

    $A_NS3         Run './test.py', which runs $B_ALL of the testcases (i.e. not
                $B_REPO_DIR_NAME specific).

    $A_FULL        Include all gcov files for all .o files. Default is to only
                include the $B_REPO_DIR_NAME files.

    $A_LIST         List all of the $B_REPO_DIR_NAME test suites that are found.

    $A_QUIET       Turn off the --verbose flag for all test runs. Note that
                the verbose (or not) information is available in
                $CCNX_DBG_LOG after each run of
                $0.

    $A_SUITE TestSuiteName
                Run a specific $B_REPO_DIR_NAME test suite. Argument is of the form:
                '$A_SUITE <TestSuiteName>'. Can provide the argument multiple
                times. See also '$A_DBG'.

    $A_DBG TestSuiteName
                Run a specific $B_REPO_DIR_NAME test suite in debug mode.

"
    exit 0
}

# ================================================================
# main()
# ================================================================

if [ "$1" == "" ]; then
    show_help
fi

DO_LCOV=0
DO_COMBINED=0 # completely skip for now
FULL=0
LIST=0
GET_SUITE=0
RUN_SUITE=0
DEBUG_SUITE=0
RUN_ALL_CCNX=0
ALL_NS3_TESTCASES=0
VERBOSE=" --verbose"
GEN_SIMPLE_COVERAGE_REPORT=0
SUITE_TO_RUN=""

for arg in "$@"; do
    # just in case
    arg=$(echo "$arg" | sed -e 's/^--//')
    if [ $GET_SUITE == 1 ]; then
        SUITE_TO_RUN+=" $arg"
        GET_SUITE=0
        continue
    fi
    case $arg in
        "ccnx") # just list the test suites
            RUN_ALL_CCNX=1
            ;;
        "full") # don't prune to just ccnx ...
            echo "Gathering info from complete simulation environment (not pruning for ccnx)..."
            FULL=1
            ;;
        "help")
            show_help
            ;;
        "list") # just list the test suites
            LIST=1
            ;;
        "simple"|"cov"|"coverage") # simple coverage report
            check_for_lcov
            echo "Running with coverage ..."
            GEN_SIMPLE_COVERAGE_REPORT=1
            ;;
        "detailed"|"lcov") # detailed and simple coverage reporting
            check_for_lcov
            echo "Running with lcov (detailed) coverage..."
            DO_LCOV=1
            GEN_SIMPLE_COVERAGE_REPORT=1
            ;;
        "ns3")
            ALL_NS3_TESTCASES=1
            ;;
        "suite") # just run a single test suite
            RUN_SUITE=1
            GET_SUITE=1
            ;;
        "debug") # run a single test suite with gdb or lldb
            DEBUG_SUITE=1
            GET_SUITE=1
            ;;
        "quiet") # turn off the --verbose flag for all test runs.
            VERBOSE=""
            ;;
        *)
            echo "ERROR! unknown argument [$arg]."
            show_help
            ;;
    esac
done

if [ "$WAF_DIR" == "" ]; then
    echo "ERROR! WAF_DIR is not set (should be set by build-functions.sh call)."
    exit 1
fi
pushd "$WAF_DIR" > /dev/null
mkdir -p "$CCNX_DBG_DIR_NAME"

# ================================================================
# show_test_results()
# ================================================================
show_test_results()
{

    RESULT="$(tail -3 $CCNX_DBG_LOG | grep ' of 1 test')"
    PASS_REGEX="0 skipped, 0 failed, 0 crashed, 0 valgrind errors"
    echo $RESULT | grep "$PASS_REGEX" > /dev/null 2>&1
    if [ $? == 0 ]; then
        DISP="${C_GREEN}${RESULT}${C_END}"
    else
        DISP="${C_RED}${RESULT}${C_END}"
    fi
    echo -e "    :: $DISP"
}
# ================================================================
# run_single_suite()
# ================================================================
run_single_suite()
{
    SUITE=$1
    echo -e "\n\n[$SUITE]\n" >> "$CCNX_DBG_LOG" 2>&1
    if [ "$SUITE" == "" ]; then
        ./test.py $VERBOSE >> "$CCNX_DBG_LOG" 2>&1
    else
        #echo "RUNNING SUITE [$SUITE]"
        #echo "VERBOSE:: [$VERBOSE]"
        ./test.py $VERBOSE --suite "$SUITE" >> "$CCNX_DBG_LOG" 2>&1
    fi
    # print the result
    show_test_results
}

# ================================================================
# debug_single_suite()
# ================================================================
debug_single_suite()
{
     SUITE=$1
     echo -e "\n\n[$SUITE (debug)]\n" >> "$CCNX_DBG_LOG" 2>&1
    if [ "$(uname -s)" == "Linux" ]; then
        ./waf --run test-runner --command-template="gdb -ex 'set args --suite=${SUITE} $VERBOSE' --args %s"
    else
        ./waf --run test-runner --command-template="lldb -- %s --suite=${SUITE} $VERBOSE"
    fi
    # print the result
    show_test_results
}

# ================================================================
# do_coverage() :: run the coverage reports.
#
# NOTE: It is assumed that this is only called when coverage is
#       given as a command line argument.
# ================================================================
do_coverage()
{
    SUITE=$1
    # capture the test report
    echo "    ==> Running lcov coverage reports for [${SUITE}]..."
    LCOV_TRACEFILE="./$CCNX_DBG_DIR_NAME/${SUITE}-lcov.info"
    ODIR="./$CCNX_DBG_DIR_NAME/${SUITE}"
    # remove the non-ccnx gcda files to cut down on processing time and data...
    if [ $FULL == 0 ]; then
        remove_non_ccnx_gcda_files
    fi
    ${LCOV} --directory . --capture --output-file "$LCOV_TRACEFILE" > /dev/null 2>&1
    ${GENHTML} --output-directory "$ODIR" "$LCOV_TRACEFILE" > /dev/null 2>&1
    LCOV_TRACEFILE_ADD+=" --add-tracefile $LCOV_TRACEFILE"
    #echo -e "\n\n${BANNER}\n${s}\n${BANNER}" >> $COVERAGE_REPORT
}

# ================================================================
# log_simple_coverage(): simple gcov reporting
# ================================================================
log_simple_coverage()
{
    if [ ! -d ./build ] || [ ! -f ./waf ]; then
        echo "$0: ERROR! [$PWD] isn't the dir with waf or ./build/ is missing ..."
        exit 1
    fi
    pushd build > /dev/null 2>&1
    /usr/bin/find . -name '*.gcda' | xargs gcov -a \
                    | ../src/"${REPO_DIR_NAME}"/bin/coverage.awk \
                    | ../src/"${REPO_DIR_NAME}"/bin/remove_duplicates.awk \
                    | sort -k2,2n -k1,1 \
                    | ../src/"${REPO_DIR_NAME}"/bin/columnsummary.awk > "../$SIMPLE_GCOV_REPORT"

    popd > /dev/null 2>&1
}

report_simple_coverage()
{
    echo -e "Line coverage report:\n
filename(s)                                 %     lines
--------------------------------------    -----   -------"
    TMPFILE=./.simple-coverage-report.temp
    [[ -f "$TMPFILE" ]] && /bin/rm -f "$TMPFILE"
    for s in $SUITES; do
        grep "${s}\." "$SIMPLE_GCOV_REPORT" >> $TMPFILE
    done
    ./src/"${REPO_DIR_NAME}"/bin/columnsummary.awk < "$TMPFILE" > "$FILTERED_SIMPLE_GCOV_REPORT"
    cat "$FILTERED_SIMPLE_GCOV_REPORT"
    /bin/rm "$TMPFILE"
    echo -e "\nThe report above is available in: $FILTERED_SIMPLE_GCOV_REPORT"
    echo -e "For a report of all files do: cat $SIMPLE_GCOV_REPORT\n"
}

# TODO: Do we want to keep a running log or keep rewriting as below???
/bin/date > $CCNX_DBG_LOG

# restart the coverage report log file
if [ $DO_LCOV == 1 ] || [ $GEN_SIMPLE_COVERAGE_REPORT == 1 ]; then
    /bin/date > $COVERAGE_REPORT
fi

# specify the correct test suites to run
TC_NAME_REGEX=""
ALL_SUITES=""
populate_all_suites()
{
     if [ "$ALL_SUITES" == "" ]; then
        echo "Building a list of all $REPO_DIR_NAME test suites..."
        TC_NAME_REGEX="ccn|nfp|static-routing-helper|static-routing-protocol|validation-rsa-sha256"
        ALL_SUITES="$(./test.py --list|egrep -i "$TC_NAME_REGEX" | egrep -vi 'waf|buildings|click' | awk '{print $2}')"
    fi
}
verify_test_suite_names()
{
     SUITES=$1
     S_ERR=0
     NAMES=""
     populate_all_suites
     echo "Verifying test suite names (only $REPO_DIR_NAME test suites expected)..."
     for s in $SUITES; do
         echo "$ALL_SUITES" | grep "$s" > /dev/null 2>&1
         if [ $? == 1  ]; then
             S_ERR=1
             NAMES+=" $s"
         fi
     done
     if [ $S_ERR == 1 ]; then
         echo "$0: ERROR! bad test suite name(s):"
         echo "    $NAMES"
         exit 13
     fi
}
if [ $RUN_SUITE == 1 ] || [ $DEBUG_SUITE == 1 ]; then
    SUITES="$SUITE_TO_RUN"
    verify_test_suite_names "$SUITES"
elif [ $ALL_NS3_TESTCASES == 1 ]; then
    SUITES=""
else # need this to be this way for the 'list' argument
    populate_all_suites
    SUITES="$ALL_SUITES"
fi
CNT=$(echo "$SUITES" | wc -w)
CNT=$(echo "$CNT" | sed -e s/\ //g)
IDX=1
LCOV_TRACEFILE_ADD=""

# ================================================================
# list_testcases()
# ================================================================
list_testcases()
{
    echo -e "\n$CNT known testcases:\n"
    for s in $SUITES; do
        echo "    $s"
    done
    echo ""
}

if [ $LIST == 1 ]; then
    list_testcases
    exit 0
fi

# For now, we'll zero the counters at the beginning of the run, not for each test suite that is run.
# This is so that we get s full summary of all of the test suites that were run. It is assumed that
# if we want the results for a single test suite, then we need to add some other option to do so ...
# and then we won't want the summary report at the end, because it will only include the results
# from the last test suite run.

if [ $DO_LCOV == 1 ]; then
    ${LCOV} --directory . --zerocounters > /dev/null 2>&1
fi

# arg checking and default run
if [ $ALL_NS3_TESTCASES == 1 ]; then
    [[ $RUN_SUITE == 1 ]] && echo "ERROR! Cannot specify both default and suite" && exit 13
    [[ $DEBUG_SUITE == 1 ]] && echo "ERROR! Cannot specify both default and debug" && exit 13
    [[ $RUN_ALL_CCNX == 1 ]] && echo "ERROR! Cannot specify both default and all or ccnx" && exit 13
    # if we get this far, run the default ...
    echo "Running all test suites..."
    run_single_suite ""
    if [ $DO_LCOV == 1 ]; then
        do_coverage "ns3"
    fi
fi

# this won't run for the default case
for s in $SUITES; do
    echo "Running ($IDX of $CNT) [$s]..."
    let "IDX=$IDX+1"
    if [ $DEBUG_SUITE == 1 ]; then
        debug_single_suite "$s"
    else
        run_single_suite "$s"
    fi
    if [ $DO_LCOV == 1 ]; then
        do_coverage "$s"
    fi
done

if [ $DO_LCOV == 1 ] && [ $DO_COMBINED == 1 ]; then
    # combine all of the tracefiles into one
    COMBINED_TRACEFILE="$CCNX_DBG_DIR_NAME/full-lcov-tracefile.info"
    echo "Generating the combined tracefile results"
    #echo "LCOV_TRACEFILE_ADD:: [$LCOV_TRACEFILE_ADD]"
    ${LCOV}$LCOV_TRACEFILE_ADD -o "$COMBINED_TRACEFILE"
    ${GENHTML} --output-directory "./$CCNX_DBG_DIR_NAME/combined-results" "$COMBINED_TRACEFILE" > /dev/null 2>&1
fi

# Generate the simple coverage report at the end ... before we report the index.html file locations,
# etc.
if [ $GEN_SIMPLE_COVERAGE_REPORT == 1 ]; then
    add_to $COVERAGE_REPORT "Gathering data for a simple coverage report..."
    echo -e "\n"
    if [ $FULL == 0 ]; then
        remove_non_ccnx_gcda_files
    fi
    log_simple_coverage
    report_simple_coverage
fi

# indicate the top-level lcov coverage report HTML files
if [ $DO_LCOV == 1 ]; then
    echo -e "\n\nTop-level lcov reports can be found here:\n"
    for s in $SUITES; do
        echo "    ./$CCNX_DBG_DIR_NAME/$s/index.html"
    done
    echo -e "\n"
fi

# TODO: we need to add some status information that includes pointers to the generated index.html
# files.

popd > /dev/null
