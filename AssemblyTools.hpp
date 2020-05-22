//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP
#define X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP

#include <cstdio>
#include "Vector.hpp"

enum REGISTER {
    EAX,
    EBX,
    ECX,
    EDX,
    EBP,
    ESP,
    ESI,
    EDI,
    DL
};

constexpr const char *regToText(REGISTER reg);

//enum OP_TYPE {
////    NOP,
////    INT,
////    MOV_REG_REG,
////    MOV_REG_IMM,
//            MOV_REG_RAM,
//    MOV_RAM_REG,
////    MOV_RM_REG_OFF8,
////    MOV_RM_REG_OFF32,
////    MOV_REG_RM_OFF8,
////    MOV_REG_RM_OFF32,
//            JMP,
////    JG,
////    JGE,
////    JL,
////    JLE,
////    JE,
////    JNE,
////    RET,
////    CALL,
//    DIV,
//    MUL,
//    INC,
//    DEC
//};



class Bytecode {

};

class Operation {
private:

public:
    virtual void toNASM(FILE *output) = 0;                          // Translate instruction to NASM
    virtual void toBytecode(Bytecode buf) = 0;                      // Translate instruction to bytecode
    virtual int getSize() = 0;                                      // Length of command in bytes
    virtual ~Operation() = default;
};

class nop : public Operation {
public:
    virtual void toNASM(FILE *output) {
        fprintf(output, "    nop\n");
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 1;
    }
};

class interrupt : public Operation {
private:
    unsigned char int_num;
public:
    interrupt(unsigned char int_num) : int_num(int_num) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    int %u\n", int_num);
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 2;
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

    virtual int getSize() {
        return 5; // B8 + rd + id
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

    virtual int getSize() {
        return 2;
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

    virtual int getSize() {
        return 3;
    }
};

class mov_rm_imm8_off8 : public Operation {
private:
    REGISTER rmreg;
    char offset;
    char imm;
public:
    mov_rm_imm8_off8(REGISTER rmreg, char offset, char imm) : rmreg(rmreg), offset(offset), imm(imm) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    mov byte [%s%+d], %d\n", regToText(rmreg), offset, imm);
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 4;
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

    virtual int getSize() {
        return 6;
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

    virtual int getSize() {
        return 3;
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

    virtual int getSize() {
        return 6;
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

    virtual int getSize() {
        return 5;
    }
};

class ret : public Operation {
private:

public:
    virtual void toNASM(FILE *output) {
        fprintf(output, "    ret\n");
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 1;
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

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 3;
    }
};

class Jump : public Operation {
protected:
    int offset;
    int labelId;
public:
    explicit Jump(int labelId) : labelId(labelId) {}

    int getLabelId() {
        return labelId;
    }

    void setOffset(int offset) {
        this->offset = offset;
    }

    virtual int getSize() {
        return 6;
    }
};

class jmp : public Jump {
public:
    explicit jmp(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jmp .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 5;
    }
};

class jg : public Jump {
public:
    explicit jg(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jg .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class jge : public Jump {
public:
    explicit jge(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jge .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class jl : public Jump {
public:
    explicit jl(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jl .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class jle : public Jump {
public:
    explicit jle(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jle .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class je : public Jump {
public:
    explicit je(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    je .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class jne : public Jump {
public:
    explicit jne(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jne .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode buf) {

    }
};

class comment : public Operation {
private:
    const char *msg;
public:
    comment(const char *msg) : msg(msg) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    ; %s\n", msg);
    }

    virtual void toBytecode(Bytecode buf) {}

    virtual int getSize() {
        return 0;
    }
};

class inc_reg : public Operation {
private:
    REGISTER reg;
public:
    inc_reg(REGISTER reg) : reg(reg) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    inc %s\n", regToText(reg));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 1;
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

    virtual int getSize() {
        return 4;
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

    virtual int getSize() {
        return 7;
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

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 1;
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

    virtual int getSize() {
        return 4;
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

    virtual int getSize() {
        return 7;
    }
};

class idiv_reg : public Operation {
private:
    REGISTER divisor;
public:
    idiv_reg(REGISTER divisor) : divisor(divisor) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    idiv %s\n", regToText(divisor));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 2;
    }
};

class imul_reg : public Operation {
private:
    REGISTER multiplier;
public:
    imul_reg(REGISTER multiplier) : multiplier(multiplier) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    imul %s\n", regToText(multiplier));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 2;
    }
};

class add_reg_reg : public Operation {
private:
    REGISTER to;
    REGISTER what;
public:
    add_reg_reg(REGISTER to, REGISTER what) : to(to), what(what) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    add %s, %s\n", regToText(to), regToText(what));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 2;
    }
};

class add_reg_imm : public Operation {
private:
    REGISTER to;
    int value;
public:
    add_reg_imm(REGISTER to, int value) : to(to), value(value) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    add %s, %d\n", regToText(to), value);
    };

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 6;
    }
};

class add_rm_imm_off8 : public Operation {
private:
    REGISTER ptr;
    char offset;
    int value;
public:
    add_rm_imm_off8(REGISTER ptr, char offset, int value) : ptr(ptr), offset(offset), value(value) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    add [%s%+d], %d\n", regToText(ptr), offset, value);
    };

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 7;
    }
};

class add_rm_imm_off32 : public Operation {
private:
    REGISTER ptr;
    int offset;
    int value;
public:
    add_rm_imm_off32(REGISTER ptr, int offset, int value) : ptr(ptr), offset(offset), value(value) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    add [%s%+d], %d\n", regToText(ptr), offset, value);
    };

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 10;
    }
};

class pop_reg : public Operation {
private:
    REGISTER reg;
public:
    pop_reg(REGISTER reg) : reg(reg) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    pop %s\n", regToText(reg));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 1;
    }
};

class push_reg : public Operation {
private:
    REGISTER reg;
public:
    push_reg(REGISTER reg) : reg(reg) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    push %s\n", regToText(reg));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 1;
    }
};

class sub_reg_reg : public Operation {
private:
    REGISTER to;
    REGISTER what;
public:
    sub_reg_reg(REGISTER to, REGISTER what) : to(to), what(what) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    sub %s, %s\n", regToText(to), regToText(what));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 2;
    }
};

class sub_reg_imm : public Operation {
private:
    REGISTER to;
    int value;
public:
    sub_reg_imm(REGISTER to, int value) : to(to), value(value) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    sub %s, %d\n", regToText(to), value);
    };

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 6;
    }
};

class sub_rm_imm_off8 : public Operation {
private:
    REGISTER ptr;
    char offset;
    int value;
public:
    sub_rm_imm_off8(REGISTER ptr, char offset, int value) : ptr(ptr), offset(offset), value(value) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    sub [%s%+d], %d\n", regToText(ptr), offset, value);
    };

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 7;
    }
};

class sub_rm_imm_off32 : public Operation {
private:
    REGISTER ptr;
    int offset;
    int value;
public:
    sub_rm_imm_off32(REGISTER ptr, int offset, int value) : ptr(ptr), offset(offset), value(value) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    sub [%s%+d], %d\n", regToText(ptr), offset, value);
    };

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 10;
    }
};

