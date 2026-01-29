#define ssthg_app_name test_pthread
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that pthread.h replacement header works correctly
// The pthread.h header conditionally includes either the system header
// or the hgcc_pthread.h library header based on HGCC_INSIDE_STL
#include <pthread.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // The pthread.h replacement checks HGCC_INSIDE_STL or HGCC_NO_REPLACEMENTS
    // If neither is set, it includes libraries/pthread/hgcc_pthread.h
    // If either is set, it includes the system pthread.h
    
    std::cerr << "PASS: pthread.h replacement header included successfully" << std::endl;

    MPI_Finalize();
    return errors;
}
