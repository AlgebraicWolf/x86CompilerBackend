//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP
#define X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP

#include <cstdio>
#include "Vector.hpp"

#define CASE_REGNAME(X) case X: return #X

enum REGISTER {
    EAX,
    EBX,
    ECX,
    EDX,
    EBP,
    ESP,
    ESI,
    EDI
};

enum OP_TYPE {
    NOP,
    INT,
    MOV_REG_REG,
    MOV_REG_IMM,
    MOV_REG_RAM,
    MOV_RAM_REG,
    MOV_RM_REG,
    MOV_REG_RM,
    JMP,
    JG,
    JGE,
    JL,
    JLE,
    JE,
    JNE,
    RET,
    CALL,
    DIV,
    MUL,
    INC,
    DEC
};

const char *regToText(REGISTER reg) {
    switch (reg) {
        CASE_REGNAME(EAX);
        CASE_REGNAME(EBX);
        CASE_REGNAME(ECX);
        CASE_REGNAME(EDX);
        CASE_REGNAME(EBP);
        CASE_REGNAME(ESP);
        CASE_REGNAME(ESI);
        CASE_REGNAME(EDI);
    }
    return "INVALID_REG";
}

class Bytecode {

};

class Operation {
private:

public:
    virtual void toNASM(FILE *output) = 0;                          // Translate instruction to NASM
    virtual void toBytecode(Bytecode buf) = 0;                        // Translate instruction to bytecode
};

class nop : public Operation {
public:
    virtual void toNASM(FILE *output) {
        fprintf(output, "    nop\n");
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class interrupt : public Operation {
private:
    char int_num;
public:
    interrupt(char int_num) : int_num(int_num) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    int %d\n", int_num);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class mov_reg_imm : public Operation {
private:
    REGISTER to;
    int imm;
public:
    mov_reg_imm(REGISTER to, int imm) : to(to), imm(imm) { }

    virtual void toNASM(FILE *output) {
        fprintf(output, "    mov %s, %d\n", regToText(to), imm);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class mov_reg_reg : public Operation {
private:
    REGISTER to;
    REGISTER from;
public:
    mov_reg_reg(REGISTER to, REGISTER from) : to(to), from(from) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    mov %s, %s\n", regToText(to), regToText(from));
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class AssemblyListing {
private:
    vector<Operation*> ops;                                         // Vector of operations i. e. part of the program
public:
    AssemblyListing();                                              // Default constructor
    AssemblyListing(AssemblyListing &&other) noexcept;              // Move constructor
    AssemblyListing &operator=(AssemblyListing &&other) noexcept;   // Move assignment
    AssemblyListing(const AssemblyListing &other) = delete;         // Prohibit copy constructor
    AssemblyListing &operator=(const AssemblyListing &other) = delete;// Prohibit copy assignment

    void syscall();                                                 // syscall

    void nop();                                                     // Good ol' nop

    void mov(REGISTER to, int imm);                                 // mov to, immediate
    void mov(REGISTER to, REGISTER from);                           // mov to, from
    void mov(REGISTER to, REGISTER ptr, char offset);               // mov to, size(to) ptr [from + offset]
    void mov(REGISTER to, REGISTER ptr, int offset);                // mov to, size(to) ptr [from + offset]
    void mov(REGISTER to, size_t addr);                             // mov to, size(to) ptr [addr]
    void mov(size_t addr, REGISTER from);                           // mov size(from) ptr [addr], from
    void mov(REGISTER ptr, char offset, REGISTER from);             // mov size(from) [ptr + offset], from
    void mov(REGISTER ptr, int offset, REGISTER from);              // mov size(from) [ptr + offset], from

    int addLocalLabel();                                            // Inserts local label at current position

    void jmp(int labelId);                                          // jmp labelId
    void jg(int labelId);                                           // jmp labelId
    void jge(int labelId);                                          // jmp labelId
    void jl(int labelId);                                           // jmp labelId
    void jle(int labelId);                                          // jmp labelId
    void je(int labelId);                                           // jmp labelId
    void jne(int labelId);                                          // jmp labelId

    void ret();                                                     // ret, yeah
    void call(int functionId);                                      // call functionId

    void div();

    void mul();

    void inc(REGISTER what);                                        // inc register
    void dec(REGISTER what);                                        // dec register
};

class AssemblyProgram {
private:
    vector<AssemblyListing> listings;                               // Vector of assembly listings
    unsigned int main;                                              // Position of main listing
public:
    AssemblyProgram();                                              // Default constructor
    AssemblyProgram(AssemblyProgram &&other);                       // Move counstructor
    AssemblyProgram(const AssemblyProgram &other) = delete;         // Prohibit copy constructor
    AssemblyProgram &operator=(AssemblyProgram &&other);            // Move assignment
    AssemblyProgram &operator=(const AssemblyProgram &other) = delete; // Prohibit copy assignment

    int pushListing(AssemblyListing &&lst);                         // Push listing to the program, return listing id
    size_t appendBytesToData(const char *bytes, int len);           // Appends bytes to data, returns offset from the beginning of .data section

    void setMainListing(int pos);                                   // Set listing for main function

    void toNASM(char *filename);                                    // Translate program to Netwide Assembly
    void toBytecode(char *filename);                                // Translate program to plain bytecode
    void toELF(char *filename);                                     // Generate ELF file
};

#endif //X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP
