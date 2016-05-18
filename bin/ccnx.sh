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

RUN_DIR=$(dirname "$0")
[[ "$(uname -s)" == "Linux" ]] && export CXXFLAGS="-std=c++11"
# ================================================================
# README
# ================================================================
# This script can be used to run various commands that are part of the ns-3
# build process. It is used in an attempt to simplify the various python file
# calls and includes some canned configurations that have been known to work.
#
# Typically, this file is symbolically linked in the same directory that
# contains the top-level waf file, but can also be run from anywhere up the
# three directory levels below the directory that contains the top-level waf
# file (NOTE: it is assumed that there is only one waf file in the project).
#
# Run './ccnx.sh help' to get a list of the supported functionality.

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

#echo "RUN_DIR:: [$RUN_DIR]"
#echo "THIS_DIR:: [$THIS_DIR]"

ccnx_help()
{
    EXIT_STATUS="$1"
    [[ "$EXIT_STATUS" == ""  ]] && EXIT_STATUS="0"

    A_BUILD="${C_WHITE}build${C_END}"
    A_CLEAN="${C_WHITE}clean${C_END}"
    A_CFG="${C_WHITE}config${C_END}"
    A_EX="${C_WHITE}example${C_END}"
    A_GCOV="${C_WHITE}gcov${C_END}"
    A_GCOV_CLEAN="${C_WHITE}gcov_clean${C_END}"
    A_LIST="${C_WHITE}list${C_END}"
    A_OPT="${C_WHITE}opt${C_END}"
    A_PURGE="${C_WHITE}debug_purge${C_END}"
    A_PYTHON="${C_WHITE}python${C_END}"
    A_REBUILD="${C_WHITE}rebuild${C_END}"
    A_TEST="${C_WHITE}test${C_END}"

    A_EXE="${C_WHITE}$0${C_END}"
    echo -e "
usage: $A_EXE [$A_LIST] [$A_CLEAN] [$A_CFG] [$A_BUILD] [$A_GCOV] [$A_OPT] [$A_REBUILD] [$A_PYTHON] [$A_TEST] [$A_EX <example_name>]

    $A_CLEAN           Runs './waf clean'
    $A_CFG          Runs './waf configure' or './waf configure --enable-gcov' if the
                    coverage argument is also provided. It is also possible to use
                    the optimized build profile with the $A_OPT argument (this will
                    add --build-profile=optimized). There is a default list of waf
                    enable/disable args that does not include python unless the
                    argument python is also provided
    $A_BUILD           Runs './waf build'
    $A_GCOV            Adds '--enable-gcov' to the configure command. Requires $A_CFG.
                    If the configuration has both $A_GCOV and $A_PYTHON enabled,
                    the build will fail.
                    You can do '$0 $A_REBUILD $A_GCOV to clean/config/build
                    for gcov.
    $A_OPT             Adds --build-profile=optimized to the configure command.
                    Requires $A_CFG. You can do '$0 $A_REBUILD $A_OPT to
                    clean/config/build
    $A_PYTHON          Used to run 'waf configure' with python enabled (i.e.
                    --disable-python is not used). Can not be combined with
                    gcov. $A_GCOV_CLEAN is implied when gcov arg is not provided.
    $A_REBUILD         Equivalent to '$0 $A_CLEAN $A_CFG $A_BUILD'. Can also provide the
                    $A_GCOV and/or $_A_OPT argument.
    $A_TEST            Run 'test.py'
    $A_EX <example_name>
                    Run the provided example name
    $A_LIST            List all of the available ccnx examples (can also do '$0 $A_EX $A_LIST')
    $A_GCOV_CLEAN      Remove all coverage .da (.gcda) and .gcov files. NOTE: .no (.gcno) files are
                       not removed. Use $A_PURGE to remove all coverage files and directories.
    $A_PURGE     Remove all coverage files and the ccnx-debug directory.

"

    exit "$EXIT_STATUS"
}

# main ()

