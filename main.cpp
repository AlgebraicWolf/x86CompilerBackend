#include <iostream>
#include <exception>
#include <sys/mman.h>

#include "utilities.hpp"
#include "AbstractSyntaxTree.hpp"
#include "AssemblyTools.hpp"

#include <limits.h>
#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

int main() {
    AbstractSyntaxTree prog;
    prog.load("input.ast");

    AssemblyProgram compiled = prog.compile();
    Bytecode bcode = compiled.toBytecode();

    unsigned char *data = bcode.data();

    size_t new_size = ((bcode.getSize() - 1) / PAGESIZE + 1) * PAGESIZE;

    unsigned char *newBuf = reinterpret_cast<unsigned char *>(aligned_alloc(PAGESIZE, new_size));

    memcpy(newBuf, data, bcode.getSize());

    mprotect(newBuf, new_size, PROT_EXEC);
    int res = errno;

    asm volatile(".intel_syntax noprefix;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        "nop;"
        ".att_syntax prefix;");

    (*reinterpret_cast<void (*)()>(newBuf))();



    FILE *out = fopen("test.bin", "wb");
    fwrite(bcode.data(), sizeof(unsigned char), bcode.getSize(), out);
    fclose(out);
    return 0;
}