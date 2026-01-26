#define ssthg_app_name test_dlfcn
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that dlfcn.h replacement header works correctly
#include <dlfcn.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_dlfcn_h_included
    std::cerr << "PASS: hgcc_dlfcn_h_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_dlfcn_h_included is not defined" << std::endl;
    errors++;
    #endif

    // Check that dlopen is redefined to hgcc_dlopen
    #ifdef dlopen
    std::cerr << "PASS: dlopen macro is defined (redirects to hgcc_dlopen)" << std::endl;
    #else
    std::cerr << "ERROR: dlopen macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
