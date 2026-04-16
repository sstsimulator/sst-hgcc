#define ssthg_app_name test_pthread
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// pthread.h replacement: system pthread + macOS spinlock shim when applicable.
#include <pthread.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;
    
    std::cerr << "PASS: pthread.h replacement header included successfully" << std::endl;

    MPI_Finalize();
    return errors;
}
