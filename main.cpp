#include <iostream>
#include <exception>

#include "utilities.hpp"
#include "AbstractSyntaxTree.hpp"

class AssemblyListing {
private:

public:
    AssemblyListing();                                                          // Default constructor
    AssemblyListing(AssemblyListing &&other);                                   // Move constructor
    AssemblyListing &operator=(AssemblyListing &&other);                        // Move assignment operator
    AssemblyListing(const AssemblyListing &other) = delete;                     // Prohibit copy construction
    AssemblyListing &operator=(const AssemblyListing &other) = delete;          // Prohibit copy assignment
    ~AssemblyListing();                                                         // Destructor
};

int main() {
    throw_exception("Testing...");
    return 0;
}