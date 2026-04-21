import os
import shutil

import lit.formats

config.name = "sst-hgcc"
config.test_format = lit.formats.ShTest(execute_external=True)
config.suffixes = [".cc", ".c"]
config.test_source_root = os.path.dirname(os.path.realpath(__file__))
config.test_exec_root = os.path.join(config.test_source_root, "Output")


def _resolve(env_name, tool_name):
    path = os.environ.get(env_name)
    if path and os.path.isfile(path) and os.access(path, os.X_OK):
        return path
    return shutil.which(tool_name)


hgcc = _resolve("HGCC", "hgcc")
filecheck = _resolve("FILECHECK", "FileCheck")

missing = []
if not hgcc:
    missing.append("hgcc (install sst-hgcc or set $HGCC)")
if not filecheck:
    missing.append("FileCheck (install LLVM tools or set $FILECHECK)")

if missing:
    config.unsupported = True
    lit_config.warning(
        "sst-hgcc lit-tests disabled; missing: " + ", ".join(missing)
    )
else:
    config.substitutions.append(("%hgcc", hgcc))
    config.substitutions.append(("%FileCheck", filecheck))

    for var in (
        "PATH",
        "HOME",
        "TMPDIR",
        "CC",
        "CXX",
        "LLVM_ROOT",
        "SST_HG_INSTALL_DIR",
        "SST_HG_CONFIG",
        "SDKROOT",
        "MACOSX_DEPLOYMENT_TARGET",
        "SSTHG_SYSTEM_INCLUDES",
        "CPATH",
        "LIBRARY_PATH",
    ):
        if var in os.environ:
            config.environment[var] = os.environ[var]

    config.environment["SST_HG_DELETE_TEMP_SOURCES"] = "0"
    config.environment["SST_HG_DELETE_TEMP_OBJECTS"] = "0"
    config.environment["SST_HG_VERBOSE"] = "0"
    config.environment["SST_HG_SKELETONIZE"] = "1"
