#define ssthg_app_name test_return_symbol_macros
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that return_symbol_macros.h correctly re-defines symbols
// First define the "must return" flags that return_symbol_macros.h checks
#define hgcc_must_return_free
#define hgcc_must_return_memset
#define hgcc_must_return_memcpy

#include <return_symbol_macros.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // After including return_symbol_macros.h, the symbols should be redefined
    // The hgcc_must_return_* flags should be undefined

    #ifdef hgcc_must_return_free
    std::cerr << "ERROR: hgcc_must_return_free should be undefined after return_symbol_macros.h" << std::endl;
    errors++;
    #else
    std::cerr << "PASS: hgcc_must_return_free is undefined after return_symbol_macros.h" << std::endl;
    #endif

    // Check that free is redefined to hgcc_free
    #ifdef free
    std::cerr << "PASS: free macro is defined (redirects to hgcc_free)" << std::endl;
    #else
    std::cerr << "ERROR: free macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
