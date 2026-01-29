#define ssthg_app_name test_stringh
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that string.h replacement header works correctly
#include <string.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_string_h_included
    std::cerr << "PASS: hgcc_string_h_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_string_h_included is not defined" << std::endl;
    errors++;
    #endif

    // Check that memset is redefined
    #ifdef memset
    std::cerr << "PASS: memset macro is defined (redirects to hgcc_memset)" << std::endl;
    #else
    std::cerr << "ERROR: memset macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that memcpy is redefined
    #ifdef memcpy
    std::cerr << "PASS: memcpy macro is defined (redirects to hgcc_memcpy)" << std::endl;
    #else
    std::cerr << "ERROR: memcpy macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
