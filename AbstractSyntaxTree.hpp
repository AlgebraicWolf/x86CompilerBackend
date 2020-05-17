
//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP
#define X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP

#include "utilities.hpp"
#include "HashTable.hpp"
#include "Vector.hpp"

const int DEFAULT_BUCKET_SIZE = 32;

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

    NODE_TYPE getType(const char *serialized);

    const char *serializeType();

public:
    AbstractSyntaxNode();                                                       // Default constructor
    const char *deserialize(const char *serialized, HashTable<CRC32CFunctor, DEFAULT_BUCKET_SIZE> &ids,
                            vector<const char *> &string_ids);                        // Deserialization
    AbstractSyntaxNode(const AbstractSyntaxNode &other) = delete;               // Copy constructor
    AbstractSyntaxNode &operator=(const AbstractSyntaxNode &other) = delete;    // Copy assignment
    AbstractSyntaxNode(AbstractSyntaxNode &&other) noexcept;                    // Move constructor
    AbstractSyntaxNode &operator=(AbstractSyntaxNode &&other) noexcept;         // Move assignment
    ~AbstractSyntaxNode();                                                      // Destructor

    NODE_TYPE getNodeType();                                                    // Node type getter
    void dump(FILE *out);                                                       // Dump node
};


class AbstractSyntaxTree {
private:
    HashTable<CRC32CFunctor, DEFAULT_BUCKET_SIZE> IDs;                          // Text identifiers that are used in program
    vector<const char *> string_ids;                                                  // Vector for string literals
    AbstractSyntaxNode *root;                                                   // Tree root
    void reset();                                                               // Empty the tree
public:
    AbstractSyntaxTree();                                                       // Default constructor
    void load(const char *filename);                                            // Load tree from file
    AbstractSyntaxTree &operator=(AbstractSyntaxTree &&other) noexcept;        // Move assignment operator
    AbstractSyntaxTree(AbstractSyntaxTree &&other) noexcept;                    // Move constructor
    AbstractSyntaxTree(const AbstractSyntaxTree &other) = delete;               // Prohibit copy construction
    AbstractSyntaxTree &operator=(const AbstractSyntaxTree &other) = delete;    // Prohibit copy assignment
    ~AbstractSyntaxTree();                                                      // Destructor

    void dump(const char *filename);                                            // Dump tree into text file
};

const char *
AbstractSyntaxNode::deserialize(const char *serialized, HashTable<CRC32CFunctor, 32> &ids,
                                vector<const char *> &string_ids) {
    if (!serialized)
        throw_exception("Received null pointer to serialized string");

    serialized = skipSpaces(serialized);

    if (*serialized == '@') { // Nothing to look for here
        delete this;
        return serialized + 1;
    } else if (isdigit(*serialized)) { // Number node
        serialized = getNum(serialized, id);
        type = NUM;
    } else { // Some other type of node
        int idLength = 0;
        const char *identifier = getIdentifier(serialized, idLength);

        type = getType(identifier);
        id = 0;

        if (type == ID) {
            if (ids.Get(identifier) == -1) {
                string_ids.push_back(identifier);
                ids.Insert(identifier, string_ids.getSize());
                id = string_ids.getSize();
            } else {
                id = ids.Get(identifier);
                delete identifier;
            }
        }

        serialized += idLength;
    }

    serialized = skipSpaces(serialized);
    if (*serialized == '{') { // There are children nodes
        serialized++;
        left = new AbstractSyntaxNode();
        left->parent = this;

        serialized = left->deserialize(serialized, ids, string_ids);

        serialized = skipSpaces(serialized);

        if (*serialized != '}')
            throw_exception("Expected '}' symbol during tree parsing");

        serialized++;
        serialized = skipSpaces(serialized);

        if (*serialized != '{')
            throw_exception("Expected '{' symbol during tree parsing");

        serialized++;

        right = new AbstractSyntaxNode();
        right->parent = this;

        serialized = right->deserialize(serialized, ids, string_ids);
        serialized = skipSpaces(serialized);

        if (*serialized != '}')
            throw_exception("Expected '}' symbol during tree parsing");

        serialized++;
    }

    serialized = skipSpaces(serialized);

    if (*serialized != '}')
        throw_exception("Expected '}' symbol during tree parsing");

    return serialized;
}

AbstractSyntaxNode::AbstractSyntaxNode() : left(nullptr), right(nullptr), parent(nullptr), id(0), type(NONE) {}

AbstractSyntaxNode::AbstractSyntaxNode(AbstractSyntaxNode &&other) noexcept {
    swap(*this, other);
}


AbstractSyntaxNode &AbstractSyntaxNode::operator=(AbstractSyntaxNode &&other) noexcept {
    swap(*this, other);
}

NODE_TYPE AbstractSyntaxNode::getNodeType() {
    return type;
}


AbstractSyntaxNode::~AbstractSyntaxNode() {
    if (parent) {
        if (this == parent->left) {
            parent->left = nullptr;
        } else if (this == parent->right) {
            parent->right = nullptr;
        } else {
            throw_exception("During node destruction, orphan node was detected");
        }
    }

    delete left;
    delete right;
}

const char *AbstractSyntaxNode::serializeType() {
    switch (type) {
        case P:
            return "PROGRAM ROOT";

        case NONE:
            return "NONE";

        case D:
            return "DEFINITION";

        case OP:
            return "OPERATION";

        case VARLIST:
            return "VARLIST";

        case ID:
            return "ID";

        case C:
            return "BRANCHING";

        case B:
            return "BLOCK";

        case DEF:
            return "FUNCTION";

        case IF:
            return "IF";

        case WHILE:
            return "WHILE";

        case ASSIGN:
            return "=";

        case VAR:
            return "VAR";

        case RETURN:
            return "RETURN";

        case CALL:
            return "CALL";

        case BELOW:
            return "\\<";


        case ABOVE:
            return "\\>";


        case EQUAL:
            return "==";


        case MUL:
            return "*";


        case DIV:
            return "/";


        case ADD:
            return "+";


        case SUB:
            return "-";


        case SQRT:
            return "sqrt";

        case NUM:
            return "INTEGER";

        case INPUT:
            return "INPUT";

        case OUTPUT:
            return "OUTPUT";

    }
}

NODE_TYPE AbstractSyntaxNode::getType(const char *serialized) {
    if (!serialized)
        throw_exception("Invalid string pointer is provided to getType function");

    if (strcmp(serialized, "DECLARATION") == 0) {
        return D;
    } else if (strcmp(serialized, "IF") == 0) {
        return IF;
    } else if (strcmp(serialized, "WHILE") == 0) {
        return WHILE;
    } else if (strcmp(serialized, "FUNCTION") == 0) {
        return DEF;
    } else if (strcmp(serialized, "VARLIST") == 0) {
        return VARLIST;
    } else if (strcmp(serialized, "OP") == 0) {
        return OP;
    } else if (strcmp(serialized, "ASSIGN") == 0) {
        return ASSIGN;
    } else if (strcmp(serialized, "RETURN") == 0) {
        return RETURN;
    } else if (strcmp(serialized, "INITIALIZE") == 0) {
        return VAR;
    } else if (strcmp(serialized, "CALL") == 0) {
        return CALL;
    } else if (strcmp(serialized, "INPUT") == 0) {
        return INPUT;
    } else if (strcmp(serialized, "OUTPUT") == 0) {
        return OUTPUT;
    } else if (strcmp(serialized, "PROGRAM_ROOT") == 0) {
        return P;
    } else if (strcmp(serialized, "C") == 0) {
        return C;
    } else if (strcmp(serialized, "BLOCK") == 0) {
        return B;
    } else if (strcmp(serialized, "ADD") == 0) {
        return ADD;
    } else if (strcmp(serialized, "SUB") == 0) {
        return SUB;
    } else if (strcmp(serialized, "MUL") == 0) {
        return MUL;
    } else if (strcmp(serialized, "DIV") == 0) {
        return DIV;
    } else if (strcmp(serialized, "BELOW") == 0) {
        return BELOW;
    } else if (strcmp(serialized, "ABOVE") == 0) {
        return ABOVE;
    } else if (strcmp(serialized, "EQUAL") == 0) {
        return EQUAL;
    } else if (strcmp(serialized, "SQR") == 0) {
        return SQRT;
    } else {
        return ID;
    }
}

AbstractSyntaxTree::AbstractSyntaxTree() : IDs(), string_ids(), root(nullptr) {}

AbstractSyntaxTree::AbstractSyntaxTree(AbstractSyntaxTree &&other) noexcept {
    swap(*this, other);
}

AbstractSyntaxTree &AbstractSyntaxTree::operator=(AbstractSyntaxTree &&other) noexcept {
    swap(*this, other);
    return *this;
}

void AbstractSyntaxTree::reset() {
    delete root;
}

AbstractSyntaxTree::~AbstractSyntaxTree() {
    reset();
}

void AbstractSyntaxTree::load(const char *filename) {
    if (!filename)
        throw_exception("Invalid pointer to filename is provided to AbstractSyntaxTree::load function.");

    FILE *inputFile = fopen(filename, "r");

    if (!inputFile)
        throw_exception("Unable to open file in AbstractSyntaxTree::load function");

    size_t filesize = getFilesize(inputFile);

    char *serializedBegin = new char[filesize]();
    fread(serializedBegin, sizeof(char), filesize, inputFile);
    fclose(inputFile);

    char *serialized = skipSpaces(serializedBegin);

    if (*serialized != '{')
        throw_exception("Expected \'{\' at the beginning of the AST file");

    serialized++;

    AbstractSyntaxNode *newRoot = new AbstractSyntaxNode();
    const char *serializedEnd = newRoot->deserialize(serialized, IDs, string_ids);

    if (*serializedEnd != '}')
        throw_exception("Expected \'}\' at the end of the AST file");

    if (root)
        reset();                // Release old root

    root = newRoot;             // Append new AST
    delete[] serializedBegin;   // Free memory. Loading done!
}

void AbstractSyntaxTree::dump(const char *filename) {
    FILE *dumpFile = fopen(filename, "w");
    fprintf(dumpFile, "digraph {\n");

    if (root)
        root->dump(dumpFile);

    fprintf(dumpFile, "}\n");
    fclose(dumpFile);
}

void AbstractSyntaxNode::dump(FILE *out) {
    fprintf(out, "node%p[shape=record, label=\"TYPE: %s | id: %d\"];\n", this, serializeType(), id);

    if (parent)
        fprintf(out, "node%p -> node%p;\n", this, parent);

    if (left) {
        fprintf(out, "node%p -> node%p;\n", this, left);
        left->dump(out);
    }

    if (right) {
        fprintf(out, "node%p -> node%p;\n", this, right);
        right->dump(out);
    }
}

#endif //X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP
