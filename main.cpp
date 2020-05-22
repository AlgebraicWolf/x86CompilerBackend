#include <iostream>
#include <exception>

#include "utilities.hpp"
#include "AbstractSyntaxTree.hpp"
#include "AssemblyTools.hpp"

int main() {
    AbstractSyntaxTree prog;
    prog.load("input.ast");

    AssemblyProgram compiled = prog.compile();
    compiled.toNASM("fact.asm");

    return 0;
}