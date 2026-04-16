#!/usr/bin/env bash
# Build MVAPICH2 smoke test with hg++ and run under SST.
# Validates the full loading chain: MVAPICH2 (static) -> PMI (SST) -> OFI
# netmod -> Iris libfabric (SUMI provider) -> Mercury NIC -> Merlin.
#
# Two simulated MPI ranks in one SST process: hgcc relocates MVAPICH2 globals;
# PMI reports per-rank identity via the libfabric transport.
#
# Usage:
#   ./run_test_mv2_sendrecv.sh [PREFIX]
#   PREFIX defaults to $SST_HG_PREFIX or ../../../install from this directory.
#
# Optional: SST_HG_MV2_PRELOAD=1 enables Python ctypes preload of libhg + libsumi +
# libtest_mv2_sendrecv.so before import sst (see run_test_mv2_sendrecv.py). Default
# is SST-only dlopen once Mercury loader dependency order is fixed.
#
# SST is often linked against libpython; if you see "error while loading shared
# libraries: libpython3.x.so", prepend that Python's lib directory to
# LD_LIBRARY_PATH (e.g. the lib/ next to the interpreter used to build sst-core).
#
# Follow-on (Mercury / SST-DL, sst-elements): prefer loader order so Iris libsumi
# is mapped before libfabric and app .so static inits run before UserAppCxxFullMain
# lookup—then run_test_mv2_sendrecv.py can rely on SST-only dlopen without ctypes
# preload (see SST_HG_MV2_PRELOAD in that script). See comment in app.cc.
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

PREFIX="${1:-${SST_HG_PREFIX:-$SCRIPT_DIR/../../../install}}"
if [[ ! -d "$PREFIX" ]]; then
  echo "Error: PREFIX not found: $PREFIX"
  echo "Usage: $0 [PREFIX]"
  exit 1
fi
PREFIX="$(cd "$PREFIX" && pwd)"

export PATH="$PREFIX/bin:$PATH"

if ! command -v hgcc >/dev/null 2>&1; then
  echo "Error: hgcc not found. Add $PREFIX/bin to PATH or set PREFIX."
  exit 1
fi

MV2_LIB="$PREFIX/mvapich2/lib"
MV2_INC="$PREFIX/mvapich2/include"
HG_ELEMS_LIB="$PREFIX/lib/sst-elements-library"
OFI_EXT_LIB="$HG_ELEMS_LIB/ext"

if [[ ! -f "$MV2_LIB/libmpi.a" ]]; then
  echo "Error: $MV2_LIB/libmpi.a not found. Build mvapich2 with hgcc first."
  exit 1
fi
if [[ ! -d "$OFI_EXT_LIB" ]] || ! ls "$OFI_EXT_LIB"/libfabric.* 1>/dev/null 2>&1; then
  echo "Error: Iris libfabric not found under $OFI_EXT_LIB. Build sst-elements first."
  exit 1
fi
if ! ls "$OFI_EXT_LIB"/libpmi.* 1>/dev/null 2>&1; then
  echo "Error: Iris PMI (libpmi) not found under $OFI_EXT_LIB. Build sst-elements (iris) first."
  exit 1
fi

# Step 1: Compile with hgcc (injects skeleton.h, pthread replacement headers).
# Step 2: Compile MPI wrappers (macOS only — provides strong global MPI_*
#         symbols to override the local weak aliases from #pragma weak).
# Step 3: Link manually with -force_load to pull all archive members and
#         the strong wrappers that ensure MPI_* resolves inside the bundle.
echo "Building libtest_mv2_sendrecv ..."
hgcc --replacements pthread.h -std=gnu11 -c test_mv2_sendrecv.c -I "$MV2_INC"

# hgcc embeds app registration under a path-mangled name inside the .o.
# Generate a tiny C++ shim that re-registers the same function under the
# short app name the SST Python driver expects ("test_mv2_sendrecv").
# Darwin: nm -gU = externals that are defined. GNU nm: -U means --unicode; use
# --defined-only instead.
if [[ "$(uname -s)" == Darwin ]]; then
  _nm_def=(nm -gU)
else
  _nm_def=(nm -g --defined-only)
