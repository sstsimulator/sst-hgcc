#define ssthg_app_name test_sstream
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that sstream replacement header works correctly
#include <sstream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_sstream_included
    std::cerr << "PASS: sstmac_sstream_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_sstream_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
