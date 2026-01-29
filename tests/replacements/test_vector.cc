#define ssthg_app_name test_vector
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that vector replacement header works correctly
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_vector_included
    std::cerr << "PASS: sstmac_vector_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_vector_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
