#define ssthg_app_name test_istream
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that istream replacement header works correctly
#include <istream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_istream_included
    std::cerr << "PASS: sstmac_istream_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_istream_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
