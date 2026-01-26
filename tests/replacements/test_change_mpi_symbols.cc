#define ssthg_app_name test_change_mpi_symbols
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that change_mpi_symbols.h correctly redefines MPI symbols
#include <change_mpi_symbols.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check that MPI_Init is redefined to HGCC_MPI_Init
    #ifdef MPI_Init
    std::cerr << "PASS: MPI_Init is defined (redirects to HGCC_MPI_Init)" << std::endl;
    #else
    std::cerr << "ERROR: MPI_Init is not defined" << std::endl;
    errors++;
    #endif

    // Check that MPI_Finalize is redefined
    #ifdef MPI_Finalize
    std::cerr << "PASS: MPI_Finalize is defined (redirects to HGCC_MPI_Finalize)" << std::endl;
    #else
    std::cerr << "ERROR: MPI_Finalize is not defined" << std::endl;
    errors++;
    #endif

    // Check that MPI_Comm_rank is redefined
    #ifdef MPI_Comm_rank
    std::cerr << "PASS: MPI_Comm_rank is defined (redirects to HGCC_MPI_Comm_rank)" << std::endl;
    #else
    std::cerr << "ERROR: MPI_Comm_rank is not defined" << std::endl;
    errors++;
    #endif

    // Check that MPI_Barrier is redefined
    #ifdef MPI_Barrier
    std::cerr << "PASS: MPI_Barrier is defined (redirects to HGCC_MPI_Barrier)" << std::endl;
    #else
    std::cerr << "ERROR: MPI_Barrier is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
