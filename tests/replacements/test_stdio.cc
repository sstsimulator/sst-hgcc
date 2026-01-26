#define ssthg_app_name test_stdio
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that stdio.h replacement header works correctly
#include <stdio.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check that stdout macro is redefined
    #ifdef stdout
    std::cerr << "PASS: stdout macro is defined (redirects to hgcc_stdout())" << std::endl;
    #else
    std::cerr << "ERROR: stdout macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that stderr macro is redefined
    #ifdef stderr
    std::cerr << "PASS: stderr macro is defined (redirects to hgcc_stderr())" << std::endl;
    #else
    std::cerr << "ERROR: stderr macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that printf macro is redefined
    #ifdef printf
    std::cerr << "PASS: printf macro is defined" << std::endl;
    #else
    std::cerr << "ERROR: printf macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
