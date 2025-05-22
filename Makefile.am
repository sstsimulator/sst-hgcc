# -*- Makefile -*-
#
#

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/include

ACLOCAL_AMFLAGS = -I acinclude

bin_PROGRAMS = ssthg_clang

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

# TODO Figure out this was included
# if CLANG_NEED_LIBCPP
# ssthg_clang_LDFLAGS += -lclang-cpp
# endif

endif
