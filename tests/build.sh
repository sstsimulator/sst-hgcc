#!/bin/bash

hg++ --replacements pthread.h -DSST_HG_USE_MERCURY_PTHREAD -c test_tls.cc
hg++ test_tls.o -o libtest_tls.so

hg++ --replacements pthread.h -DSST_HG_USE_MERCURY_PTHREAD -c test_pthread.cc
hg++ test_pthread.o -o libtest_pthread.so
