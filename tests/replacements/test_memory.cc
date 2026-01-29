#define ssthg_app_name test_memory
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that memory replacement header works correctly
#include <memory>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef memory_header_included
    std::cerr << "PASS: memory_header_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: memory_header_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
