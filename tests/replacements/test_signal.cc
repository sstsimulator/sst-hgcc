#define ssthg_app_name test_signal
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that signal.h replacement header works correctly
#include <signal.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_signal_h_included
    std::cerr << "PASS: hgcc_signal_h_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_signal_h_included is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
