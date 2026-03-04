#ifndef HGCC_SYS_TYPES_H
#define HGCC_SYS_TYPES_H

#ifndef HGCC_INSIDE_STL
#define HGCC_INSIDE_STL

#include <ssthg_pthread_clear.h>

#include_next <sys/types.h>
#undef HGCC_INSIDE_STL

#include <hgcc_pthread_return.h>

#else
#include_next <sys/types.h>
#endif

#endif
