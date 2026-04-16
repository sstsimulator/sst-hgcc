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

import json
import os
import shlex
import sys


def _hgcc_bool_attr(mod, name, default=False):
  v = getattr(mod, name, default)
  if isinstance(v, bool):
    return v
  if isinstance(v, str):
    return v.strip().lower() in ("1", "true", "yes", "on")
  return bool(v)


def _argv_drop_compiler_leader(argv):
  """Drop compile_commands argv[0] driver name."""
  if not argv:
    return []
  first = argv[0]
  bn = os.path.basename(first)
  for suffix in ("g++", "gcc", "c++", "cc", "clang++", "clang", "hg++", "hgcc"):
    if bn == suffix or bn.endswith("-" + suffix):
      return list(argv[1:])
  return list(argv)


def _filter_ast_host_flags(flags):
  """Strip flags not useful for ssthg_clang parse-only."""
  out = []
  i = 0
  n = len(flags)
  while i < n:
    f = flags[i]
    if f in ("-E", "-c", "-pipe", "-S"):
      i += 1
      continue
    if f == "-o" and i + 1 < n:
      i += 2
      continue
    if len(f) > 2 and f.startswith("-o"):
      i += 1
      continue
    if f.startswith("-stdlib="):
      i += 1
      continue
    if f in ("-MD", "-MP", "-MMD", "-MG", "-M", "-MM"):
      i += 1
      continue
    if f.startswith("-M"):
      i += 1
      continue
    if f in ("-MF", "-MT", "-MQ") and i + 1 < n:
      i += 2
      continue
    out.append(f)
    i += 1
  return out


def _strip_include_directives(argv):
  """Remove -include and its path from argv.

  addSrc2SrcCompile runs gcc -E with the same -D/-I/-include as the host
  compile, then passes the preprocessed file to ssthg_clang. Re-applying
  -include on the pp file pulls headers twice (redefinition of static
  inlines in skeleton.h, glibc typedef conflicts in bits/types.h).
  """
  out = []
  i = 0
  n = len(argv)
  while i < n:
    a = argv[i]
    if a == "-include":
      if i + 1 < n:
        i += 2
      else:
        i += 1
      continue
    out.append(a)
    i += 1
  return out


def _trim_trailing_source_arg(argv):
  """Drop trailing source path from compile_commands argv."""
  if not argv:
    return argv
  last = argv[-1]
  if last.startswith("-"):
    return argv
  low = last.lower()
  for suf in (".c", ".cc", ".cpp", ".cxx", ".C", ".CPP"):
    if low.endswith(suf):
      return argv[:-1]
  return argv


def host_compile_argv_for_ast(ctx, args):
  """Host -D/-I/-std/-include list for AST (matches preprocess)."""
  argv = []
  argv.extend("-D%s" % d for d in ctx.defines)
  argv.extend("-D%s" % d for d in getattr(args, "D", []) or [])
  argv.extend("-I%s" % p for p in (args.I or []))
  argv.extend(ctx.cppFlags)
  argv.extend(ctx.compilerFlags)
  for entry in ctx.directIncludes:
    argv.append("-include")
    argv.append(entry)
  return _filter_ast_host_flags(argv)


def compile_commands_argv_for_source(source_path):
  """Argv from HGCC_COMPILE_COMMANDS JSON for this TU, or None."""
  raw = os.environ.get("HGCC_COMPILE_COMMANDS")
  if not raw:
    return None
  path = os.path.expanduser(raw.strip())
  if not path or not os.path.isfile(path):
    return None
  try:
    with open(path, "r", encoding="utf-8") as fh:
      db = json.load(fh)
  except (OSError, ValueError, json.JSONDecodeError):
    return None
  want = os.path.abspath(source_path)
  for ent in db:
    f = ent.get("file")
    if not f:
      continue
    try:
      if os.path.abspath(f) != want:
        continue
    except OSError:
      continue
    args = ent.get("arguments")
    if isinstance(args, list) and args:
      return _trim_trailing_source_arg(
          _filter_ast_host_flags(_argv_drop_compiler_leader(args)))
    cmd = ent.get("command")
    if isinstance(cmd, str) and cmd.strip():
      try:
        parts = shlex.split(cmd)
      except ValueError:
        return None
      return _trim_trailing_source_arg(
          _filter_ast_host_flags(_argv_drop_compiler_leader(parts)))
  return None