class cmp_reg_reg : public Operation {
private:
    REGISTER first;
    REGISTER second;
public:
    cmp_reg_reg(REGISTER first, REGISTER second) : first(first), second(second) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    cmp %s, %s\n", regToText(first), regToText(second));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 2;
    }
};

class cmp_reg_imm : public Operation {
private:
    REGISTER first;
    int second;
public:
    cmp_reg_imm(REGISTER first, int second) : first(first), second(second) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    cmp %s, %d\n", regToText(first), second);
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 2;
    }
};

class neg_reg : public Operation {
private:
    REGISTER what;
public:
    neg_reg(REGISTER what) : what(what) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    neg %s\n", regToText(what));
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 2;
    }
};

class and_reg_imm : public Operation {
private:
    REGISTER what;
    unsigned int imm;
public:
    and_reg_imm(REGISTER what, unsigned int imm) : what(what), imm(imm) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    and %s, %u\n", regToText(what), imm);
    }

    virtual void toBytecode(Bytecode buf) {

    }

    virtual int getSize() {
        return 6;
    }
};

class label : public Operation {
private:
    int num;

public:
    label(int num) : num(num) {}

    virtual void toNASM(FILE* output) {
        fprintf(output, ".label%d:\n", num);
    }

    virtual void toBytecode(Bytecode buf) {}

    virtual int getSize() {
        return 0;
    }
};

class AssemblyListing {
private:
    vector<Operation *> ops;                                        // Vector of operations i. e. part of the program
    vector<unsigned int> labels;                                    // Positions of local labels relative to the beginning of the listing
    vector<int> requiredListings;                                   // IDs of listings that are required for this one to function
    unsigned int pos;                                               // Current position of the end of the listing

