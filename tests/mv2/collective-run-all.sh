#!/usr/bin/env bash
# Build and run every MV2 collective test; report a pass/fail summary.
#
# Usage:
#   ./collective-run-all.sh                 # run all collectives
#   ./collective-run-all.sh bcast allreduce # run a subset
#   FAIL_FAST=1 ./collective-run-all.sh     # abort on first failure
#
# Exit status: 0 iff every selected collective passed build and run.

set -u
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

ALL_OPS=(barrier bcast reduce allreduce gather allgather scatter alltoall reduce_scatter)

if [[ $# -gt 0 ]]; then
  OPS=("$@")
else
  OPS=("${ALL_OPS[@]}")
fi

FAIL_FAST="${FAIL_FAST:-0}"

declare -a results
rc_total=0

for op in "${OPS[@]}"; do
  name="test_mv2_${op}"
  build_sh="./build_${name}.sh"
  run_sh="./run_${name}.sh"

  if [[ ! -x "$build_sh" || ! -x "$run_sh" ]]; then
    echo "!!! [${name}] missing or non-executable wrapper scripts; skipping"
    results+=("SKIP        ${name}")
    rc_total=1
    [[ "$FAIL_FAST" == 1 ]] && break
    continue
  fi

  echo
  echo "=== [${name}] build ==="
  if ! "$build_sh"; then
    results+=("FAIL(build) ${name}")
    rc_total=1
    [[ "$FAIL_FAST" == 1 ]] && break
    continue
  fi

  echo
  echo "=== [${name}] run ==="
  if "$run_sh"; then
    results+=("PASS        ${name}")
  else
    results+=("FAIL(run)   ${name}")
    rc_total=1
    [[ "$FAIL_FAST" == 1 ]] && break
  fi
done

echo
echo "===== MV2 collective summary ====="
printf '  %s\n' "${results[@]}"
exit "$rc_total"