def apply_ast_gnuxx_remap(argv, ast_gnuxx_remap):
  if not ast_gnuxx_remap:
    return argv
  out = []
  for a in argv:
    if a.startswith("-std=c++") and not a.startswith("-std=gnu"):
      out.append("-std=gnu++17")
    else:
      out.append(a)
  return out


def _pp_compiler_flags_match_ast_std(ctx, args, ast_base_argv, sstStdFlag):
  """Align host g++ -std= with ssthg_clang after apply_ast_libstdcxx_cpp14_floor / gnuxx remap.

  Used for: clang++ -E, and compiler flags for the sst.pp.* / sstGlobals compile
  (driver may be clang++ or g++; see use_clangxx_host in addSrc2SrcCompile).

  If the preprocessor uses c++11 while ssthg_clang uses c++14+, libstdc++ <utility>
  is expanded for GCC (__integer_pack); Clang then cannot parse the .ii file.
  If the final driver is g++ at c++14, libstdc++ may still use Clang builtins
  (__make_integer_seq) that g++ does not provide; use clang++ -c with libstdc++.
  """
  out = list(ctx.compilerFlags)
  if ctx.typ != "c++":
    return out
  std_from_ast = next((a for a in ast_base_argv if a.startswith("-std=")), None)
  if std_from_ast:
    out = [a for a in out if not a.startswith("-std=")]
    return [std_from_ast] + out
  if not any(a.startswith("-std=") for a in out):
    if getattr(args, "std", None):
      return ["-std=%s" % args.std] + out
    return [sstStdFlag] + out
  return out


def apply_ast_libstdcxx_cpp14_floor(argv, enabled):
  """Raise C++11 dialect for ssthg_clang parse when using libstdc++ with Clang.

  libstdc++ 9+ uses __has_builtin(__make_integer_seq) in <utility>; g++ -E
  still expands the __integer_pack branch (GCC builtin), so ssthg_clang must
  not parse g++-preprocessed libstdc++. We also use clang++ -E for C++/libstdc++
  (see addPreprocess). -std=c++14+ is still required for the preprocessor.
  """
  if not enabled:
    return argv
  out = []
  for a in argv:
    if a in ("-std=c++11", "-std=c++0x"):
      out.append("-std=c++14")
    elif a in ("-std=gnu++11", "-std=gnu++0x"):
      out.append("-std=gnu++14")
    else:
      out.append(a)
  return out


def _clang_cxx_for_preprocess():
  """Path to clang++ under CLANG_INSTALL_DIR, or None if missing."""
  import os
  try:
    from hgccvars import clangDir
  except ImportError:
    return None
  cand = os.path.join(clangDir, "bin", "clang++")
  return cand if os.path.isfile(cand) else None


def _clang_cc_for_preprocess():
  """Path to clang (not clang++) under CLANG_INSTALL_DIR, or None if missing."""
  try:
    from hgccvars import clangDir
  except ImportError:
    return None
  cand = os.path.join(clangDir, "bin", "clang")
  return cand if os.path.isfile(cand) else None


def _clang_cxx_from_libclang_include_flags(tokens):
  """Derive .../bin/clang++ from -I.../lib/clang/<ver>/include when hgccvars.clangDir is wrong."""
  for tok in tokens:
    if not tok.startswith("-I"):
      continue
    p = tok[2:]
    key = "/lib/clang/"
    i = p.find(key)
    if i == -1:
      continue
    root = p[:i]
    cand = os.path.join(root, "bin", "clang++")
    if os.path.isfile(cand):
      return cand
  return None


def _clang_cc_from_libclang_include_flags(tokens):
  """Derive .../bin/clang from -I.../lib/clang/<ver>/include."""
  for tok in tokens:
    if not tok.startswith("-I"):
      continue
    p = tok[2:]
    key = "/lib/clang/"
    i = p.find(key)
    if i == -1:
      continue
    root = p[:i]
    cand = os.path.join(root, "bin", "clang")
    if os.path.isfile(cand):
      return cand
  return None


