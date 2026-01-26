#define ssthg_app_name test_limits
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

// Test that linux/limits.h replacement header works correctly
#include <linux/limits.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check that PATH_MAX is defined
    #ifdef PATH_MAX
    std::cerr << "PASS: PATH_MAX is defined" << std::endl;
    // Check that PATH_MAX has the replacement value (1075)
    if (PATH_MAX == 1075) {
        std::cerr << "PASS: PATH_MAX has correct replacement value (1075)" << std::endl;
    } else {
        std::cerr << "ERROR: PATH_MAX has incorrect value (" << PATH_MAX << "), expected 1075" << std::endl;
        errors++;
    }
    #else
    std::cerr << "ERROR: PATH_MAX is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
