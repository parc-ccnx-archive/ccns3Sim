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

# ================================================================
# README
# ================================================================
# This script provides various functions that are used primarily by ./ccnx.sh. In fact,
# if this script is not called from ccnx.sh some of the functionality like 'list' will
# not work.
#
# NOTE: It is assumed that the functions from this script are run in the same directory as the
# top-level waf file. This is taken care of in ./ccnx.sh with a pushd/popd wrapper around the
# function calls.

# Set the name of the repo directory name, just in case it changes in the
# future. We get the environment variable CCNX_BIN_DIR from ccnx.sh. This
# is primarily for the 'list' argument.

# make sure that we have the necessary environment variables defined
if [ "$RUN_DIR" == "" ] || [ "$CCNX_BIN_DIR" == "" ]; then
    echo "ERROR! Both RUN_DIR [$RUN_DIR] and CCNX_BIN_DIR [$CCNX_BIN_DIR] must be defined by the calling entity!"
    exit 1
fi

REPO_DIR_NAME=$(basename "${CCNX_BIN_DIR%/*}")
export REPO_DIR_NAME

# [20160328] A recent Xcode upgrade broke the NS3 compile, so we need to adjust the configure
# command until this is fixed.
NS3_XCODE_COMPILE_BUGS_FIXED=0

# ================================================================
# define some colors
# ================================================================
# From: http://unix.stackexchange.com/questions/43408/printing-colored-text-using-echo -e
#
#   "\e[1;31m This is red text \e[0m"
#
#   Black       0;30     Dark Gray     1;30
#   Blue        0;34     Light Blue    1;34
#   Green       0;32     Light Green   1;32
#   Cyan        0;36     Light Cyan    1;36
#   Red         0;31     Light Red     1;31
#   Purple      0;35     Light Purple  1;35
#   Brown       0;33     Yellow        1;33
#   Light Gray  0;37     White         1;37

export C_BLACK="\x1b[0;30m"
export C_BLUE="\x1b[0;34m"
export C_GREEN="\x1b[0;32m"
export C_CYAN="\x1b[0;36m"
export C_RED="\x1b[0;31m"
export C_PURPLE="\x1b[0;35m"
export C_BROWN="\x1b[0;33m"
export C_LIGHT_GRAY="\x1b[0;37m"

export C_DARK_GRAY="\x1b[1;30m"
export C_LIGHT_BLUE="\x1b[1;34m"
export C_LIGHT_GREEN="\x1b[1;32m"
export C_LIGHT_CYAN="\x1b[1;36m"
export C_LIGHT_RED="\x1b[1;31m"
export C_LIGHT_PURPLE="\x1b[1;35m"
export C_YELLOW="\x1b[1;33m"
export C_WHITE="\x1b[1;37m"

export C_END="\x1b[0m"

# ================================================================
# find_waf_dir()
# ================================================================
WAF_DIR=$RUN_DIR
UP_COUNT=3
IDX=0
WAF_DIR_FOUND=0
find_waf_dir()
{
    #echo "find_waf_dir() called"
    if [ -f "${WAF_DIR}/waf" ]; then
        WAF_DIR_FOUND=1
        #echo "FOUND waf in [$WAF_DIR]"
    else
        WAF_DIR=$WAF_DIR/..
    fi
    #echo "IDX [$IDX]"
    while [ "$IDX" -lt "$UP_COUNT" ] && [ $WAF_DIR_FOUND == 0 ]; do
        if [ -f "${WAF_DIR}/waf" ]; then
            WAF_DIR_FOUND=1
            #echo "FOUND waf in [$WAF_DIR]"
            export WAF_DIR
        else
            WAF_DIR=$WAF_DIR/..
            let "IDX=$IDX+1"
        fi
        #echo "IDX [$IDX] WAF_DIR [$WAF_DIR] WAF_DIR_FOUND [$WAF_DIR_FOUND]"
    done
    if [ $WAF_DIR_FOUND == 0 ]; then
        echo "ERROR! cannot find waf in [$RUN_DIR] or up to [$WAF_DIR]"
    fi
}
# Make sure that the waf dir is accessible (execute when this file is sourced).
find_waf_dir

