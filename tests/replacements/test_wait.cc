#define ssthg_app_name test_wait
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that wait.h replacement header works correctly
// The wait.h replacement simply includes sys/wait.h
#include <wait.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // The wait.h replacement simply forwards to sys/wait.h
    // Just verify inclusion worked
    std::cerr << "PASS: wait.h replacement header included successfully" << std::endl;

    MPI_Finalize();
    return errors;
}
