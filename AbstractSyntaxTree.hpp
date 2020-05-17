#include "utilities.hpp"
//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP
#define X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP

enum NODE_TYPE {
    NONE,
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

class AbstractSyntaxNode {
private:
    NODE_TYPE type;                                                             // Get current node type
    AbstractSyntaxNode *left;                                                   // Pointer to the left child of the node
    AbstractSyntaxNode *right;                                                  // Pointer to the right child of the node
    AbstractSyntaxNode *parent;                                                 // Pointer to the parent of the node
    int id;                                                                     // Node identifier
public:
    AbstractSyntaxNode();                                                       // Default constructor
    const char *deserialize(const char *serialized);                            // Deserialization
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


#endif //X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP
