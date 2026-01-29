#define ssthg_app_name test_time
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that time.h replacement header works correctly
#include <time.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_replacement_time_h
    std::cerr << "PASS: hgcc_replacement_time_h is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_replacement_time_h is not defined" << std::endl;
    errors++;
    #endif

    // Check that nanosleep is redefined
    #ifdef nanosleep
    std::cerr << "PASS: nanosleep macro is defined (redirects to hgcc_ts_nanosleep)" << std::endl;
    #else
    std::cerr << "ERROR: nanosleep macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that clock_gettime is redefined
    #ifdef clock_gettime
    std::cerr << "PASS: clock_gettime macro is defined (redirects to HGCC_clock_gettime)" << std::endl;
    #else
    std::cerr << "ERROR: clock_gettime macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
