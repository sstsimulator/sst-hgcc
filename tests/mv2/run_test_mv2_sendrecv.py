#!/usr/bin/env python3
#
# SST driver: single Mercury node (Merlin), MVAPICH2 smoke test.
# Validates the full loading chain: MVAPICH2 -> PMI -> OFI -> Iris -> SUMI -> NIC.
# Requires platform_file_mv2_test.py on PYTHONPATH (same directory as this).

import os
import sys


def _preload_hg_and_user_app():
    """Optionally load Mercury + deps + libtest_mv2_sendrecv.so before import sst.

    Disabled by default. Set SST_HG_MV2_PRELOAD=1 to enable a Linux-only
    workaround: some setups run app .so static inits late relative to Mercury;
    early dlopen can force userSkeletonMainInitFxn registration.

    When enabled: load libhg, then Iris libsumi (libfabric needs SimTransport
    typeinfo from libsumi), then the app .so. All use RTLD_GLOBAL so symbols
    match the libhg used in-process.

    On macOS, ctypes dlopen of the bundle fails resolving hgcc_* (e.g. _hgcc_putenv)
    in the flat namespace; SST's normal DSO load satisfies those. Skip preload.
    """
    if sys.platform == "darwin":
        return
    flag = (os.environ.get("SST_HG_MV2_PRELOAD") or "").strip().lower()
    if flag not in ("1", "yes", "true", "on"):
        return
    try:
        import ctypes
    except ImportError:
        return
    here = os.path.dirname(os.path.abspath(__file__))
    pfx = (os.environ.get("SST_HG_PREFIX") or "").strip()
    if not pfx:
        cand = os.path.normpath(os.path.join(here, "..", "..", "..", "install"))
        libhg = os.path.join(cand, "lib", "sst-elements-library", "libhg.so")
        libhg_dy = os.path.join(cand, "lib", "sst-elements-library", "libhg.dylib")
        if os.path.isfile(libhg) or os.path.isfile(libhg_dy):
            pfx = cand
    if not pfx:
        return
    lib_dir = os.path.join(pfx, "lib", "sst-elements-library")
    ext_dir = os.path.join(lib_dir, "ext")
    sumi_so = os.path.join(ext_dir, "libsumi.so")
    app_so = os.path.join(here, "libtest_mv2_sendrecv.so")
    if not os.path.isfile(app_so):
        return
    hg = None
    for name in ("libhg.so", "libhg.dylib"):
        p = os.path.join(lib_dir, name)
        if os.path.isfile(p):
            hg = p
            break
    if not hg:
        return
    try:
        ctypes.CDLL(hg, mode=ctypes.RTLD_GLOBAL)
        if os.path.isfile(sumi_so):
            ctypes.CDLL(sumi_so, mode=ctypes.RTLD_GLOBAL)
        ctypes.CDLL(app_so, mode=ctypes.RTLD_GLOBAL)
    except OSError as err:
        raise RuntimeError(
            "Could not dlopen Mercury or libtest_mv2_sendrecv.so "
            "(check SST_HG_PREFIX=%r and LD_LIBRARY_PATH): %s" % (pfx, err)
        ) from err


_preload_hg_and_user_app()

import sst
from sst.merlin.base import *
from sst.merlin.endpoint import *
from sst.merlin.interface import *
from sst.merlin.topology import *
from sst.hg import *

if __name__ == "__main__":
    # Load platform from this script's directory when run via sst from elsewhere.
    _here = os.path.dirname(os.path.abspath(__file__))
    if _here not in sys.path:
        sys.path.insert(0, _here)

    PlatformDefinition.loadPlatformFile("platform_file_mv2_test")
    PlatformDefinition.setCurrentPlatform("platform_mv2_test")
    platform = PlatformDefinition.getCurrentPlatform()

    platform.addParamSet("operating_system", {
        "app1.name": "test_mv2_sendrecv",
        "app1.exe_library_name": "test_mv2_sendrecv",
        "app1.dependencies": ["sumi", "fabric"],
        "app1.libraries": [
            "computelibrary:ComputeLibrary",
            "macro:libfabric",
        ],
    })

    topo = topoSingle()
    topo.link_latency = "20ns"
    topo.num_ports = 32

    ep = HgJob(0, 2)

    system = System()
    system.setTopology(topo)
    system.allocateNodes(ep, "linear")

    system.build()