fi
MAIN_SYM=$("${_nm_def[@]}" test_mv2_sendrecv.o \
  | awk '/sst_hg_user_main/{print $3; exit}' | sed 's/^_//')
if [[ -z "$MAIN_SYM" ]]; then
  echo "Error: could not find sst_hg_user_main symbol in test_mv2_sendrecv.o"
  exit 1
fi
cat > register_app_gen.cpp <<CPPEOF
extern "C" int ${MAIN_SYM}(int, char**);
typedef int (*main_fxn)(int, char**);
extern int userSkeletonMainInitFxn(const char* name, main_fxn fxn);
static int _reg = userSkeletonMainInitFxn("test_mv2_sendrecv", ${MAIN_SYM});
CPPEOF
clang++ -std=c++11 -c register_app_gen.cpp \
  -I"$PREFIX/include/sst/elements"

# Strip MVAPICH2/MPICH embedded simple PMI client so the SST PMI library is used.
# Object names inside libmpi.a vary by version and platform (e.g. lib_libmpi_la-* vs
# other prefixes); remove every archive member whose name contains simple_pmi.
LIBMPI_NOPMI="$SCRIPT_DIR/libmpi_nopmi.a"
cp "$MV2_LIB/libmpi.a" "$LIBMPI_NOPMI"
while IFS= read -r _mem; do
  [[ -z "$_mem" ]] && continue
  ar -d "$LIBMPI_NOPMI" "$_mem" 2>/dev/null || true
done < <(ar t "$LIBMPI_NOPMI" 2>/dev/null | grep -i 'simple_pmi' || true)

if [[ "$(uname -s)" == Darwin ]]; then
  gcc -c mv2_mpi_wrappers.c -I "$MV2_INC"
  gcc -bundle -undefined dynamic_lookup \
    -Wl,-rpath,"$PREFIX/lib" \
    -Wl,-rpath,"$OFI_EXT_LIB" \
    -Wl,-force_load,"$LIBMPI_NOPMI" \
    -L "$OFI_EXT_LIB" -lpmi -lfabric \
    mv2_mpi_wrappers.o register_app_gen.o test_mv2_sendrecv.o \
    -o libtest_mv2_sendrecv.so
  rm -f mv2_mpi_wrappers.o
else
  # Link libhg so userSkeletonMainInitFxn resolves and the loader can find Mercury
  # (register_app_gen calls into libhg.so). Without this, dlopen(libtest_...) can fail
  # silently during SST requireLibrary and no main is registered ("no main_fxns_").
  _stub_cflags=(-fPIC)
  # Some hosts define __linux__ but not __GLIBC__ when preprocessing the stub; keep body.
  if [[ "$(uname -s)" == Linux ]] && command -v gcc >/dev/null 2>&1; then
    _dM=$(echo | gcc -dM -E - 2>/dev/null || true)
    if echo "$_dM" | grep -q '#define __linux__' && ! echo "$_dM" | grep -q '#define __GLIBC__'; then
      _stub_cflags+=(-D__GLIBC__=1)
    fi
  fi
  gcc "${_stub_cflags[@]}" -c "$SCRIPT_DIR/mv2_ld_glibc_stub.c" -o "$SCRIPT_DIR/mv2_ld_glibc_stub.o"
  # mv2_mpi_wrappers.c provides hgcc_gethostname / hgcc_gethostid / hgcc_usleep
  # stubs that hgcc-rewritten code in libmpi.a references via replacements/unistd.h.
  # (The MPI_* -> PMPI_* wrappers in the same file are no-ops on Linux once the
  # MVAPICH2 weak-alias #pragmas take effect, but they are harmless.)
  gcc -fPIC -c mv2_mpi_wrappers.c -I "$MV2_INC" -o "$SCRIPT_DIR/mv2_mpi_wrappers.o"
  # -Bsymbolic-functions: prefer our own (MVAPICH2) MPI_* function symbols over
  # any MPI_* the host sst process already loaded (e.g. Open MPI pulled in as an
  # MCA component). Without this the DSO's MPI_Init call resolves to Open MPI's
  # MPI_Init, which is already initialized and errors "MPI_INIT called more than
  # once". Use the -functions variant rather than plain -Bsymbolic so extern DATA
  # symbols (e.g. SST::Hg::GlobalVariable::glblCtx, sst_hg_global_stacksize) keep
  # resolving to libhg's single-instance definitions instead of getting a local
  # copy in the DSO (which would make the per-rank globals segment diverge from
  # the one libhg uses to allocate it, causing OOB writes).
  gcc -shared \
    -Wl,-Bsymbolic-functions \
    -Wl,-rpath,"$PREFIX/lib" \
    -Wl,-rpath,"$HG_ELEMS_LIB" \
    -Wl,-rpath,"$OFI_EXT_LIB" \
    -Wl,--whole-archive "$LIBMPI_NOPMI" -Wl,--no-whole-archive \
    -L "$OFI_EXT_LIB" -lpmi -lfabric \
    -L "$HG_ELEMS_LIB" -Wl,--no-as-needed -lhg -Wl,--as-needed \
    "$SCRIPT_DIR/mv2_ld_glibc_stub.o" \
    "$SCRIPT_DIR/mv2_mpi_wrappers.o" \
    register_app_gen.o test_mv2_sendrecv.o -o libtest_mv2_sendrecv.so
  rm -f "$SCRIPT_DIR/mv2_ld_glibc_stub.o" "$SCRIPT_DIR/mv2_mpi_wrappers.o"
fi
rm -f "$LIBMPI_NOPMI"
rm -f test_mv2_sendrecv.o register_app_gen.o register_app_gen.cpp

if [[ "$(uname -s)" == Linux ]] && command -v ldd >/dev/null 2>&1; then
  _bad=$(ldd "$SCRIPT_DIR/libtest_mv2_sendrecv.so" 2>/dev/null | grep 'not found' || true)
  if [[ -n "$_bad" ]]; then
    echo "Error: libtest_mv2_sendrecv.so has unresolved loader dependencies:"
    echo "$_bad"
    echo "Hint: ensure LD_LIBRARY_PATH includes $HG_ELEMS_LIB (Mercury libhg.so)."
    exit 1
  fi
  # Do not use ldd -r here: it resolves without the sst process / libsst loaded and
  # falsely reports SST::*, hgcc_*, and transitive deps (e.g. libhg -> libsst).
fi

echo "Running SST ..."
export SST_HG_PREFIX="$PREFIX"
export FI_PROVIDER=sstmac
export SST_LIB_PATH="$SCRIPT_DIR:$OFI_EXT_LIB:${SST_LIB_PATH:-}"
# Mercury (libhg), Iris ext libs, and the test .so must all resolve at dlopen time.
export LD_LIBRARY_PATH="$HG_ELEMS_LIB:$PREFIX/lib:$OFI_EXT_LIB:$SCRIPT_DIR:${LD_LIBRARY_PATH:-}"
export DYLD_LIBRARY_PATH="$PREFIX/lib:$OFI_EXT_LIB:$SCRIPT_DIR:${DYLD_LIBRARY_PATH:-}"
export PYTHONPATH="$SCRIPT_DIR:${PYTHONPATH:-}"

if ! command -v sst >/dev/null 2>&1; then
  echo "Error: sst not found in PATH."
  exit 1
fi

# Fail early with a clear hint if the dynamic linker cannot resolve sst's deps
# (GNU ldd reports "not found"; macOS uses different tooling — skip there).
if [[ "$(uname -s)" == Linux ]] && command -v ldd >/dev/null 2>&1; then
  _sst_bin=$(command -v sst)
  _ldd_bad=$(ldd "$_sst_bin" 2>/dev/null | grep 'not found' || true)
  if [[ -n "$_ldd_bad" ]]; then
    echo "Error: sst cannot load one or more shared libraries:"
    echo "$_ldd_bad"
    if echo "$_ldd_bad" | grep -q libpython; then
      echo "Fix: prepend the directory containing that libpython*.so to LD_LIBRARY_PATH (the Python tree SST was built with)."
      echo "  Example:  export LD_LIBRARY_PATH=\"/path/to/python/lib:\${LD_LIBRARY_PATH}\""
    fi
    exit 1
  fi
fi

