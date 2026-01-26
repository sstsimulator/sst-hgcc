#define ssthg_app_name test_unistd
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that unistd.h replacement header works correctly
#include <unistd.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef hgcc_unistd_h
    std::cerr << "PASS: hgcc_unistd_h is defined" << std::endl;
    #else
    std::cerr << "ERROR: hgcc_unistd_h is not defined" << std::endl;
    errors++;
    #endif

    // Check that sleep is redefined
    #ifdef sleep
    std::cerr << "PASS: sleep macro is defined (redirects to hgcc_sleep)" << std::endl;
    #else
    std::cerr << "ERROR: sleep macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that gethostname is redefined
    #ifdef gethostname
    std::cerr << "PASS: gethostname macro is defined (redirects to hgcc_gethostname)" << std::endl;
    #else
    std::cerr << "ERROR: gethostname macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that alarm is redefined
    #ifdef alarm
    std::cerr << "PASS: alarm macro is defined (redirects to hgcc_alarm)" << std::endl;
    #else
    std::cerr << "ERROR: alarm macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
