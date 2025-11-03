#include <iostream>
#include <limits.h>  // Standard system limits.h
#include <unistd.h>
#include <cstring>

int main() {
    std::cout << "Testing SST-HGCC with limits.h..." << std::endl;
    
    // Check which limits.h was used by examining the values
    std::cout << "\n=== Limits.h Version Detection ===" << std::endl;
    std::cout << "PATH_MAX = " << PATH_MAX << std::endl;
    std::cout << "NAME_MAX = " << NAME_MAX << std::endl;
    
    // Test PATH_MAX constant
    std::cout << "PATH_MAX = " << PATH_MAX << std::endl;
    
    // Test with getcwd function
    char current_path[PATH_MAX];
    if (getcwd(current_path, sizeof(current_path)) != nullptr) {
        std::cout << "Current directory: " << current_path << std::endl;
        std::cout << "Path length: " << strlen(current_path) << " characters" << std::endl;
        
        if (strlen(current_path) < PATH_MAX) {
            std::cout << "Path length is within PATH_MAX limit" << std::endl;
        } else {
            std::cout << "Path length exceeds PATH_MAX limit" << std::endl;
        }
    } else {
        std::cout << "Failed to get current directory" << std::endl;
    }
    
    // Test other limits
    std::cout << "\nOther limits:" << std::endl;
    std::cout << "NAME_MAX = " << NAME_MAX << std::endl;
    
    std::cout << "\nSST-HGCC linux/limits.h test completed!" << std::endl;
    return 0;
}
