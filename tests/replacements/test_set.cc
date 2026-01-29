#define ssthg_app_name test_set
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that set replacement header works correctly
#include <set>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_set_included
    std::cerr << "PASS: sstmac_set_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_set_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