def _clang_cc_resolve_for_linux_preprocess(ctx, flag_tokens):
  """Same LLVM as ssthg_clang: glibc + gcc -E embeds __malloc__(deallocator) that Clang cannot parse."""
  c = _clang_cc_for_preprocess()
  if c:
    return c
  c = _clang_cc_from_libclang_include_flags(flag_tokens)
  if c:
    return c
  for attr in ("compiler", "cxx"):
    comp = getattr(ctx, attr, None) or ""
    if not comp or "++" in os.path.basename(comp):
      continue
    base = os.path.basename(comp)
    if base.startswith("clang") and os.path.isfile(comp):
      return comp
  return None


def _clang_cxx_resolve_for_libstdc_host(ctx, flag_tokens):
  """LLVM bin/clang++ for -E / host -c with libstdc++; hgccvars.clangDir may be unset or wrong."""
  c = _clang_cxx_for_preprocess()
  if c:
    return c
  c = _clang_cxx_from_libclang_include_flags(flag_tokens)
  if c:
    return c
  for attr in ("compiler", "cxx"):
    comp = getattr(ctx, attr, None) or ""
    if not comp:
      continue
    base = os.path.basename(comp)
    if base.startswith("clang++") and os.path.isfile(comp):
      return comp
  return None


def addPreprocess(
    ctx,
    sourceFile,
    outputFile,
    args,
    cmds,
    compiler_flags=None,
    use_clang_cpp_for_e=False,
    clang_exe_for_preprocess=None,
    clang_cc_for_linux_preprocess=None,
):
  """use_clang_cpp_for_e: run clang++ -E so libstdc++ expands for Clang (__make_integer_seq),
  not g++ (__integer_pack), which ssthg_clang cannot parse.

  clang_cc_for_linux_preprocess: on Linux C, gcc -E expands glibc with __GNUC__ from gcc,
  embedding __malloc__(deallocator, n) into the .ii file; ssthg_clang then cannot parse
  that text. Use clang -fgnuc-version=10 -E so the preprocessor output matches what Clang
  can parse (see addSrc2SrcCompile)."""
  if use_clang_cpp_for_e and ctx.typ == "c++":
    if clang_exe_for_preprocess:
      clang_pp = clang_exe_for_preprocess
    else:
      scan = []
      cf = compiler_flags if compiler_flags is not None else ctx.compilerFlags
      if isinstance(cf, (list, tuple)):
        scan.extend(cf)
      else:
        scan.extend(shlex.split(str(cf)))
      clang_pp = _clang_cxx_resolve_for_libstdc_host(ctx, scan)
    if clang_pp:
      ppArgs = [clang_pp, "-stdlib=libstdc++"]
    else:
      ppArgs = [ctx.compiler]
  elif clang_cc_for_linux_preprocess:
    ppArgs = [clang_cc_for_linux_preprocess, "-fgnuc-version=10"]
  else:
    ppArgs = [ctx.compiler]
  ppArgs.extend(map(lambda x: "-D%s" % x, ctx.defines))
  ppArgs.extend(map(lambda x: "-D%s" % x, args.D))
  ppArgs.extend(map(lambda x: "-I%s" % x, args.I)) 
  ppArgs.extend(ctx.cppFlags)
  ppArgs.extend(compiler_flags if compiler_flags is not None else ctx.compilerFlags)
  for entry in ctx.directIncludes:
    ppArgs.append("-include")
    ppArgs.append(entry)
  ppArgs.append("-E")
  ppArgs.append(sourceFile)
  if args.O:
    ppArgs.append("-O%s" % args.O)
  cmds.append([outputFile,ppArgs,[]]) #pipe file, no extra temps

def addEmitLlvm(ctx, sourceFile, outputFile, args, cmds):
  cmdArr = [
    ctx.compiler,
    "-emit-llvm",
    "-S",
    "--no-integrated-cpp",
    sourceFile,
    "-o",
    outputFile
  ]
  if args.O:
    cmds.append("-O%s" % args.O)
  cmds.append([None,cmdArr,[outputFile]])

def addLlvmOptPass(ctx, llFile, llvmPass, args, cmds):
  from hgccvars import clangDir
  from hgccvars import prefix
  import os
  optTool = os.path.join(clangDir, "bin", "opt")
  llvmLib = os.path.join(prefix, "lib", "lib%s.so" % llvmPass)
  cmdArr = [
    optTool,
    "-S",
    "-load",
    llvmLib,
    llFile,
    "-sst-%s" % llvmPass,
    "-o",
    llFile,
  ]
  cmds.append([None,cmdArr,[]])

