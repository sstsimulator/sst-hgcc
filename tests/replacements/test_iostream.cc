#define ssthg_app_name test_iostream
#include <skeleton.h>

#include <mask_mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int errors = 0;

    // Check the header guard macro
    #ifdef sstmac_iostream_included
    std::cerr << "PASS: sstmac_iostream_included is defined" << std::endl;
    #else
    std::cerr << "ERROR: sstmac_iostream_included is not defined" << std::endl;
    errors++;
    #endif

    // Check that cout macro is defined (redirects to cout_wrapper())
    #ifdef cout
    std::cerr << "PASS: cout macro is defined (redirects to cout_wrapper())" << std::endl;
    #else
    std::cerr << "ERROR: cout macro is not defined" << std::endl;
    errors++;
    #endif

    // Check that cerr macro is defined (redirects to cerr_wrapper())
    #ifdef cerr
    std::cerr << "PASS: cerr macro is defined (redirects to cerr_wrapper())" << std::endl;
    #else
    std::cerr << "ERROR: cerr macro is not defined" << std::endl;
    errors++;
    #endif

    MPI_Finalize();
    return errors;
}
