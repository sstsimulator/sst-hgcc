#define ssthg_app_name test_cstring
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that cstring replacement header works correctly
#include <cstring>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef cstring_header_h_included
    std::cerr << "PASS: cstring_header_h_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: cstring_header_h_included is not defined" << std::endl;
    errors++;
    #endif

    // Check that sstmac_must_return_memcpy is set
    #ifdef sstmac_must_return_memcpy
    std::cerr << "PASS: sstmac_must_return_memcpy is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_must_return_memcpy is not defined" << std::endl;
    errors++;
    #endif

    // Check that sstmac_must_return_memset is set
    #ifdef sstmac_must_return_memset
    std::cerr << "PASS: sstmac_must_return_memset is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_must_return_memset is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