def addLlvmCompile(ctx, llFile, objFile, args, cmds):
  cmdArr = [
    ctx.compiler,
    "-o",
    objFile,
    "-c",
    llFile,
  ]
  cmdArr.extend(ctx.compilerFlags)
  cmds.append([None,cmdArr,[objFile]])

def addModeDefines(ctx, args):
  if getattr(ctx, "is_conftest", False):
    return
  if ctx.mode != ctx.COMPONENT:
    # this breaks current Mercury way of doing things
    #ctx.defines.append("SST_HG_EXTERNAL")
    if ctx.sstCore:
      ctx.defines.append("SST_HG_EXTERNAL_SKELETON")

  if ctx.srcToSrc() or ctx.mode == ctx.COMPONENT:
    ctx.defines.append("SST_HG_NO_REFACTOR_MAIN")

  if not ctx.simulateMode():
    ctx.defines.append("SST_HG_NO_REPLACEMENTS")

def addSrc2SrcCompile(ctx, sourceFile, outputFile, args, cmds):
  import hgccvars as _hv
  from hgccvars import prefix
  from hgccvars import defaultIncludePaths, includeDir, includeDirElements
  from hgccutils import cleanFlag, swapSuffix, addPrefixAndRebase, addPrefix
  from hgccvars import clangCppFlagsStr, clangLdFlagsStr
  from hgccvars import clangLibtoolingCxxFlagsStr, clangLibtoolingCFlagsStr
  from hgccvars import haveFloat128
  from hgccvars import sstStdFlag

  use_libcxx = _hgcc_bool_attr(_hv, "useLibcxxForAst", False)
  ast_gnuxx_remap = _hgcc_bool_attr(_hv, "astGnuxxRemap", False)
  cxx_resource = getattr(_hv, "clangLibtoolingCxxResourceStr", "").strip()

  # Build AST argv first so g++ -E uses the same -std= as ssthg_clang (libstdc++).
  base_argv = compile_commands_argv_for_source(sourceFile)
  if base_argv is None:
    base_argv = host_compile_argv_for_ast(ctx, args)
  if ctx.typ == "c++" and not any(a.startswith("-std=") for a in base_argv):
    if args.std:
      base_argv = ["-std=%s" % args.std] + base_argv
    else:
      base_argv = [sstStdFlag] + base_argv
  base_argv = apply_ast_libstdcxx_cpp14_floor(
      base_argv, (not use_libcxx) and ctx.typ == "c++")
  base_argv = apply_ast_gnuxx_remap(base_argv, ast_gnuxx_remap)
  host_cxx_flags_for_obj = _pp_compiler_flags_match_ast_std(
      ctx, args, base_argv, sstStdFlag)
  # Resource -I flags passed to ssthg_clang (same list order as below).
  if ctx.typ == "c++":
    if use_libcxx:
      ast_resource_tokens = [
          x for x in clangLibtoolingCxxFlagsStr.split() if x]
    else:
      res_flags = cxx_resource if cxx_resource else clangLibtoolingCFlagsStr
      ast_resource_tokens = [x for x in res_flags.split() if x]
  else:
    ast_resource_tokens = [x for x in clangLibtoolingCFlagsStr.split() if x]
  # g++ does not implement Clang's __make_integer_seq; libstdc++ can still pick
  # that branch when __has_builtin matches. Use clang++ -c (with libstdc++) to
  # match clang++ -E and ssthg_clang. Resolve clang++ even when hgccvars.clangDir
  # is wrong (e.g. hg++ already uses clang++; or -I.../lib/clang/N/include).
  pre_tokens = list(base_argv) + ast_resource_tokens
  clang_cc_linux = None
  if ctx.typ != "c++" and sys.platform.startswith("linux"):
    clang_cc_linux = _clang_cc_resolve_for_linux_preprocess(ctx, pre_tokens)
  clang_cxx_bin = _clang_cxx_resolve_for_libstdc_host(ctx, pre_tokens)
  use_clangxx_host = (
      (not use_libcxx) and ctx.typ == "c++" and bool(clang_cxx_bin))

  #First we must pre-process the file to get it read for source-to-source
  objBaseFolder, objName = os.path.split(outputFile)
  ppTmpFile = addPrefixAndRebase("pp.", sourceFile, objBaseFolder)
  if not ctx.src2srcDebug:
    addPreprocess(
        ctx,
        sourceFile,
        ppTmpFile,
        args,
        cmds,
        compiler_flags=host_cxx_flags_for_obj,
        use_clang_cpp_for_e=(not use_libcxx) and ctx.typ == "c++",
        clang_exe_for_preprocess=clang_cxx_bin,
        clang_cc_for_linux_preprocess=clang_cc_linux,
    )

  rawPaths = defaultIncludePaths.split(":")
  cleanPaths = []
  for path in rawPaths:
    cleanPaths.append(os.path.abspath(path))
  defaultIncludePaths = ":".join(cleanPaths)
  #defaultIncludePaths += ":" + cleanFlag(includeDir)

  # System include roots for isInSystemHeader(). Use SSTHG_SYSTEM_INCLUDES so
  # the subprocess does not need --system-includes (llvm::cl + CommonOptionsParser
  # rejects that flag on some Apple/LLVM-linked ssthg_clang builds).
  os.environ["SSTHG_SYSTEM_INCLUDES"] = defaultIncludePaths

  clangDeglobal = os.path.join(prefix, "bin", "ssthg_clang")
  clangCmdArr = [clangDeglobal]
  clangCmdArr.append(ppTmpFile)
  clangCmdArr.extend(ctx.clangArgs)

  clangCmdArr.append("--")
  # Linux C: pp.* should come from clang -fgnuc-version=10 -E (see addPreprocess). Keep
  # the same for any remaining parse so libc feature tests stay consistent.
  if ctx.typ != "c++" and sys.platform.startswith("linux"):
    clangCmdArr.append("-fgnuc-version=10")

  if hasattr(ctx, "requestedReplacements") and ctx.requestedReplacements:
    if hasattr(ctx, "tempReplacementsDir"):
      clangCmdArr.append("-I")
      clangCmdArr.append(ctx.tempReplacementsDir)
      clangCmdArr.append("-I")
      clangCmdArr.append(os.path.join(prefix, "include"))

  if not ctx.src2srcDebug:
    base_argv = _strip_include_directives(base_argv)
  clangCmdArr.extend(base_argv)

  if ctx.typ == "c++":
    if use_libcxx:
      clangCmdArr.append("-stdlib=libc++")
    else:
      clangCmdArr.append("-stdlib=libstdc++")

  need_float_stubs = (not haveFloat128 and
                      not (ctx.typ == "c++" and sys.platform.startswith("linux")))
  if need_float_stubs:
    clangCmdArr.append("-D__float128=clangFloat128Fix")
    if ctx.typ != "c++":
      clangCmdArr.append("-D_Float32=float")
      clangCmdArr.append("-D_Float64=double")
      clangCmdArr.append("-D_Float32x=double")
      clangCmdArr.append("-D_Float64x=long double")
      clangCmdArr.append("-D_Float128=long double")
  clangCmdArr.append("-D__builtin_clzg(...)=0")
  clangCmdArr.append("-D__builtin_ctzg(...)=0")

  clangCmdArr.extend(ast_resource_tokens)

  srcRepl = addPrefixAndRebase("sst.pp.",sourceFile,objBaseFolder)
  cxxInitSrcFile = addPrefixAndRebase("sstGlobals.pp.",sourceFile,objBaseFolder) + ".cpp"
  if not ctx.src2srcDebug:
    cmds.append([None,clangCmdArr,[ppTmpFile,srcRepl,cxxInitSrcFile]]) #None -> don't pipe output anywhere

  tmpTarget = addPrefix("tmp.", outputFile)
  llvmPasses = []
  if args.skeletonize:
    llvmPasses = args.skeletonize.split(",")

  if llvmPasses:
    llvmFile = swapSuffix("ll", tmpTarget)
    addEmitLlvm(ctx, srcRepl, llvmFile, args, cmds)
    for llvmPass in llvmPasses:
      addLlvmOptPass(ctx, llvmFile, llvmPass, args, cmds)
    addLlvmCompile(ctx, llvmFile, tmpTarget, args, cmds)
    
  else:
    if use_clangxx_host:
      cmdArr = [clang_cxx_bin, "-stdlib=libstdc++"]
    else:
      cmdArr = [ctx.compiler]
    cmdArr.extend(map(lambda x: "-D%s" % x, ctx.defines))
    cmdArr.extend(map(lambda x: "-D%s" % x, getattr(args, "D", [])))
    cmdArr.extend(map(lambda x: "-I%s" % x, args.I))
    cmdArr.extend(ctx.cppFlags)
    cmdArr.extend(host_cxx_flags_for_obj)
    if not use_clangxx_host:
      cmdArr.append("--no-integrated-cpp")
    cmdArr.append("-o")
    cmdArr.append(tmpTarget)
    cmdArr.append("-c")
    cmdArr.append(srcRepl)
    if args.O:
      cmdArr.append("-O%s" % args.O)
    if args.g:
      cmdArr.append("-g")
    cmds.append([None,cmdArr,[tmpTarget]])

  cxxInitObjFile = addPrefix("sstGlobals.", outputFile)
  if use_clangxx_host:
    cxxInitCmdArr = [
        clang_cxx_bin,
        "-stdlib=libstdc++",
        "-o",
        cxxInitObjFile,
        "-I%s/include" % prefix,
        "-c",
        cxxInitSrcFile,
    ]
  else:
    cxxInitCmdArr = [
        ctx.cxx,
        "-o",
        cxxInitObjFile,
        "-I%s/include" % prefix,
        "-c",
        cxxInitSrcFile,
    ]
  cxxInitCmdArr.extend(ctx.cxxFlags)
  cxxInitCmdArr.extend(ctx.cppFlags)
  if ctx.typ == "c++":
    std_ast = next(
        (a for a in host_cxx_flags_for_obj if a.startswith("-std=")), None)
    if std_ast:
      cxxInitCmdArr = [a for a in cxxInitCmdArr if not a.startswith("-std=")]
      cxxInitCmdArr.append(std_ast)
  if args.O:
    cxxInitCmdArr.append("-O%s" % args.O)
  if args.g:
    cxxInitCmdArr.append("-g")
  cmds.append([None,cxxInitCmdArr,[cxxInitObjFile]])

  mergeCmdArr = [
    "ld", "-r"
  ]

  import platform
  if not platform.system() == "Darwin":
    mergeCmdArr.append("--unique")

  mergeCmdArr.append(tmpTarget)
  mergeCmdArr.append(cxxInitObjFile)
  mergeCmdArr.append("-o")
  mergeCmdArr.append(outputFile)

  cmds.append([None,mergeCmdArr,[]])

