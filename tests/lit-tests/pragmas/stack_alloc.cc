// #pragma sst stack_alloc alloc(<size>) -- rewrites the init RHS to `ssthg_alloc_stack(...)`.

// RUN: rm -rf %t.d && mkdir -p %t.d
// RUN: %hgcc -c %s -o %t.d/out.o || true
// RUN: %FileCheck %s --input-file=%t.d/sst.pp.stack_alloc.cc

void sst_hg_lit_stack_alloc() {
#pragma sst stack_alloc alloc(4096)
  void* sp = (void*)0;
  (void)sp;
}

// CHECK-LABEL: sst_hg_lit_stack_alloc
// CHECK: ssthg_alloc_stack(4096