    void addOperation(Operation* op);
public:
    AssemblyListing();                                              // Default constructor
    AssemblyListing(AssemblyListing &&other) noexcept;              // Move constructor
    AssemblyListing &operator=(AssemblyListing &&other) noexcept;   // Move assignment
    AssemblyListing(const AssemblyListing &other) = delete;         // Prohibit copy constructor
    AssemblyListing &operator=(const AssemblyListing &other) = delete;// Prohibit copy assignment
    ~AssemblyListing();                                             // Destructor

    bool markRequiredFunctions(int *listingPositions);              // Mark unmarked functions for compilation
    int getSize();                                                  // Return size of listing
    void placeCallOffsets(const int *listingPositions);             // Place offsets in call functions
    void placeLocalLabelJumpOffsets();                              // Place offsets in local label jumps

    void toNASM(FILE *output);                                      // Translate listing into NASM file

    void interrupt(unsigned char int_num);                          // interrupt

    void nop();                                                     // Good ol' nop

    void mov(REGISTER ptr, char offset, char byte);                 // move byte ptr [ptr+offset], byte
    void mov(REGISTER to, int imm);                                 // mov to, immediate
    void mov(REGISTER to, REGISTER from);                           // mov to, from
    void mov(REGISTER to, REGISTER ptr, char offset);               // mov to, size(to) ptr [from + offset]
    void mov(REGISTER to, REGISTER ptr, int offset);                // mov to, size(to) ptr [from + offset]
    void mov(REGISTER to, size_t addr);                             // mov to, size(to) ptr [addr]
    void mov(size_t addr, REGISTER from);                           // mov size(from) ptr [addr], from
    void mov(REGISTER ptr, char offset, REGISTER from);             // mov size(from) [ptr + offset], from
    void mov(REGISTER ptr, int offset, REGISTER from);              // mov size(from) [ptr + offset], from

    int addLocalLabel();                                            // Inserts local label at current position
    int getLabelCount();                                            // Get number of local labels in current listing
    void comment(const char *msg);                                  // Insert comment

    void cmp(REGISTER fst, REGISTER snd);                           // cmp fst, snd
    void cmp(REGISTER fst, int snd);                           // cmp fst, snd

    void jmp(int labelId);                                          // jmp labelId
    void jg(int labelId);                                           // jg labelId
    void jge(int labelId);                                          // jge labelId
    void jl(int labelId);                                           // jl labelId
    void jle(int labelId);                                          // jle labelId
    void je(int labelId);                                           // je labelId
    void jne(int labelId);                                          // jne labelId

    void ret();                                                     // ret, yeah
    void ret(unsigned short to_pop);                                // ret that pops value
    void call(int functionId);                                      // call functionId

    void idiv(REGISTER divisor);
    void imul(REGISTER multiplier);

    void inc(REGISTER what);                                        // inc register
    void inc(REGISTER ptr, char offset);                            // inc [ptr+off]
    void inc(REGISTER ptr, int offset);                             // inc [ptr+off]
    void dec(REGISTER what);                                        // dec register
    void dec(REGISTER ptr, char offset);                            // dec [ptr+off]
    void dec(REGISTER ptr, int offset);                             // dec [ptr+off]

    void add(REGISTER to, REGISTER what);                           // add to, what
    void add(REGISTER to, int imm);                                 // add to, imm
    void add(REGISTER ptr, char offset, int imm);                   // add to, [ptr+offset]
    void add(REGISTER ptr, int offset, int imm);                    // add to, [ptr+offset]

    void sub(REGISTER to, REGISTER what);                           // add to, what
    void sub(REGISTER to, int imm);                                 // sub to, imm
    void sub(REGISTER ptr, char offset, int imm);                   // sub to, [ptr+offset]
    void sub(REGISTER ptr, int offset, int imm);                    // sub to, [ptr+offset]

    void neg(REGISTER what);                                        // neg what
    void logical_and(REGISTER what, unsigned int imm);              // and what, imm

    void push(REGISTER reg);                                        // push reg
    void pop(REGISTER reg);                                         // pop reg
};

class AssemblyProgram {
private:
    vector<AssemblyListing> listings;                               // Vector of assembly listings
    unsigned int main;                                              // Position of main listing

    int *prepare();                                                 // Do required routines i. e. mark functions, place local and global offsets in jumps and calls
public:
    AssemblyProgram();                                              // Default constructor
    AssemblyProgram(AssemblyProgram &&other) noexcept;              // Move counstructor
    AssemblyProgram(const AssemblyProgram &other) = delete;         // Prohibit copy constructor
    AssemblyProgram &operator=(AssemblyProgram &&other) noexcept;   // Move assignment
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
