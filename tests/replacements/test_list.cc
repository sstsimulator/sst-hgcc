#define ssthg_app_name test_list
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that list replacement header works correctly
#include <list>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_list_included
    std::cerr << "PASS: sstmac_list_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_list_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
