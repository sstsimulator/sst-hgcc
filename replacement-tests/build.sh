#!/bin/bash

hgcc --replacements="limits.h" -c test_limits.cc
hgcc --replacements="limits.h" test_limits.o -o test_limits.so
cp test_limits.so ../../install/sst-elements/lib/sst-elements-library/libtest_limits.so
