#ifndef SST_HG_REPL_SYS_CDEFS_H
#define SST_HG_REPL_SYS_CDEFS_H

#include_next <sys/cdefs.h>

/* Neuter glibc __attr_dealloc* for Clang parse. */
#if defined(__linux__)
#ifdef __attr_dealloc
#undef __attr_dealloc
#endif
#define __attr_dealloc(...)

#ifdef __attr_dealloc_fclose
#undef __attr_dealloc_fclose
#endif
#define __attr_dealloc_fclose

#ifdef __attr_dealloc_free
#undef __attr_dealloc_free
#endif
#define __attr_dealloc_free
#endif

#endif
