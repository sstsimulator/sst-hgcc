#define ssthg_app_name test_ios
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that ios replacement header works correctly
#include <ios>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_ios_included
    std::cerr << "PASS: sstmac_ios_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_ios_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
