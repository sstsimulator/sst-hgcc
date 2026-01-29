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
#ifndef hgcc_replacement_omp_h
#define hgcc_replacement_omp_h


#define omp_init_lock hgcc_omp_init_lock
#define omp_destroy_lock hgcc_omp_destroy_lock
#define omp_set_lock hgcc_omp_set_lock
#define omp_unset_lock hgcc_omp_unset_lock
#define omp_test_lock hgcc_omp_test_lock
#define omp_get_num_threads hgcc_omp_get_num_threads
#define omp_get_thread_num hgcc_omp_get_thread_num
#define omp_get_max_threads hgcc_omp_get_max_threads
#define omp_get_wtime hgcc_omp_get_wtime
#define omp_get_num_procs hgcc_omp_get_num_procs
#define omp_set_num_threads hgcc_omp_set_num_threads
#define omp_in_parallel hgcc_omp_in_parallel
#define omp_get_level hgcc_omp_get_level
#define omp_get_ancestor_thread_num hgcc_omp_get_ancestor_thread_num


#define hgcc_omp_lock_t int

#ifdef __cplusplus
extern "C" {
#endif

void hgcc_omp_init_lock(hgcc_omp_lock_t *lock);

void hgcc_omp_destroy_lock(hgcc_omp_lock_t *lock);

void hgcc_omp_set_lock(hgcc_omp_lock_t *lock);

void hgcc_omp_unset_lock(hgcc_omp_lock_t *lock);

int hgcc_omp_test_lock(hgcc_omp_lock_t *lock);

int hgcc_omp_get_thread_num();

int hgcc_omp_get_num_procs();

int hgcc_omp_get_num_threads();

int hgcc_omp_get_max_threads();

void hgcc_omp_set_num_threads(int nthr);

int hgcc_omp_in_parallel();

int hgcc_omp_get_level();

int hgcc_omp_get_ancestor_thread_num();

double hgcc_omp_get_wtime();

#define omp_lock_t hgcc_omp_lock_t

#ifdef __cplusplus
}
#endif

#endif

