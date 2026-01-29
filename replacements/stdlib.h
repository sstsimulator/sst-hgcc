/**
Copyright 2009-2024 National Technology and Engineering Solutions of Sandia,
LLC (NTESS).  Under the terms of Contract DE-NA-0003525, the U.S. Government
retains certain rights in this software.

Sandia National Laboratories is a multimission laboratory managed and operated
by National Technology and Engineering Solutions of Sandia, LLC., a wholly
owned subsidiary of Honeywell International, Inc., for the U.S. Department of
Energy's National Nuclear Security Administration under contract DE-NA0003525.

Copyright (c) 2009-2024, NTESS

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Questions? Contact sst-macro-help@sandia.gov
*/
//because of library weirdness on some platforms
//if need malloc/calloc - only include the next file
//and don't do anything else
#ifndef hgcc_stdlib_included_h
#ifndef __need_malloc_and_calloc
#define hgcc_stdlib_included_h

#ifndef HGCC_INSIDE_STL
#define HGCC_INSIDE_STL
#include <hgcc_pthread_clear.h>
#include <clear_symbol_macros.h>
#define STDLIB_OWNS_STL
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

//gcc is an abomination, which requires these functions to be available
//because stdlib.h can turn around and include cstdlib
//because gcc is an abomination
int hgcc_atexit(void (*)());
int hgcc_on_exit(void (*)(int,void*),void*);
void hgcc_exit(int code);
#pragma sst null_ptr safe
extern void hgcc_free(void* ptr);

char* hgcc_getenv(const char* name);
int hgcc_putenv(char* input);
int hgcc_setenv(const char* name, const char* val, int overwrite);

#include_next <stdlib.h>

#pragma sst null_ptr safe
void free(void* ptr);

#ifdef __cplusplus
}
#endif

#ifndef __need_malloc_and_calloc
#define atexit hgcc_atexit
#define _exit hgcc_exit
#define on_exit hgcc_on_exit
#define getenv hgcc_getenv
#define setenv hgcc_setenv
#define putenv hgcc_putenv



#ifdef STDLIB_OWNS_STL
#undef STDLIB_OWNS_STL
#undef HGCC_INSIDE_STL
#include <hgcc_pthread_return.h>
#include <return_symbol_macros.h>
#endif

#endif 
#endif

