#define ssthg_app_name test_malloc
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that malloc.h replacement header works correctly
#include <malloc.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_malloc_included_h
    std::cerr << "PASS: hgcc_malloc_included_h is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_malloc_included_h is not defined" << std::endl;
    errors++;
    #endif

    // Check that _mm_free is redefined to hgcc_free
    #ifdef _mm_free
    std::cerr << "PASS: _mm_free macro is defined (redirects to hgcc_free)" << std::endl;
    #else
    std::cerr << "ERROR: _mm_free macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
