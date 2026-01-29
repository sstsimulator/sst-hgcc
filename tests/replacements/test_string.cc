#define ssthg_app_name test_string
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that string replacement header works correctly
#include <string>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_string_included
    std::cerr << "PASS: sstmac_string_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_string_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
