#include <iostream>
#include <exception>

#include "utilities.hpp"
#include "AbstractSyntaxTree.hpp"
#include "AssemblyTools.hpp"

int main() {
    AbstractSyntaxTree prog;
    prog.load("input.ast");
    prog.dump("out.dot");

    AssemblyProgram compiled = prog.compile();
    compiled.toNASM("test.asm");

    return 0;
}