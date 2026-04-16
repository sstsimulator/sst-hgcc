#!/usr/bin/env python3
"""Unit tests for hgcompile AST argv helpers."""
import json
import os
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import hgcompile as hc
import hglink as hl  


class _FakeArgs:
  def __init__(self):
    self.D = ["EXTRA=1"]
    self.I = ["/tmp/include"]
    self.std = None


class _FakeCtx:
  def __init__(self):
    self.defines = ["A", "B=2"]
    self.cppFlags = ["-I/sys/cpp"]
    self.compilerFlags = ["-fPIC", "-std=c++17", "-c", "-o", "out.o"]
    self.directIncludes = ["/skel.h"]


class TestHgcompileAst(unittest.TestCase):
  def test_filter_ast_host_flags(self):
    flags = ["-c", "-O2", "-stdlib=libc++", "-o", "x.o", "-pipe", "foo.cc"]
    got = hc._filter_ast_host_flags(flags)
    self.assertEqual(got, ["-O2", "foo.cc"])

  def test_trim_trailing_source_arg(self):
    self.assertEqual(
        hc._trim_trailing_source_arg(["-std=c++11", "/a/b.cc"]),
        ["-std=c++11"],
    )
    self.assertEqual(hc._trim_trailing_source_arg(["-v"]), ["-v"])

  def test_host_compile_argv_for_ast(self):
    argv = hc.host_compile_argv_for_ast(_FakeCtx(), _FakeArgs())
    self.assertIn("-DA", argv)
    self.assertIn("-DB=2", argv)
    self.assertIn("-DEXTRA=1", argv)
    self.assertIn("-I/tmp/include", argv)
    self.assertIn("-I/sys/cpp", argv)
    self.assertIn("-fPIC", argv)
    self.assertIn("-std=c++17", argv)
    self.assertIn("-include", argv)
    self.assertIn("/skel.h", argv)
    self.assertNotIn("-c", argv)
    self.assertNotIn("-o", argv)

  def test_strip_include_directives(self):
    argv = ["-DA=1", "-include", "/skel.h", "-I/tmp", "-include-pch", "x.pch"]
    got = hc._strip_include_directives(argv)
    self.assertEqual(got, ["-DA=1", "-I/tmp", "-include-pch", "x.pch"])

  def test_apply_ast_gnuxx_remap(self):
    self.assertEqual(
        hc.apply_ast_gnuxx_remap(["-std=c++11", "-DZ"], True),
        ["-std=gnu++17", "-DZ"],
    )
    self.assertEqual(
        hc.apply_ast_gnuxx_remap(["-std=c++11"], False),
        ["-std=c++11"],
    )

  def test_pp_compiler_flags_match_ast_std(self):
    ctx = _FakeCtx()
    ctx.typ = "c++"
    ctx.compilerFlags = ["-fPIC", "-std=c++11", "-c", "-o", "x.o"]
    ast = ["-std=c++14", "-DZ"]
    got = hc._pp_compiler_flags_match_ast_std(ctx, _FakeArgs(), ast, "-std=c++17")
    self.assertEqual(got[0], "-std=c++14")
    self.assertIn("-fPIC", got)
    self.assertNotIn("-std=c++11", got)

  def test_apply_ast_libstdcxx_cpp14_floor(self):
    self.assertEqual(
        hc.apply_ast_libstdcxx_cpp14_floor(["-std=c++11", "-DZ"], True),
        ["-std=c++14", "-DZ"],
    )
    self.assertEqual(
        hc.apply_ast_libstdcxx_cpp14_floor(["-std=gnu++0x"], True),
        ["-std=gnu++14"],
    )
    self.assertEqual(
        hc.apply_ast_libstdcxx_cpp14_floor(["-std=c++11"], False),
        ["-std=c++11"],
    )
    self.assertEqual(
        hc.apply_ast_libstdcxx_cpp14_floor(["-std=c++17"], True),
        ["-std=c++17"],
    )

  def test_compile_commands_argv(self):
    with tempfile.NamedTemporaryFile(mode="w", suffix=".cc", delete=False) as tu:
      tu.write("//x\n")
      src = tu.name
    try:
      db_path = src + ".json"
      entry = {
        "file": src,
        "arguments": [
            "g++",
            "-std=c++14",
            "-stdlib=libstdc++",
            "-c",
            src,
        ],
      }
      with open(db_path, "w", encoding="utf-8") as fh:
        json.dump([entry], fh)
      os.environ["HGCC_COMPILE_COMMANDS"] = db_path
      argv = hc.compile_commands_argv_for_source(src)
      self.assertIsNotNone(argv)
      self.assertIn("-std=c++14", argv)
      self.assertNotIn("-c", argv)
      self.assertFalse(any(a.endswith(".cc") and not a.startswith("-") for a in argv))
    finally:
      os.environ.pop("HGCC_COMPILE_COMMANDS", None)
      try:
        os.unlink(src)
      except OSError:
        pass
      try:
        os.unlink(db_path)
      except OSError:
        pass

  def test_conftest_strip_sst_pmi_link(self):
    flags = ["-fPIC", "-L/prefix/sst-elements-library/ext", "-lpmi", "-O0"]
    L = ["/prefix/sst-elements-library/ext", "/usr/lib"]
    l = ["pmi", "m"]
    out_f, out_L, out_l = hl._conftest_strip_sst_pmi_link(flags, L, l)
    self.assertEqual(out_f, ["-fPIC", "-O0"])
    self.assertEqual(out_L, ["/usr/lib"])
    self.assertEqual(out_l, ["m"])


if __name__ == "__main__":
  unittest.main()