def addComponentCompile(ctx, sourceFile, outputFile, args, cmds):
  buildArgs = [ctx.compiler] 
  for entry in ctx.directIncludes:
    buildArgs.append("-include")
    buildArgs.append(entry)
  buildArgs.extend(map(lambda x: "-D%s" % x, ctx.defines))
  buildArgs.extend(map(lambda x: "-D%s" % x, getattr(args, "D", [])))
  buildArgs.extend(map(lambda x: "-I%s" % x, args.I)) 
  buildArgs.extend(ctx.cppFlags)
  buildArgs.extend(ctx.compilerFlags)
  buildArgs.append("-c")
  buildArgs.append(sourceFile)
  buildArgs.append("-o")
  buildArgs.append(outputFile)
  cmds.append([None,buildArgs,[]])

def addCompile(ctx, sourceFile, outputFile, args, cmds):
  ppArgs = [ctx.compiler]
  for entry in ctx.directIncludes:
    ppArgs.append("-include")
    ppArgs.append(entry)
  ppArgs.extend(map(lambda x: "-D%s" % x, ctx.defines))
  ppArgs.extend(map(lambda x: "-D%s" % x, getattr(args, "D", [])))
  ppArgs.extend(map(lambda x: "-I%s" % x, args.I))
  ppArgs.extend(ctx.cppFlags)
  ppArgs.extend(ctx.compilerFlags)
  ppArgs.append("-c")
  ppArgs.append(sourceFile)
  ppArgs.append("-o")
  ppArgs.append(outputFile)
  cmds.append([outputFile,ppArgs,[]]) #pipe file, no extra temps

