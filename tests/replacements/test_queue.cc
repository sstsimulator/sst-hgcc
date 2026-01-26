#define ssthg_app_name test_queue
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that queue replacement header works correctly
#include <queue>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_queue_included
    std::cerr << "PASS: sstmac_queue_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_queue_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
