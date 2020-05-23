
//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP
#define X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP

#include "utilities.hpp"
#include "HashTable.hpp"
#include "Vector.hpp"
#include "AssemblyTools.hpp"

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
    void parseLocalVariables(int &alloc,
                             int *offsets);                                         // Count local variables and determine their offsets
    void
    parseArguments(int *offsets, int depth);                                        // Determine offsets for function arguments

    void compileOperation(AssemblyListing &func, int *numbers, int *offsets);       // Compile Operation node
    void compileExpression(AssemblyListing &func, int *numbers, int *offsets);      // Compile expression subtree
    int pushVarlist(AssemblyListing &func, int *numbers, int *offsets);             // Push function arguments into stack

public:
    AbstractSyntaxNode();                                                       // Default constructor
    const char *deserialize(const char *serialized, HashTable<CRC32CFunctor, DEFAULT_BUCKET_SIZE> &ids,
                            vector<const char *> &string_ids);                  // Deserialization
    AbstractSyntaxNode(const AbstractSyntaxNode &other) = delete;               // Copy constructor
    AbstractSyntaxNode &operator=(const AbstractSyntaxNode &other) = delete;    // Copy assignment
    AbstractSyntaxNode(AbstractSyntaxNode &&other) noexcept;                    // Move constructor
    AbstractSyntaxNode &operator=(AbstractSyntaxNode &&other) noexcept;         // Move assignment
    ~AbstractSyntaxNode();                                                      // Destructor

    NODE_TYPE getNodeType();                                                    // Node type getter
    AbstractSyntaxNode *getRight();

    AbstractSyntaxNode *getLeft();

    AssemblyListing compileFunction(int *numbers,
                                    int idsSize);                              // Function compiler (Should start only in function node)

    int getID();

    void dump(FILE *out);                                                       // Dump node
};


class AbstractSyntaxTree {
private:
    HashTable<CRC32CFunctor, DEFAULT_BUCKET_SIZE> IDs;                          // Text identifiers that are used in program
    vector<const char *> string_ids;                                                  // Vector for string literals
    AbstractSyntaxNode *root;                                                   // Tree root
    void reset();                                                               // Empty the tree
    AssemblyListing getOutputFunction();                                        // Generate output function
    AssemblyListing getInputFunction();                                         // Generate input function

public:
    int *
    functionIDtoNumber();                                                       // Perform simple traversal and determine listing ID for each function
    AssemblyProgram compile();                                                  // Translate program into assembly

    AbstractSyntaxTree();                                                       // Default constructor
    void load(const char *filename);                                            // Load tree from file
    AbstractSyntaxTree &operator=(AbstractSyntaxTree &&other) noexcept;         // Move assignment operator
    AbstractSyntaxTree(AbstractSyntaxTree &&other) noexcept;                    // Move constructor
    AbstractSyntaxTree(const AbstractSyntaxTree &other) = delete;               // Prohibit copy construction
    AbstractSyntaxTree &operator=(const AbstractSyntaxTree &other) = delete;    // Prohibit copy assignment
    ~AbstractSyntaxTree();                                                      // Destructor

    void dump(const char *filename);                                            // Dump tree into text file
};

void AbstractSyntaxNode::parseLocalVariables(int &alloc, int *offsets) {
    if (type == VAR) {
        alloc++;
        offsets[right->id] = alloc * (-4);
    }

    if (right)
        right->parseLocalVariables(alloc, offsets);

    if (left)
        left->parseLocalVariables(alloc, offsets);
}

void AbstractSyntaxNode::parseArguments(int *offsets, int depth) {
    if (type != VARLIST)
        throw_exception("Parsing arguments in non-varlist node");

    if (right) {
        depth++;
        offsets[right->id] = 4 * depth;
        if (left)
            left->parseArguments(offsets, depth);
    }
}

int AbstractSyntaxNode::pushVarlist(AssemblyListing &func, int *numbers, int *offsets) {
    if(type != VARLIST)
        throw_exception("Trying to push arguments in non-varlist node");

    if(right) {
        int pushed = 0;
        if(left) {
            pushed = left->pushVarlist(func, numbers, offsets);
        }

        right->compileExpression(func, numbers, offsets);
        func.push(EAX);
        return pushed + 1;
    }

    return 0;
}

void AbstractSyntaxNode::compileExpression(AssemblyListing &func, int *numbers, int *offsets) {
    if(type == CALL) {
        func.comment("Pushing varlist START");
        int vars = right->pushVarlist(func, numbers, offsets);
        func.comment("Pushing varlist END");
        func.call(numbers[left->id]);
        func.add(ESP, vars * 4);
        return;
    }

    if(right) {
        right->compileExpression(func, numbers, offsets); // If right subtree is present, execute it. Result is stored in EAX

    }

    if(left) {
        func.push(EAX); // Save result
        left->compileExpression(func, numbers, offsets); // If left subtree is present, execute it. Result stored in EBX
        func.pop(EBX);
    }




    switch(type) {
        case ADD:
            func.add(EAX, EBX);
            break;

        case SUB:
            func.sub(EAX, EBX);
            break;

        case SQRT:
            throw_exception("SQRT is not yet implemented");
            break;

        case MUL:
            func.imul(EBX);
            break;

        case DIV:
            func.idiv(EBX);
            break;

        case ID:
            func.mov(EAX, EBP, offsets[id]);
            break;

        case NUM:
            func.mov(EAX, id);
            break;

        default:
            throw_exception("Invalid node type during expression compilation");
            break;
    }
}



void AbstractSyntaxNode::compileOperation(AssemblyListing &func, int *numbers, int *offsets) {
    if(type != OP)
        throw_exception("Trying to compile non-operation node as operation one");


    switch (right->type) {
        case INPUT:
            func.call(0);
            func.mov(EBP, offsets[right->right->id], EAX);
            break;

        case OUTPUT:
            right->right->compileExpression(func, numbers, offsets);
            func.call(1);
            break;

        case IF:
            if(!(right->left->type == EQUAL || right->left->type == ABOVE || right->left->type == EQUAL))
                throw_exception("Invalid comparison node while compiling IF statement");
            {
                int labelCount = func.getLabelCount();
                right->left->left->compileExpression(func, numbers, offsets);
                func.push(EAX);
                right->left->right->compileExpression(func, numbers, offsets);
                func.pop(EBX);
                func.cmp(EAX, EBX);
                switch(right->left->type) {
                    case EQUAL:
                        func.jne(labelCount);
                        break;

                    case ABOVE:
                        func.jge(labelCount);
                        break;

                    case BELOW:
                        func.jle(labelCount);
                        break;
                }
                right->right->right->right->compileOperation(func, numbers, offsets);
                if(right->right->left) { // ELSE branch is present
                    func.jmp(labelCount + 1); // DO NOT execute ELSE branch if statement is true
                    func.addLocalLabel(); // ELSE branch label
                    right->right->left->right->compileOperation(func, numbers, offsets); // compile ELSE branch
                    func.addLocalLabel(); // End of if label
                } else {
                    func.addLocalLabel(); // End of IF statement
                }
            }
            break;

        case ASSIGN:
            right->right->compileExpression(func, numbers, offsets);
            func.mov(EBP, offsets[right->left->id], EAX);
            break;

        case VAR:
            break;

        case WHILE:
            if(!(right->left->type == EQUAL || right->left->type == ABOVE || right->left->type == EQUAL))
                throw_exception("Invalid comparison node while compiling WHILE statement");
            {

                int labelCount = func.addLocalLabel(); // Label before check -- Start of the loop
                right->left->left->compileExpression(func, numbers, offsets); // First operand
                func.push(EAX);
                right->left->right->compileExpression(func, numbers, offsets); // Second operand
                func.pop(EBX);

                switch (right->left->type) {
                    case EQUAL:
                        func.jne(labelCount + 1);
                        break;

                    case ABOVE:
                        func.jge(labelCount + 1);
                        break;

                    case BELOW:
                        func.jle(labelCount + 1);
                        break;
                }

                right->right->right->compileOperation(func, numbers, offsets); // Compile loop body
                func.jmp(labelCount); // Go back to the check
                func.addLocalLabel(); // End of loop
            }
            break;

        case RETURN:
            right->right->compileExpression(func, numbers, offsets);
            func.mov(ESP, EBP); // Restore old stack pointer i. e. deallocate everything
            func.pop(EBP); // Restore old stack frame
            func.ret();
            break;

    }

    if(left)
        left->compileOperation(func, numbers, offsets);
}

AssemblyListing AbstractSyntaxNode::compileFunction(int *numbers, int idsSize) {
    if (type != DEF)
        throw_exception("Function compilation started from non-function node");

    AssemblyListing function;  // Create listing for current function
    function.push(EBP); // Preserve caller stack frame
    function.mov(EBP, ESP); // Create stack frame

    int *offsets = new int[idsSize](); // Array for offsets
    int alloc = 0; // Number of variables to allocate
    parseLocalVariables(alloc, offsets); // Parse all the local variables
    left->parseArguments(offsets, 1); // Parse arguments

    function.sub(ESP, alloc * 4); // Allocate space for local variables

    right->right->right->compileOperation(function, numbers, offsets);

    function.mov(ESP, EBP); // Restore old stack pointer i. e. deallocate everything
    function.pop(EBP); // Restore old stack frame
    function.ret();
    return function;
}

AssemblyListing AbstractSyntaxTree::getOutputFunction() {
    AssemblyListing output; // Output function listing

    output.mov(ESI, EAX);
    output.mov(EDX, 0); // Zero in edx
    output.mov(ECX, ESP); // Old pointer

    output.dec(ESP);
    output.mov(ESP, 0, '\n'); // Line break

    output.cmp(EAX, EDX); // Check whether integer is negative

    output.jge(0); // If it is, skip neg
    output.neg(EAX);

    output.addLocalLabel(); // Actual rendering
    output.mov(EBX, 10);
    output.idiv(EBX); // Divide EDX:EAX by EBX
    output.add(EDX, 48); // Turn it into character
    output.dec(ESP);
    output.mov(ESP, 0, DL);
    output.mov(EDX, 0); // Free EDX
    output.cmp(EAX, EDX); // Check whether it is over
    output.jne(0); // Continue if it is not over


    output.cmp(ESI, EDX); // Check whether it is negative

    output.jge(1); // If it is not, skip part with neg sign
    output.dec(ESP);
    output.mov(ESP, 0, '-');

    output.addLocalLabel(); // Writing process
    output.mov(EDX, ECX);
    output.sub(EDX, ESP); // Calculating length

    output.mov(EAX, 4); // sys_write
    output.mov(EBX, 1); // FD - STDOUT
    output.mov(ECX, ESP); // Buffer position
    output.interrupt(0x80); // Call interrupt
    output.add(ESP, EDX); // Clear stack
    output.ret(); // Return

    return output;
}

AssemblyListing AbstractSyntaxTree::getInputFunction() {
    AssemblyListing input;
    input.push(EBP); // I want one more free register
    input.mov(EBP, 0);

    input.mov(ESI, 0); // Start from zero
    input.mov(EDI, 10); // Base

    input.sub(ESP, 4); // Allocate four bytes bcause I am lazy

    input.mov(EBX, 0); // STDIN descriptor
    input.mov(ECX, ESP); // Buffer address

    input.addLocalLabel(); // Reading loop starts here
    input.mov(EAX, 3); // sys_read()
    input.mov(EDX, 1); // Amount of characters to read
    input.interrupt(0x80); // Call function

    input.mov(EAX, ESP, 0); // Load character
    input.cmp(EAX, '\n'); // Compare with '\n'
    input.je(2); // If it is equal, leave the loop
    input.cmp(EAX, '-'); // If it is not negative
    input.jne(1); // Skip sign switching flag
    input.mov(EBP, 1); // Set flag for latter sign switching
    input.jmp(0); // Proceed reading
    input.addLocalLabel(); // End of sign switching
    input.mov(EAX, ESI); // Get result to eax
    input.imul(EDI); // Multiply by 10
    input.mov(ESI, ESP, 0);
    input.sub(ESI, 48);
    input.add(ESI, EAX);
    input.jmp(0);

    input.addLocalLabel(); // End of input
    input.cmp(EBP, 0); // Check whether it is required to change sign
    input.je(3); // Skip switching if required
    input.neg(ESI);
    input.addLocalLabel(); // End of switching sign
    input.add(ESP, 4); // Free memory
    input.pop(EBP); // Restore EBP
    input.mov(EAX, ESI); // Move result into EAX
    input.ret(); // Return
    return input;
}

AssemblyProgram AbstractSyntaxTree::compile() {
    int *numbers = functionIDtoNumber(); // Translate function IDs into listing numbers for further use

    AssemblyProgram prog; // Create assembly program

    AbstractSyntaxNode *current = root->getRight(); // Start from the first definition

    prog.pushListing(getInputFunction());
    prog.pushListing(getOutputFunction());

    while (current) { // Traverse through all the functions and compile them as listings
        prog.pushListing(current->getRight()->compileFunction(numbers,
                                                              string_ids.getSize())); // Translate function into asm listing
        current = current->getLeft(); // Proceed to the next function
    }
    prog.setMainListing(numbers[IDs.Get("main")]);
    delete[] numbers;
    return prog;
}


AbstractSyntaxNode *AbstractSyntaxNode::getLeft() {
    return left;
}

AbstractSyntaxNode *AbstractSyntaxNode::getRight() {
    return right;
}

int AbstractSyntaxNode::getID() {
    return id;
}

int *AbstractSyntaxTree::functionIDtoNumber() {
    int *numbers = new int[string_ids.getSize()]();
    int cur = 2; // Leave space for itoa and atoi
    AbstractSyntaxNode *current = root->getRight();
    while (current && current->getNodeType() == D) {
        numbers[current->getRight()->getRight()->getID()] = cur++;
        current = current->getLeft();
    }

    return numbers;
}

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
                ids.Insert(identifier, string_ids.getSize() - 1);
                id = string_ids.getSize() - 1;
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
    fprintf(out, "node%p[shape=record, label=\"{TYPE: %s | id: %d | {<l> left | <r> right}}\"];\n", this,
            serializeType(), id);

    if (parent)
        fprintf(out, "node%p -> node%p;\n", this, parent);

    if (left) {
        fprintf(out, "node%p:l -> node%p;\n", this, left);
        left->dump(out);
    }

    if (right) {
        fprintf(out, "node%p:r -> node%p;\n", this, right);
        right->dump(out);
    }
}

#endif //X86COMPILERBACKEND_ABSTRACTSYNTAXTREE_HPP
