#!/bin/bash

echo "Running the NS3 uncrustify on all .h, .cc, and .cpp files"

find . \( -name '*.cc' -or -name '*.h' -or -name '*.cpp' \) -exec ../../utils/check-style.py -l 3 -f {} -i \;

