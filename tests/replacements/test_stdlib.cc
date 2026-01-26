#define ssthg_app_name test_stdlib
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that stdlib.h replacement header works correctly
#include <stdlib.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_stdlib_included_h
    std::cerr << "PASS: hgcc_stdlib_included_h is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_stdlib_included_h is not defined" << std::endl;
    errors++;
    #endif

    // Check that atexit is redefined
    #ifdef atexit
    std::cerr << "PASS: atexit macro is defined (redirects to hgcc_atexit)" << std::endl;
    #else
    std::cerr << "ERROR: atexit macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that getenv is redefined
    #ifdef getenv
    std::cerr << "PASS: getenv macro is defined (redirects to hgcc_getenv)" << std::endl;
    #else
    std::cerr << "ERROR: getenv macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that setenv is redefined
    #ifdef setenv
    std::cerr << "PASS: setenv macro is defined (redirects to hgcc_setenv)" << std::endl;
    #else
    std::cerr << "ERROR: setenv macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