# Preflight resource check. SST + MVAPICH2 globals + Mercury stacks easily
# need several hundred MB; shared front-end / login nodes often have tight
# cgroup / ulimit caps that SIGKILL the process mid-MPI_Init with a segfault
# that looks like a NULL deref. Catch that before we spend 10s loading plugins.
# Override threshold with SST_HG_MV2_MIN_MEM_MB; skip entirely with
# SST_HG_MV2_SKIP_PREFLIGHT=1.
_min_mem_mb="${SST_HG_MV2_MIN_MEM_MB:-1024}"
if [[ "${SST_HG_MV2_SKIP_PREFLIGHT:-0}" != 1 ]]; then
  _pf_warn=""
  _pf_fatal=""

  # Available physical memory.
  _avail_mb=""
  if [[ -r /proc/meminfo ]]; then
    _avail_kb=$(awk '/^MemAvailable:/ {print $2; exit}' /proc/meminfo 2>/dev/null || true)
    [[ -n "$_avail_kb" ]] && _avail_mb=$(( _avail_kb / 1024 ))
  elif [[ "$(uname -s)" == Darwin ]] && command -v vm_stat >/dev/null 2>&1; then
    _page_bytes=$(vm_stat | awk '/page size of/ {print $8; exit}' 2>/dev/null || echo 4096)
    _free_pages=$(vm_stat | awk '/Pages free/ {gsub(/\./,""); print $3; exit}' 2>/dev/null || echo 0)
    _inact_pages=$(vm_stat | awk '/Pages inactive/ {gsub(/\./,""); print $3; exit}' 2>/dev/null || echo 0)
    _avail_mb=$(( (_free_pages + _inact_pages) * _page_bytes / 1024 / 1024 ))
  fi
  if [[ -n "$_avail_mb" && "$_avail_mb" -lt "$_min_mem_mb" ]]; then
    _pf_fatal+="  available RAM ${_avail_mb} MB < required ${_min_mem_mb} MB"$'\n'
  fi

  # Virtual-memory ulimit (cgroup-imposed caps often show up here).
  _vlim=$(ulimit -v 2>/dev/null || echo unlimited)
  if [[ "$_vlim" != unlimited && -n "$_vlim" ]]; then
    _vlim_mb=$(( _vlim / 1024 ))
    if [[ "$_vlim_mb" -lt "$_min_mem_mb" ]]; then
      _pf_fatal+="  ulimit -v = ${_vlim_mb} MB < required ${_min_mem_mb} MB"$'\n'
    fi
  fi

  # cgroup v2 memory.max (common on newer clusters' login nodes).
  if [[ -r /sys/fs/cgroup/memory.max ]]; then
    _cg_max=$(cat /sys/fs/cgroup/memory.max 2>/dev/null)
    if [[ "$_cg_max" =~ ^[0-9]+$ ]]; then
      _cg_mb=$(( _cg_max / 1024 / 1024 ))
      if [[ "$_cg_mb" -lt "$_min_mem_mb" ]]; then
        _pf_fatal+="  cgroup memory.max = ${_cg_mb} MB < required ${_min_mem_mb} MB"$'\n'
      fi
    fi
  fi

  # Not inside a scheduler allocation on Linux -> warn (not fatal).
  if [[ "$(uname -s)" == Linux ]]; then
    if [[ -z "${SLURM_JOB_ID:-}${PBS_JOBID:-}${LSB_JOBID:-}${FLUX_JOB_ID:-}" ]]; then
      _pf_warn+="  not inside a SLURM/PBS/LSF/Flux job; you may be on a shared front-end"$'\n'
    fi
  fi

  if [[ -n "$_pf_fatal" ]]; then
    echo "Error: preflight resource check failed:"
    echo -n "$_pf_fatal"
    echo "Fix: run inside a compute allocation (e.g. salloc -N1 --mem=4G)."
    echo "     Or override:  SST_HG_MV2_MIN_MEM_MB=<mb>  or  SST_HG_MV2_SKIP_PREFLIGHT=1"
    exit 1
  fi
  if [[ -n "$_pf_warn" ]]; then
    echo "Warning: preflight resource check:"
    echo -n "$_pf_warn"
    [[ -n "$_avail_mb" ]] && echo "  (MemAvailable ${_avail_mb} MB, threshold ${_min_mem_mb} MB)"
  fi
fi

sst --verbose run_test_mv2_sendrecv.py
echo "PASS: run_test_mv2_sendrecv completed."
