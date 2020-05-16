#include <iostream>
#include <exception>

#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define throw_exception(str) throw runtime_error("In " __FILE__ ":" STRINGIFY(__LINE__) ", the following exception occured:\n" str)

size_t getFilesize(FILE *f) {
    if (!f)
        throw std::runtime_error("Invalid file pointer is provided");

    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    return sz;
}

char *skipSpaces(char *str) {
    while (isblank(*str) && *str != '\0')
        str++;

    return str;
}

enum NODE_TYPE {
    D,
    DEF,
    VARLIST,
    ID,
    P,
    OP,
    C,
    B,
    IF,
    WHILE,
    E,
    ASSIGN,
    VAR,
    RETURN,
    CALL,
    ARITHM_OP,
    NUM,
    INPUT,
    OUTPUT,
    ADD,
    MUL,
    DIV,
    SUB,
    SQRT,
    BELOW,
    ABOVE,
    EQUAL
};

class runtime_error : public std::exception {
private:
    const char *msg;

public:
    runtime_error(const char *msg) : std::exception(), msg(msg) {}
    [[nodiscard]] const char * what() const throw() override {
        return msg;
    }
};

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

class AbstractSyntaxNode {
private:
    NODE_TYPE type;                                                             // Get current node type
    AbstractSyntaxNode *left;                                                   // Pointer to the left child of the node
    AbstractSyntaxNode *right;                                                  // Pointer to the right child of the node
    AbstractSyntaxNode *parent;                                                 // Pointer to the parent of the node
    int id;                                                                     // Node identifier
public:
    AbstractSyntaxNode();                                                       // Default constructor
    AbstractSyntaxNode(const AbstractSyntaxNode &other);                        // Copy constructor
    AbstractSyntaxNode &operator=(const AbstractSyntaxNode &other);             // Copy assignment
    AbstractSyntaxNode(AbstractSyntaxNode &&other);                             // Move constructor
    AbstractSyntaxNode &operator=(AbstractSyntaxNode &&other);                  // Move assignment
    ~AbstractSyntaxNode();                                                      // Destructor

    NODE_TYPE getType();                                                        // Node type getter
};

class AbstractSyntaxTree {
private:
    AbstractSyntaxNode *root;                                                   // Tree root
    void reset();                                                               // Empty the tree
public:
    AbstractSyntaxTree();                                                       // Default constructor
    void load(const char *filename);                                            // Load tree from file
    AbstractSyntaxTree &operator=(AbstractSyntaxTree &&other);                  // Move assignment operator
    AbstractSyntaxTree(AbstractSyntaxTree &&other);                             // Move constructor
    AbstractSyntaxTree(const AbstractSyntaxTree &other) = delete;               // Prohibit copy construction
    AbstractSyntaxTree &operator=(const AbstractSyntaxTree &other) = delete;    // Prohibit copy assignment
    ~AbstractSyntaxTree();                                                      // Destructor
};

void AbstractSyntaxTree::load(const char *filename) {
    if (!filename)
        throw std::runtime_error("Invalid pointer to filename is provided to AbstractSyntaxTree::load function.");

    FILE *inputFile = fopen(filename, "r");

    if (!inputFile)
        throw std::runtime_error("Unable to open file in AbstractSyntaxTree::load function");

    size_t filesize = getFilesize(inputFile);

    char *serialized = new char[filesize]();
    fread(serialized, sizeof(char), filesize, inputFile);
    fclose(inputFile);

    serialized = skipSpaces(serialized);
}

int main() {
    throw_exception("Testing...");
    return 0;
}