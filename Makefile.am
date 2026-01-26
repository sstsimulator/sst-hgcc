# -*- Makefile -*-
#
#

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/include

ACLOCAL_AMFLAGS = -I acinclude

dist_bin_SCRIPTS = \
  hgcclib.py \
  hgccutils.py \
  hgcompile.py \
  hglink.py \
  configlib.py

bin_SCRIPTS = \
  hgccvars.py \
  hgcc \
  hg++

AM_LDFLAGS = -ldl

EXTRA_DIST = clang

if HAVE_CLANG

bin_PROGRAMS = ssthg_clang

ssthg_clang_SOURCES = \
  clang/util.cc \
  clang/main.cc \
  clang/astVisitorGlobalVars.cc \
  clang/pragmas.cc \
  clang/computePragma.cc \
  clang/computeVisitor.cc \
  clang/replacePragma.cc \
  clang/astConsumers.cc \
  clang/frontendActions.cc \
  clang/astVisitor.cc 

if USE_REPLACEMENT_HEADERS
	nobase_library_include_HEADERS += \
  replacements/time.h 
#   replacements/sys/types.h 
#  	replacements/linux/mmtimer.h \
#  	replacements/linux/limits.h \
#  replacements/clear_symbol_macros.h \
#  replacements/return_symbol_macros.h \
  #replacements/sstmac_pthread_return.h \
#  replacements/sys/sendfile.h \
#  replacements/dlfcn.h \
  #replacements/future \
  #replacements/fstream \
  #replacements/iostream \
  #replacements/ios \
 # replacements/ostream \
 # replacements/istream \
  #replacements/condition_variable \
  #replacements/list \
  #replacements/map \
  #replacements/unordered_map \
  #replacements/unordered_set \
  #replacements/memory \
  #replacements/mutex \
  #replacements/thread \
  #replacements/sched.h \
  #replacements/pthread.h \
  #replacements/unistd.h \
  #replacements/cstring \
  #replacements/cstdlib \
  #replacements/stdlib.h \
  #replacements/malloc.h \
  #replacements/mm_malloc.h \
  #replacements/string.h \
  #replacements/queue \
  #replacements/sstream \
  #replacements/stack \
  #replacements/set \
  #replacements/string \
  #replacements/vector \
  #replacements/stdio.h \
  #replacements/signal.h \
  #replacements/sstmac_pthread_clear.h \
  #replacements/wait.h \
  #replacements/sys/time.h \
  #replacements/sys/signal.h 
endif
ssthg_clang_CXXFLAGS = -fno-rtti $(AM_CXXFLAGS)
ssthg_clang_CPPFLAGS = $(CLANG_CPPFLAGS) $(AM_CPPFLAGS) $(SST_CPPFLAGS)

if HAVE_CXX17
ssthg_clang_SOURCES += \
	clang/annotatePragma.cc \
	clang/memoizePragma.cc \
	clang/memoizeVariableCaptureAnalyzer.cc \
	clang/ompPuppetizePragma.cc \
	clang/memoizeVariable.cc

old_standards=-std=c++11 -std=c++0x -std=c++1y -std=c++14 -std=c++1z
libclang17_a_CXXFLAGS = $(filter-out $(old_standards), $(sstmac_clang_CXXFLAGS))

library_includedir=$(includedir)/memoization
library_include_HEADERS = clang/memoization/capture.h
endif

# Install replacement headers directory (outside HAVE_CXX17, inside HAVE_CLANG)
install-data-local:
	$(MKDIR_P) $(DESTDIR)$(includedir)/replacements
	$(MKDIR_P) $(DESTDIR)$(includedir)/replacements/sys
	$(MKDIR_P) $(DESTDIR)$(includedir)/replacements/linux
	$(MKDIR_P) $(DESTDIR)$(includedir)/replacements/mpi
	$(MKDIR_P) $(DESTDIR)$(includedir)/replacements/libraries
	$(MKDIR_P) $(DESTDIR)$(includedir)/replacements/libraries/pthread
	cp -r $(srcdir)/replacements/* $(DESTDIR)$(includedir)/replacements/
	@# Copy configured files from build directory (overwrite .in files)
	if test -f $(builddir)/replacements/sstmac_pthread_clear.h; then \
		cp $(builddir)/replacements/sstmac_pthread_clear.h $(DESTDIR)$(includedir)/replacements/; \
	fi
	if test -f $(builddir)/replacements/libraries/pthread/sstmac_sys_types.h; then \
		cp $(builddir)/replacements/libraries/pthread/sstmac_sys_types.h $(DESTDIR)$(includedir)/replacements/libraries/pthread/; \
	fi
	@# Remove .in files that have been configured
	rm -f $(DESTDIR)$(includedir)/replacements/sstmac_pthread_clear.h.in
	rm -f $(DESTDIR)$(includedir)/replacements/libraries/pthread/sstmac_sys_types.h.in

ssthg_clang_LDFLAGS = $(CLANG_LDFLAGS) \
 -Wl,-rpath,@CLANG_INSTALL_DIR@/lib \
 -lclang \
 -lclangFrontend \
 -lclangFrontendTool \
 -lclangTooling \
 -lclangBasic \
 -lclangASTMatchers \
 -lclangFormat \
 -lclangFrontend \
 -lclangDriver \
 -lclangParse \
 -lclangSerialization \
 -lclangSema \
 -lclangEdit \
 -lclangAnalysis \
 -lclangToolingCore \
 -lclangAST \
 -lclangRewrite \
 -lclangLex \
 -lclangBasic \
 -lclangSupport \
 -lclangAPINotes \
 @CLANG_LIBTOOLING_LIBS@ \
 @CLANG_LIBTOOLING_SYSTEM_LIBS@

# TODO Figure out why this was included
# if CLANG_NEED_LIBCPP
# ssthg_clang_LDFLAGS += -lclang-cpp
# endif

endif
