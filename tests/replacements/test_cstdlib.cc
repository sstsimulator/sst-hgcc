#define ssthg_app_name test_cstdlib
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that cstdlib replacement header works correctly
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_cstdlib_included_h
    std::cerr << "PASS: sstmac_cstdlib_included_h is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_cstdlib_included_h is not defined" << std::endl;
    errors++;
    #endif

    // Check that sstmac_must_return_free is set
    #ifdef sstmac_must_return_free
    std::cerr << "PASS: sstmac_must_return_free is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_must_return_free is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
