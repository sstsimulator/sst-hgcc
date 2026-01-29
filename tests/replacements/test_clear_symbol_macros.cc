#define ssthg_app_name test_clear_symbol_macros
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// First define the symbols that will be undefined
#define free test_free
#define memset test_memset
#define memcpy test_memcpy

// Test that clear_symbol_macros.h correctly undefines symbols
#include <clear_symbol_macros.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // After including clear_symbol_macros.h, the macros should be undefined
    // and hgcc_must_return_* flags should be set

    #ifdef hgcc_must_return_free
    std::cerr << "PASS: hgcc_must_return_free is defined after clear_symbol_macros.h" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_must_return_free is not defined" << std::endl;
    errors++;
    #endif

    #ifdef hgcc_must_return_memset
    std::cerr << "PASS: hgcc_must_return_memset is defined after clear_symbol_macros.h" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_must_return_memset is not defined" << std::endl;
    errors++;
    #endif

    #ifdef hgcc_must_return_memcpy
    std::cerr << "PASS: hgcc_must_return_memcpy is defined after clear_symbol_macros.h" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_must_return_memcpy is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
