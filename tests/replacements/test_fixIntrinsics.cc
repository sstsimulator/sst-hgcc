#define ssthg_app_name test_fixIntrinsics
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that fixIntrinsics.h replacement header works correctly
#include <fixIntrinsics.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_replacements_fix_intrinsics_h
    std::cerr << "PASS: hgcc_replacements_fix_intrinsics_h is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_replacements_fix_intrinsics_h is not defined" << std::endl;
    errors++;
    #endif

    // Check that __is_aggregate is defined
    #ifdef __is_aggregate
    std::cerr << "PASS: __is_aggregate macro is defined" << std::endl;
    #else
    std::cerr << "ERROR: __is_aggregate macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that __rdtsc is redirected
    #ifdef __rdtsc
    std::cerr << "PASS: __rdtsc macro is defined (redirects to __redirect_rdtsc)" << std::endl;
    #else
    std::cerr << "ERROR: __rdtsc macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that __has_unique_object_representations is defined
    #ifdef __has_unique_object_representations
    std::cerr << "PASS: __has_unique_object_representations macro is defined" << std::endl;
    #else
    std::cerr << "ERROR: __has_unique_object_representations macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