CLEAN=0
CONFIG=0
BUILD=0
GCOV=0
OPTIMIZED=0
TEST=0
RUN_EXAMPLE=0
LIST=0
GET_EXAMPLE_NAME=0
EXAMPLE_NAME=""
PYTHON=0
GCOV_CLEAN=0
PURGE_DEBUG=0

if [ "$1" == ""  ]; then
    ccnx_help 0
fi

for arg in "$@"; do
    arg=$(echo "$arg" | sed -e 's/^--//;s/^-//')
    if [ "$GET_EXAMPLE_NAME" == "1" ]; then
        if [ "$arg" == "list" ]; then
            LIST=1
        else
            EXAMPLE_NAME="$arg"
        fi
        GET_EXAMPLE_NAME=0
        continue
    fi
    case $arg in
        "example")
            RUN_EXAMPLE=1
            GET_EXAMPLE_NAME=1
            ;;
        "list")
            LIST=1
            ;;
        "build")
            BUILD=1
            ;;
        "clean")
            CLEAN=1
            ;;
        "cov"|"gcov"|"coverage")
            GCOV=1
            ;;
        "config"|"configure"|"cfg")
            CONFIG=1
            ;;
        "opt"|"optimized")
            OPTIMIZED=1
            ;;
        "python")
            PYTHON=1
            ;;
        "rebuild")
            CLEAN=1
            CONFIG=1
            BUILD=1
            ;;
        "test")
            TEST=1
            ;;
        "gcov_clean")
            GCOV_CLEAN=1
            ;;
        "debug_purge")
            PURGE_DEBUG=1
            ;;
        "help")
            ccnx_help 0
            ;;
        *)
            echo "Don't know what to do with [$arg]!"
            ccnx_help 1
            ;;
    esac
done

# ================================================================
# verify_example_name()
# ================================================================
verify_example_name()
{
    CXX_EXAMPLES=$(get_ccnx_example_names)
    if [ "$RUN_EXAMPLE" == "1" ] && [ "$LIST" != "1" ]; then
        if [ "$EXAMPLE_NAME" == ""  ]; then
            echo -e "\n${C_RED}ERROR!${C_END} Please provide an example name!"
            ccnx_help 13
        else
            echo "$CXX_EXAMPLES" | grep "$EXAMPLE_NAME" > /dev/null 2>&1
            if [ $? == 1  ]; then
                echo -e "\nERROR! $0: example name [$EXAMPLE_NAME] is not valid!"
                list_ccnx_examples
                exit 13
            fi
        fi
    fi
}

# get to the correct dir before we run any of the following ... need to make sure that we pop out
# though...

if [ "$WAF_DIR" == "" ]; then
    echo "ERROR! WAF_DIR is not set (should be set by build-functions.sh call)."
    exit 1
fi

pushd "$WAF_DIR" > /dev/null

    [[ $GCOV == 0  ]] && [[ $CONFIG == 1 ]] && GCOV_CLEAN=1
    [[ $GCOV_CLEAN == 1  ]] && gcov_clean
    [[ $PURGE_DEBUG == 1  ]] && do_debug_purge

    [[ $CLEAN == 1  ]] && waf_clean

    # configuration options
    if [ $CONFIG == 1 ]; then
        if [ $PYTHON == 1  ] && [ $GCOV == 1 ]; then
            echo "ERROR! $0: cannot enable both python and gcov!"
            exit 13
        fi
        CONFIG_ARGS=""
        [ $PYTHON == 1 ] && CONFIG_ARGS+=" python"
        [ $GCOV == 1 ] && CONFIG_ARGS+=" gcov"
        [ $OPTIMIZED == 1 ] && CONFIG_ARGS+=" optimized"
        waf_config "$CONFIG_ARGS"
    fi

    if [ $BUILD == 1  ]; then
        waf_build_raw
    fi

    [[ $TEST == 1  ]] && test_all
    if [ "$RUN_EXAMPLE" == "1" ]; then
        if [ "$LIST" == 1 ]; then
            list_ccnx_examples
        else
            verify_example_name
            run_example "$EXAMPLE_NAME"
        fi
    elif [ "$LIST" == "1"  ]; then # list without the example keyword
        list_ccnx_examples
    fi

popd > /dev/null
