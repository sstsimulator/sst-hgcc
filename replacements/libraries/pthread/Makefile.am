#
#   This file is part of SST/macroscale: 
#                The macroscale architecture simulator from the SST suite.
#   Copyright (c) 2009-2024, NTESS.
#   This software is distributed under the BSD License.
#   Under the terms of Contract DE-NA0003525 with NTESS,
#   the U.S. Government retains certain rights in this software.
#   For more information, see the LICENSE file in the top 
#   SST/macroscale directory.
#

include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libhgcc_pthread.la
library_includedir=$(includedir)/sstmac/libraries/pthread

AM_FCFLAGS = -I$(top_builddir) -I$(top_srcdir) -m32 -m64

libhgcc_pthread_la_LDFLAGS = 

libhgcc_pthread_la_SOURCES = \
  hgcc_pthread.cc \
  hgcc_pthread_runner.cc \
  hgcc_sched.cc 

nodist_library_include_HEADERS = \
  hgcc_sys_types.h

nobase_library_include_HEADERS = \
  hgcc_cpu_set.h \
  hgcc_pthread_impl.h \
  hgcc_pthread_macro.h \
  hgcc_pthread_clear_macros.h \
  hgcc_pthread.h \
  hgcc_pthread_runner.h \
  hgcc_sched.h \
  hgcc_sched_macro.h \
  hgcc_sched_clear_macros.h 

