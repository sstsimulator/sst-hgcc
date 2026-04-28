// #pragma sst overhead <param> -- inserts `ssthg_advance_time("<param>");`.

// RUN: rm -rf %t.d && mkdir -p %t.d
// RUN: %hgcc -c %s -o %t.d/out.o || true
// RUN: %FileCheck %s --input-file=%t.d/sst.pp.overhead.cc

void sst_hg_lit_overhead() {
  int x = 0;
#pragma sst overhead sst_hg_lit_ov
  x = 1;
  (void)x;
}

// CHECK-LABEL: sst_hg_lit_overhead
// CHECK: ssthg_advance_time("sst_hg_lit_ov")
