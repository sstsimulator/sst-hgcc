/* Static libmpi.a can reference glibc internals used with program_invocation_name
 * when those .o files were built as PIC. Linking them into a shared library does
 * not get normal executable crt/init, so dlopen() can fail with:
 *   undefined symbol: __offset_program_invocation_name
 * Provide a zero definition so the DSO loads. Keep this file minimal: only symbols
 * proven undefined by the linker (avoid extra __offset_* that can confuse reloc). */

#if defined(__linux__) && defined(__GLIBC__)

#include <stdint.h>

#define MV2_LD_OFFSET_STUB(name)                                               \
  __attribute__((visibility("default"))) uintptr_t name = 0

MV2_LD_OFFSET_STUB(__offset_program_invocation_name);

#endif
