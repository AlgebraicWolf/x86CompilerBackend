#include <iostream>

class AssemblyListing {
private:

public:
    AssemblyListing();                                                          // Default constructor
    AssemblyListing(AssemblyListing&& other);                                   // Move constructor
    AssemblyListing& operator=(AssemblyListing&& other);                        // Move assignment operator
    AssemblyListing(const AssemblyListing& other) = delete;                     // Prohibit copy construction
    AssemblyListing& operator=(const AssemblyListing& other) = delete;          // Prohibit copy assignment
    ~AssemblyListing();                                                         // Destructor
};

class AbstractSyntaxNode {
private:

public:
    AbstractSyntaxNode();                                                       // Default constructor
    AbstractSyntaxNode(const AbstractSyntaxNode& other);                        // Copy constructor
    AbstractSyntaxNode& operator=(const AbstractSyntaxNode& other);             // Copy assignment
    AbstractSyntaxNode(AbstractSyntaxNode&& other);                             // Move constructor
    AbstractSyntaxNode& operator=(AbstractSyntaxNode&& other);                  // Move assignment 
};

class AbstractSyntaxTree {
private:

public:
    AbstractSyntaxTree();                                                       // Default constructor
    AbstractSyntaxTree& operator=(AbstractSyntaxTree&& other);                  // Move assignment operator
    AbstractSyntaxTree(AbstractSyntaxTree&& other);                             // Move constructor
    AbstractSyntaxTree(const AbstractSyntaxTree& other) = delete;               // Prohibit copy construction
    AbstractSyntaxTree& operator=(const AbstractSyntaxTree& other) = delete;    // Prohibit copy assignment
    ~AbstractSyntaxTree();                                                      // Destructor
};

int main() {

    return 0;
}