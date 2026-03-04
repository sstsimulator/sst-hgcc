#define ssthg_app_name test_pthread
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that pthread.h replacement header works correctly
// The pthread.h replacement includes system pthread.h then hgcc_pthread_spinlock.h
// (e.g. pthread_spinlock_t on macOS). HGCC_INSIDE_STL / HGCC_NO_REPLACEMENTS use system only.
#include <pthread.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;
    
    std::cerr << "PASS: pthread.h replacement header included successfully" << std::endl;

    MPI_Finalize();
    return errors;
}
