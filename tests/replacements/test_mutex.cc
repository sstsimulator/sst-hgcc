#define ssthg_app_name test_mutex
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that mutex replacement header works correctly
#include <mutex>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_mutex_header_included
    std::cerr << "PASS: sstmac_mutex_header_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_mutex_header_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
