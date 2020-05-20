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
//    NOP,
//    INT,
//    MOV_REG_REG,
//    MOV_REG_IMM,
            MOV_REG_RAM,
    MOV_RAM_REG,
//    MOV_RM_REG_OFF8,
//    MOV_RM_REG_OFF32,
//    MOV_REG_RM_OFF8,
//    MOV_REG_RM_OFF32,
            JMP,
//    JG,
//    JGE,
//    JL,
//    JLE,
//    JE,
//    JNE,
//    RET,
//    CALL,
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
    virtual void toBytecode(Bytecode buf) = 0;                      // Translate instruction to bytecode
    virtual int getSize() = 0;                                      // Length of command in bytes
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
    mov_reg_imm(REGISTER to, int imm) : to(to), imm(imm) {}

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

class mov_rm_reg_off8 : public Operation {
private:
    REGISTER rmreg;
    char offset;
    REGISTER from;
public:
    mov_rm_reg_off8(REGISTER rmreg, char offset, REGISTER from) : rmreg(rmreg), offset(offset), from(from) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    mov [%s%+d], %s\n", regToText(rmreg), offset, regToText(from));
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class mov_rm_reg_off32 : public Operation {
private:
    REGISTER rmreg;
    int offset;
    REGISTER from;
public:
    mov_rm_reg_off32(REGISTER rmreg, int offset, REGISTER from) : rmreg(rmreg), offset(offset), from(from) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    mov [%s%+d], %s\n", regToText(rmreg), offset, regToText(from));
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class mov_reg_rm_off8 : public Operation {
private:
    REGISTER rmreg;
    char offset;
    REGISTER to;
public:
    mov_reg_rm_off8(REGISTER to, REGISTER rmreg, char offset) : rmreg(rmreg), offset(offset), to(to) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    mov %s, [%s%+d]\n", regToText(to), regToText(rmreg), offset);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class mov_reg_rm_off32 : public Operation {
private:
    REGISTER rmreg;
    int offset;
    REGISTER to;
public:
    mov_reg_rm_off32(REGISTER to, REGISTER rmreg, int offset) : rmreg(rmreg), offset(offset), to(to) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    mov %s, [%s%+d]\n", regToText(to), regToText(rmreg), offset);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class call : public Operation {
private:
    int listingId;
    int offset;
public:
    explicit call(int listingId) : listingId(listingId) {}

    int getListingId() {
        return listingId;
    }

    int setOffset(int offset) {
        this->offset = offset;
    }

    virtual void toNASM(FILE *output) {
        fprintf(output, "    call listing%d\n", listingId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class ret : public Operation {
private:

public:
    virtual void toNASM(FILE *output) {
        fprintf(output, "    ret\n");
    }
};

class ret_pop : public Operation {
private:
    unsigned short to_pop;
public:
    ret_pop(unsigned short to_pop) : to_pop(to_pop) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    ret %d\n", to_pop);
    }
};

class Jump : public Operation {
protected:
    int offset;
    int labelId;
public:
    explicit Jump (int labelId) : labelId(labelId), offset(0) {}

    int getLabelId() {
        return labelId;
    }

    void setOffset(int offset) {
        this->offset = offset;
    }

    virtual int getSize() {
        return 5;
    }
};

class jmp : public Jump {
public:
    virtual void toNASM(FILE *output) {
        fprintf(output, "    jmp .label%d", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class jg : public Jump {
public:
    virtual void toNASM(FILE *output) {
        fprintf(output, "    jg .label%d", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};


// TODO jge, jl, jle, je, jne

class comment : public Operation {
private:
    const char *msg;
public:
    comment(const char *msg) : msg(msg) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    ; %s\n", msg);
    }

    virtual void toBytecode(Bytecode buf) {}
};

class inc_reg : public Operation {
private:
    REGISTER reg;
public:
    inc_reg(REGISTER reg) : reg(reg) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    inc %s\n", regToText(reg));
    }
};

class inc_rm_off8 : public Operation {
private:
    REGISTER reg;
    char offset;
public:
    inc_rm_off8(REGISTER reg, char offset) : reg(reg), offset(offset) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    inc [%s%+d]\n", regToText(reg), offset);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class inc_rm_off32 : public Operation {
private:
    REGISTER reg;
    int offset;
public:
    inc_rm_off32(REGISTER reg, int offset) : reg(reg), offset(offset) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    inc [%s%+d]\n", regToText(reg), offset);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class dec_reg : public Operation {
private:
    REGISTER reg;
public:
    dec_reg(REGISTER reg) : reg(reg) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    dec %s\n", regToText(reg));
    }
};

class dec_rm_off8 : public Operation {
private:
    REGISTER reg;
    char offset;
public:
    dec_rm_off8(REGISTER reg, char offset) : reg(reg), offset(offset) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    dec [%s%+d]\n", regToText(reg), offset);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class dec_rm_off32 : public Operation {
private:
    REGISTER reg;
    int offset;
public:
    dec_rm_off32(REGISTER reg, int offset) : reg(reg), offset(offset) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    dec [%s%+d]\n", regToText(reg), offset);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class AssemblyListing {
private:
    vector<Operation *> ops;                                         // Vector of operations i. e. part of the program
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
    size_t appendBytesToData(const char *bytes,
                             int len);           // Appends bytes to data, returns offset from the beginning of .data section

    void setMainListing(int pos);                                   // Set listing for main function

    void toNASM(char *filename);                                    // Translate program to Netwide Assembly
    void toBytecode(char *filename);                                // Translate program to plain bytecode
    void toELF(char *filename);                                     // Generate ELF file
};

#endif //X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP
