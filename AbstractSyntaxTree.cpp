//
// Created by alexey on 17.05.2020.
//

#include <cctype>
#include <cstdio>

#include "AbstractSyntaxTree.hpp"
#include "utilities.hpp"

NODE_TYPE AbstractSyntaxNode::getType() {
    return type;
}

const char *AbstractSyntaxNode::deserialize(const char *serialized) {
    if (!serialized)
        throw_exception("Received null pointer to serialized string");

    serialized = skipSpaces(serialized);

    if (*serialized == '@') { // Nothing to look for here
        delete this;
        return serialized++;
    } else if (isdigit(*serialized)) { // Number node
        int number = 0;
        serialized = getNum(serialized, number);
        id = number;
        type = NUM;
    } else { // Some other type of node
        const char *identifier = nullptr;
    }

    serialized = skipSpaces(serialized);
    if (*serialized == '{') { // There are children nodes
        serialized++;
        left = new AbstractSyntaxNode();
        serialized = left->deserialize(serialized);

        if (*serialized != '}')
            throw_exception("Expected '}' symbol during tree parsing");

        serialized = skipSpaces(serialized);

        if (*serialized != '{')
            throw_exception("Expected '{' symbol during tree parsing");

        serialized++;

        right = new AbstractSyntaxNode();
        serialized = right->deserialize(serialized);
        serialized = skipSpaces(serialized);

        if (*serialized != '}')
            throw_exception("Expected '}' symbol during tree parsing");

        serialized++;
    }

    // Do some routine if required

    if (*serialized != '}')
        throw_exception("Expected '}' symbol during tree parsing");

    return serialized;
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

    AbstractSyntaxNode *newRoot = new AbstractSyntaxNode();
    const char *serializedEnd = newRoot->deserialize(serialized);

    if (*serializedEnd != '}')
        throw_exception("Expected \'}\' at the end of the AST file");

    if (root)
        reset();                // Release old root

    root = newRoot;             // Append new AST
    delete[] serializedBegin;   // Free memory. Loading done!
}
