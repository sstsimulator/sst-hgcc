#define ssthg_app_name test_map
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that map replacement header works correctly
#include <map>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_map_included
    std::cerr << "PASS: sstmac_map_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_map_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
