

#if defined(__linux__) && defined(__GLIBC__)

#include <stdint.h>

#define MV2_LD_OFFSET_STUB(name)                                               \
  __attribute__((visibility("default"))) uintptr_t name = 0

MV2_LD_OFFSET_STUB(__offset_program_invocation_name);

#endif
