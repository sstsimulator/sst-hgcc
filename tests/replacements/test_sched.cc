#define ssthg_app_name test_sched
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that sched.h replacement header works correctly
// The sched.h header conditionally includes either the system header
// or the hgcc_sched.h library header based on HGCC_INSIDE_STL
#include <sched.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // The sched.h replacement checks HGCC_INSIDE_STL or HGCC_NO_REPLACEMENTS
    // If neither is set, it includes libraries/pthread/hgcc_sched.h
    // If either is set, it includes the system sched.h
    
    std::cerr << "PASS: sched.h replacement header included successfully" << std::endl;

    MPI_Finalize();
    return errors;
}