# ================================================================
# add_to(): echo a message and also append it to a file
# ================================================================
add_to()
{
    FILE=$1
    MSG=$2
    echo "$MSG"
    echo "$MSG" >> $FILE
}
# ================================================================
# waf_clean()
# ================================================================
waf_clean()
{
    ./waf clean
}

# ================================================================
# gcov_clean()
# ================================================================
gcov_clean()
{
    if [ ! -f ./waf ]; then
        echo "ERROR! cannot find ./waf in $PWD! Not gonna remove gcov files..."
        exit 1
    fi
    /usr/bin/find . -name '*.gcda' | xargs rm -f
    /usr/bin/find . -name '*.gcov' | xargs rm -f
}

# ================================================================
# do_debug_purge()
# ================================================================
do_debug_purge()
{
    gcov_clean # this will also make sure we're in the correct 'waf' dir
    /usr/bin/find . -name '*.gcno' | xargs rm -f
    rm -rf ./ccnx-debug
}

# ================================================================
# waf_config()
# ================================================================
waf_config()
{
    OPTS="$@"
    ARGS="--build-profile=optimized --enable-examples --enable-tests"
    WITH_PYTHON=0
    for opt in $OPTS; do
        case $opt in
            "python")
                WITH_PYTHON=1
                ;;
            "gcov")
                ARGS+=" --enable-gcov"
                ;;
            "opt"|"optimized")
                ARGS+=" --build-profile=optimized"
                ;;
            *)
                echo "$0: waf_config: ERROR!"
                echo "    unknown argument: [$opt]"
                exit 13
                ;;
        esac
    done
    if [ "$WITH_PYTHON" == "0" ]; then
        ARGS+=" --disable-python --disable-nsclick --disable-gtk"
    fi
    if [ "$NS3_XCODE_COMPILE_BUGS_FIXED" == 1 ] || [ "$(uname -s)" == "Linux" ]; then
        echo "$0: waf_config:: [./waf configure $ARGS]"
        ./waf configure $ARGS
    else
        XCODE_MAJ=$(xcodebuild -version | head -1 | awk '{print $2}' | awk -F\. '{print $1}')
        XCODE_MIN=$(xcodebuild -version | head -1 | awk '{print $2}' | awk -F\. '{print $2}')
        if [ $XCODE_MAJ -gt 7 ] || [ $XCODE_MAJ -eq 7 -a $XCODE_MIN -gt 2 ]; then
            echo "$0: waf_config:: [CXXFLAGS_EXTRA=\"...\" ./waf configure $ARGS]"
            CXXFLAGS_EXTRA="-std=c++98 -Wno-unused-private-field -Wno-mismatched-new-delete" \
                ./waf configure $ARGS
        else
            echo "$0: waf_config:: [./waf configure $ARGS]"
            ./waf configure $ARGS
        fi
    fi
#    ./waf configure \
#        --enable-examples \
#        --disable-python \
#        --disable-nsclick \
#        --disable-gtk \
#        --enable-tests
}

# ================================================================
# waf_build_raw()
# ================================================================
waf_build_raw()
{
    ./waf build
}

# ================================================================
# test_all()
# ================================================================
test_all()
{
    ./test.py
}

# ================================================================
# run_example()
# ================================================================
run_example()
{
    EXAMPLE_NAME=$1
    EXAMPLE_NAME=$(echo "$EXAMPLE_NAME" | sed -e 's/\.cc$//')
    ./waf --run "$EXAMPLE_NAME"
}

# ================================================================
# get_ccnx_example_names()
# ================================================================
get_ccnx_example_names()
{
    CXX_EXAMPLES=$(ls ./src/"${REPO_DIR_NAME}"/examples/*.cc)
    echo "$CXX_EXAMPLES"
}

# ================================================================
# list_ccnx_examples()
# ================================================================
list_ccnx_examples()
{
    CXX_EXAMPLES="$(get_ccnx_example_names)"
    echo -e "\n${REPO_DIR_NAME} example scripts:\n"
    for ex in $CXX_EXAMPLES; do
        EX=$(echo $(basename "$ex") | sed -e 's/\.cc$//')
        echo "  $EX"
    done
    echo ""
}


