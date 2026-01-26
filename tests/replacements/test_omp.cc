#define ssthg_app_name test_omp
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that omp.h replacement header works correctly
#include <omp.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_replacement_omp_h
    std::cerr << "PASS: hgcc_replacement_omp_h is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_replacement_omp_h is not defined" << std::endl;
    errors++;
    #endif

    // Check that omp_get_thread_num is redefined
    #ifdef omp_get_thread_num
    std::cerr << "PASS: omp_get_thread_num macro is defined (redirects to hgcc_omp_get_thread_num)" << std::endl;
    #else
    std::cerr << "ERROR: omp_get_thread_num macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that omp_get_num_threads is redefined
    #ifdef omp_get_num_threads
    std::cerr << "PASS: omp_get_num_threads macro is defined (redirects to hgcc_omp_get_num_threads)" << std::endl;
    #else
    std::cerr << "ERROR: omp_get_num_threads macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that omp_lock_t is redefined
    #ifdef omp_lock_t
    std::cerr << "PASS: omp_lock_t macro is defined (redirects to hgcc_omp_lock_t)" << std::endl;
    #else
    std::cerr << "ERROR: omp_lock_t macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
