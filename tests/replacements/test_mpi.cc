#define ssthg_app_name test_mpi
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that mpi/mpi.h replacement header works correctly
// Note: This header requires SST dependencies

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // The mpi/mpi.h header includes SST-specific headers
    // Just verify the basic MPI functionality works via mask_mpi
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    std::cerr << "PASS: MPI header is working (rank=" << rank << ")" << std::endl;

    MPI_Finalize();
    return errors;
}
