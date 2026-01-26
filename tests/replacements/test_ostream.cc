#define ssthg_app_name test_ostream
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that ostream replacement header works correctly
#include <ostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_ostream_included
    std::cerr << "PASS: sstmac_ostream_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_ostream_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
