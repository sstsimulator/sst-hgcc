"""
Copyright 2009-2023 National Technology and Engineering Solutions of Sandia,
LLC (NTESS).  Under the terms of Contract DE-NA-0003525, the U.S. Government
retains certain rights in this software.

Sandia National Laboratories is a multimission laboratory managed and operated
by National Technology and Engineering Solutions of Sandia, LLC., a wholly
owned subsidiary of Honeywell International, Inc., for the U.S. Department of
Energy's National Nuclear Security Administration under contract DE-NA0003525.

Copyright (c) 2009-2023, NTESS

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Questions? Contact sst-macro-help@sandia.gov
"""

import os


def _conftest_strip_sst_pmi_link(flags, lib_dirs, libs):
  """
  Nested autoconf (e.g. hwloc under MVAPICH) inherits LDFLAGS with -lpmi and
  -L.../sst-elements-library/ext; linking trivial conftests then loads libpmi.so
  and fails on unresolved SST/Mercury symbols. Drop those for conftest only.
  """
  ext_mark = "sst-elements-library" + os.sep + "ext"
  ext_alt = "sst-elements-library/ext"

  def bad_libpath(p):
    if not p:
      return False
    norm = p.replace("\\", "/")
    return ext_alt in norm or ext_mark in p

  out_flags = []
  for f in flags:
    if f == "-lpmi":
      continue
    if f.startswith("-L") and len(f) > 2 and bad_libpath(f[2:]):
      continue
    out_flags.append(f)

  out_L = [p for p in (lib_dirs or []) if not bad_libpath(p)]
  out_l = [x for x in (libs or []) if x != "pmi"]
  return out_flags, out_L, out_l


def addLink(ctx, ldTarget, args, cmds, objects, toExe=False):
  #xargs includes the list of object files
  from hgccvars import prefix
  from hgccvars import soFlagsStr

  is_conftest = getattr(ctx, 'is_conftest', False)

  wlFlags = getattr(ctx, 'wlFlags', [])

  if is_conftest:
    # Conftest: minimal link; drop -flat_namespace (dyld/SST issues on macOS).
    bad_flags = {"-Wl,-flat_namespace", "-flat_namespace"}
    linkCmdArr = [ctx.ld]
    for f in wlFlags:
      if f not in bad_flags:
        linkCmdArr.append(f)
    cflags, use_L, use_l = _conftest_strip_sst_pmi_link(
        ctx.compilerFlags, args.L, args.l)
    for f in cflags:
      if f not in bad_flags:
        linkCmdArr.append(f)
    linkCmdArr.extend(map(lambda x: "-L%s" % x, use_L))
    linkCmdArr.extend(map(lambda x: "-l%s" % x, use_l))
  else:
    ldpathMaker = "-Wl,-rpath,%s/lib" % prefix
    linkCmdArr = [ctx.ld, ldpathMaker] 
    linkCmdArr.extend(wlFlags)
    linkCmdArr.extend(ctx.ldFlags)
    if not toExe:
      linkCmdArr.extend(soFlagsStr.split())
    linkCmdArr.extend(ctx.libs)
    linkCmdArr.extend(ctx.compilerFlags)
    linkCmdArr.extend(map(lambda x: "-L%s" % x, args.L))
    linkCmdArr.extend(map(lambda x: "-l%s" % x, args.l))
  
  # Add conditional replacement libraries based on --replacements parameter
  if hasattr(ctx, 'requestedReplacements') and ctx.requestedReplacements:
    replacementsLibPath = prefix + "/lib"
    
    # pthread.h has no hgcc_* lib (Mercury provides pthread).
    replacementLibMap = {
      'omp.h': 'hgcc_omp',
      'blas.h': 'hgcc_blas',
      'cblas.h': 'hgcc_blas',
      'bgp.h': 'hgcc_machines'
    }
    
    for replacement in ctx.requestedReplacements:
      if replacement in replacementLibMap:
        libName = replacementLibMap[replacement]
        libPath = replacementsLibPath + "/lib%s.a" % libName
        try:
          with open(libPath, 'r'):
            pass
          libExists = True
        except:
          libExists = False
        if libExists:
          linkCmdArr.append(libPath)
        else:
          # Try as shared library
          libPath = replacementsLibPath + "/lib%s.so" % libName
          try:
            with open(libPath, 'r'):
              pass
            libExists = True
          except:
            libExists = False
          if libExists:
            linkCmdArr.append(libPath)
          else:
            print("Warning: Replacement library for '%s' not found" % replacement)
  
  linkCmdArr.extend(objects)
  linkCmdArr.append("-o")
  linkCmdArr.append(ldTarget)
  cmds.append([None,linkCmdArr,[]])
