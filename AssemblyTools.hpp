//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP
#define X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP

#include <cstdio>
#include "Vector.hpp"

enum REGISTER {
    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI,
    AL,
    CL,
    DL,
    BL,
    AH,
    CH,
    DH,
    BH
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
private:
    vector<unsigned char> bytecode;
public:
    Bytecode() : bytecode() {}

    void append(unsigned char byte) {
        bytecode.push_back(byte);
    }

    void append_byte(unsigned char byte) {
        bytecode.push_back(byte);
    }

    void append(unsigned int dword) {
        bytecode.push_back(reinterpret_cast<unsigned char *>(&dword)[0]);
        bytecode.push_back(reinterpret_cast<unsigned char *>(&dword)[1]);
        bytecode.push_back(reinterpret_cast<unsigned char *>(&dword)[2]);
        bytecode.push_back(reinterpret_cast<unsigned char *>(&dword)[3]);
    }

    void append(unsigned short dword) {
        bytecode.push_back(reinterpret_cast<unsigned char *>(&dword)[0]);
        bytecode.push_back(reinterpret_cast<unsigned char *>(&dword)[1]);
    }

    void append(char byte) {
        bytecode.push_back(*reinterpret_cast<unsigned char *>(&byte));
    }

    void append(int dword) {
        append(*reinterpret_cast<unsigned int *>(&dword));
    }

    int getSize() {
        return bytecode.getSize();
    }

    unsigned char *data() {
        return bytecode.data();
    }
};

enum OP_TYPE {
    OTHER,
    JUMP_OP,
    CALL_OP
};

class Operation {
private:

public:
    virtual void toNASM(FILE *output) = 0;                          // Translate instruction to NASM
    virtual void toBytecode(Bytecode &buf) = 0;                      // Translate instruction to bytecode
    virtual int getSize() = 0;                                      // Length of command in bytes
    virtual OP_TYPE getType() {
        return OTHER;
    }
    virtual ~Operation() = default;
};

class nop : public Operation {
public:
    virtual void toNASM(FILE *output) {
        fprintf(output, "    nop\n");
    }

    virtual void toBytecode(Bytecode &buf) {
        buf.append_byte(0x90); // NOP instruction code
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

    virtual void toBytecode(Bytecode &buf) {
        buf.append_byte(0xcd);
        buf.append(int_num);
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

    virtual void toBytecode(Bytecode &buf) {
        if (to <= EDI) {
            buf.append_byte(0xb8 | to);
            buf.append(imm);
        } else {
            throw_exception("MOV immediate supports only 32-bit registers");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (to <= EDI && from <= EDI) {
            buf.append_byte(0x89); // MOV opcode
            buf.append_byte(0b11000000 | (from << 3) | (to)); // MODE: REG, FROM, TO
        } else {
            throw_exception("Non-32-bit register to register mov is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (from <= EDI) {
            buf.append_byte(0x89); // MOV opcode
            buf.append_byte(0b01000000 | (from << 3) | rmreg); // REG+OFF8 MODE, SRC, DST
        } else if (from >= AL) {
            buf.append_byte(0x88); // MOV opcode
            buf.append_byte(0b01000000 | ((from - AL) << 3) | rmreg); // REG+OFF8 MODE, SRC, DST
        }

        if(rmreg == ESP)
            buf.append_byte(0x24);

        buf.append(offset);
    }

    virtual int getSize() {
        if(rmreg == ESP)
            return 4;

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

    virtual void toBytecode(Bytecode &buf) {
        buf.append_byte(0xc6); // Opcode
        buf.append_byte(0b01000000 | rmreg);
        if(rmreg == ESP)
            buf.append_byte(0x24);

        buf.append(offset);
        buf.append(imm);
    }

    virtual int getSize() {
        if(rmreg == ESP)
            return 5;

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

    virtual void toBytecode(Bytecode &buf) {
        if (from <= EDI) {
            buf.append_byte(0x89); // MOV opcode
            buf.append_byte(0b10000000 | (from << 3) | rmreg); // REG+OFF32 MODE, SRC, DST
        } else if (from >= AL) {
            buf.append_byte(0x88); // MOV opcode
            buf.append_byte(0b10000000 | ((from - AL) << 3) | rmreg); // REG+OFF32 MODE, SRC, DST
        }

        if(rmreg == ESP) {
            buf.append_byte(0x24);
        }

        buf.append(offset);
    }

    virtual int getSize() {
        if(rmreg == ESP)
            return 7;

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

    virtual void toBytecode(Bytecode &buf) {
        if (to <= EDI) {
            buf.append_byte(0x8b); // MOV opcode
            buf.append_byte(0b01000000 | (to << 3) | rmreg); // REG+OFF8 MODE, SRC, DST
        } else if (to >= AL) {
            buf.append_byte(0x8a); // MOV opcode
            buf.append_byte(0b01000000 | ((to - AL) << 3) | rmreg); // REG+OFF8 MODE, SRC, DST
        }

        if(rmreg == ESP)
            buf.append_byte(0x24);

        buf.append(offset);
    }

    virtual int getSize() {
        if(rmreg == ESP)
            return 4;

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

    virtual void toBytecode(Bytecode &buf) {
        if (to <= EDI) {
            buf.append_byte(0x8b); // MOV opcode
            buf.append_byte(0b10000000 | (to << 3) | rmreg); // REG+OFF32 MODE, SRC, DST
        } else if (to >= AL) {
            buf.append_byte(0x8a); // MOV opcode
            buf.append_byte(0b10000000 | ((to - AL) << 3) | rmreg); // REG+OFF32 MODE, SRC, DST
        }
        if(rmreg == ESP)
            buf.append_byte(0x24); // Damn intel why do you need SIB addressing mode?!
        buf.append(offset);
    }

    virtual int getSize() {
        if(rmreg == ESP)
            return 7;

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

    virtual void toBytecode(Bytecode &buf) {
        buf.append_byte(0xe8); // CALL instruction opcode
        buf.append(offset); // Relative shift
    }

    virtual OP_TYPE getType() {
        return CALL_OP;
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

    virtual void toBytecode(Bytecode &buf) {
        buf.append_byte(0xc3); // Ret instruction bytecode
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

    virtual void toBytecode(Bytecode &buf) {
        buf.append_byte(0xc2); // RET imm instrunction bytecode
        buf.append(to_pop); // Amount of bytes to pop
    }

    virtual int getSize() {
        return 3;
    }
};

class Jump : public Operation {
protected:
    int offset;
    int labelId;

    void JccBytecode(Bytecode &buf, unsigned char cc) {
        buf.append_byte(0x0F);
        buf.append(cc);
        buf.append(offset);
    }

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

    virtual OP_TYPE getType() {
        return JUMP_OP;
    }
};

class jmp : public Jump {
public:
    explicit jmp(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jmp .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode &buf) {
        buf.append_byte(0xe9); // Relative jump opcode
        buf.append(offset); // Jump offset
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

    virtual void toBytecode(Bytecode &buf) {
        JccBytecode(buf, 0x8f);
    }
};

class jge : public Jump {
public:
    explicit jge(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jge .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode &buf) {
        JccBytecode(buf, 0x8d);
    }
};

class jl : public Jump {
public:
    explicit jl(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jl .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode &buf) {
        JccBytecode(buf, 0x8c);
    }
};

class jle : public Jump {
public:
    explicit jle(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jle .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode &buf) {
        JccBytecode(buf, 0x8e);
    }
};

class je : public Jump {
public:
    explicit je(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    je .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode &buf) {
        JccBytecode(buf, 0x84);
    }
};

class jne : public Jump {
public:
    explicit jne(int labelId) : Jump(labelId) {}

    virtual void toNASM(FILE *output) {
        fprintf(output, "    jne .label%d\n", labelId);
    }

    virtual void toBytecode(Bytecode &buf) {
        JccBytecode(buf, 0x85);
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

    virtual void toBytecode(Bytecode &buf) {}

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

    virtual void toBytecode(Bytecode &buf) {
        if (reg <= EDI) {
            buf.append_byte(0x40 | reg);
        } else {
            throw_exception("Increment of non-32-bit registers is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (reg <= EDI) {
            buf.append_byte(0xff);
            buf.append_byte(0b01000000 | reg);
            if(reg == ESP) {
                buf.append_byte(0x24);
            }
            buf.append(offset);
        } else {
            throw_exception("Increment addresed by non-32-bit register is not yet supported");
        }
    }

    virtual int getSize() {
        if(reg == ESP)
            return 4;

        return 3;
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

    virtual void toBytecode(Bytecode &buf) {
        if (reg <= EDI) {
            buf.append_byte(0xff);
            buf.append_byte(0b10000000 | reg);
            buf.append(offset);
        } else {
            throw_exception("Increment addresed by non-32-bit register is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (reg <= EDI) {
            buf.append_byte(0x48 | reg);
        } else {
            throw_exception("Increment of non-32-bit registers is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (reg <= EDI) {
            buf.append_byte(0xff);
            buf.append_byte(0b01001000 | reg);
            buf.append(offset);
        } else {
            throw_exception("Decrement addresed by non-32-bit register is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (reg <= EDI) {
            buf.append_byte(0xff);
            buf.append_byte(0b10001000 | reg);
            buf.append(offset);
        } else {
            throw_exception("Increment addresed by non-32-bit register is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (divisor <= EDI) {
            buf.append_byte(0xf7);
            buf.append_byte(0b11111000 | divisor);
        } else {
            throw_exception("Non-32 bit IDIV is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (multiplier <= EDI) {
            buf.append_byte(0xf7);
            buf.append_byte(0b11101000 | multiplier);
        } else {
            throw_exception("Non-32 bit IMUL is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (to <= EDI && what <= EDI) {
            buf.append_byte(0x01);
            buf.append_byte(0b11000000 | (what << 3) | to);
        } else {
            throw_exception("Non-32-bit exceptions are not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (to <= EDI) {
            buf.append_byte(0x81);
            buf.append_byte(0b11000000 | to);
            buf.append(value);
        } else {
            throw_exception("Addition to non-32-bit registers is not supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if(ptr <= EDI) {
            buf.append_byte(0x81);
            buf.append_byte(0x01000000 | ptr);
            if(ptr == ESP)
                buf.append_byte(0x24);

            buf.append(offset);
            buf.append(value);
        } else {
            throw_exception("Non-32-bit register adressing is not supported");
        }
    }

    virtual int getSize() {
        if(ptr == ESP)
            return 8;

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

    virtual void toBytecode(Bytecode &buf) {
        if(ptr <= EDI) {
            buf.append_byte(0x81);
            buf.append_byte(0x10000000 | ptr);
            if(ptr == ESP) {
                buf.append_byte(0x24);
            }
            buf.append(offset);
            buf.append(value);
        } else {
            throw_exception("Non-32-bit register adressing is not supported");
        }
    }

    virtual int getSize() {
        if(ptr == ESP)
            return 11;

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

    virtual void toBytecode(Bytecode &buf) {
        if(reg <= EDI) {
            buf.append_byte(0x58 | reg);
        } else {
            throw_exception("Popping non-32-bit register is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if(reg <= EDI) {
            buf.append_byte(0x50 | reg);
        } else {
            throw_exception("Pushing non-32-bit register is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (to <= EDI && what <= EDI) {
            buf.append_byte(0x29);
            buf.append_byte(0b11000000 | (what << 3) | to);
        } else {
            throw_exception("Non-32-bit exceptions are not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if (to <= EDI) {
            buf.append_byte(0x81);
            buf.append_byte(0b11101000 | to);
            buf.append(value);
        } else {
            throw_exception("Addition to non-32-bit registers is not supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if(ptr <= EDI) {
            buf.append_byte(0x81);
            buf.append_byte(0x01101000 | ptr);

            if(ptr == ESP) {
                buf.append_byte(0x24);
            }

            buf.append(offset);
            buf.append(value);
        } else {
            throw_exception("Non-32-bit register adressing is not supported");
        }
    }

    virtual int getSize() {
        if(ptr == ESP)
            return 8;

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

    virtual void toBytecode(Bytecode &buf) {
        if(ptr <= EDI) {
            buf.append_byte(0x81);
            buf.append_byte(0x10101000 | ptr);

            if(ptr == ESP)
                buf.append_byte(0x24);

            buf.append(offset);
            buf.append(value);
        } else {
            throw_exception("Non-32-bit register adressing is not supported");
        }
    }

    virtual int getSize() {
        if(ptr == ESP)
            return 11;

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

    virtual void toBytecode(Bytecode &buf) {
        if(first <= EDI && second <= EDI) {
            buf.append_byte(0x39);
            buf.append_byte(0b11000000 | (second << 3) | first);
        } else {
            throw_exception("Can only compare 32-bit integers");
        }
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

    virtual void toBytecode(Bytecode &buf) {
        if(first <= EDI) {
            buf.append_byte(0x81);
            buf.append_byte(0b11111000 | first);
            buf.append(second);
        } else {
            throw_exception("Only 32-bit registers support comparison yet");
        }
    }

    virtual int getSize() {
        return 6;
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

    virtual void toBytecode(Bytecode &buf) {
        if (what <= EDI) {
            buf.append_byte(0xf7);
            buf.append_byte(0b11011000 | what);
        } else {
            throw_exception("Non-32 bit NEG is not yet supported");
        }
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

    virtual void toBytecode(Bytecode &buf) {

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

    virtual void toNASM(FILE *output) {
        fprintf(output, ".label%d:\n", num);
    }

    virtual void toBytecode(Bytecode &buf) {}

    virtual int getSize() {
        return 0;
    }
};

class AssemblyListing {
private:
    vector<Operation *> ops;                                        // Vector of operations i. e. part of the program
    vector<int> labels;                                             // Positions of local labels relative to the beginning of the listing
    vector<int> requiredListings;                                   // IDs of listings that are required for this one to function
    unsigned int pos;                                               // Current position of the end of the listing

    void addOperation(Operation *op);

public:
    AssemblyListing();                                              // Default constructor
    AssemblyListing(AssemblyListing &&other) noexcept;              // Move constructor
    AssemblyListing &operator=(AssemblyListing &&other) noexcept;   // Move assignment
    AssemblyListing(const AssemblyListing &other) = delete;         // Prohibit copy constructor
    AssemblyListing &operator=(const AssemblyListing &other) = delete;// Prohibit copy assignment
    ~AssemblyListing();                                             // Destructor

    bool markRequiredFunctions(int *listingPositions);              // Mark unmarked functions for compilation
    int getSize();                                                  // Return size of listing
    int placeCallOffsets(const int *listingPositions, int pos);     // Place offsets in call functions
    void placeLocalLabelJumpOffsets();                              // Place offsets in local label jumps

    void toNASM(FILE *output);                                      // Translate listing into NASM file
    void toBytecode(Bytecode &buf);                                 // Translate listing into bytecode

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
    struct ELFHeader {
        unsigned char EI_MAG[4]; // File signature
        unsigned char EI_CLASS; // Object file class
        unsigned char EI_DATA; // Little Endian or Big Endian
        unsigned char EI_VERSION; // ELF Header Version
        unsigned char EI_OSABI; // OS-specific ABI
        unsigned char EI_ABIVERSION; // Version of ABI
        unsigned char EI_PAD0; // Reserved
        unsigned char EI_PAD1; // Reserved
        unsigned char EI_PAD2; // Reserved
        unsigned char EI_PAD3; // Reserved
        unsigned char EI_PAD4; // Reserved
        unsigned char EI_PAD5; // Reserved
        unsigned char EI_PAD6; // Reserved
        unsigned short e_type; // File type
        unsigned short e_machine; // Architecture
        unsigned int e_version; // Format version
        unsigned int e_entry; // Entry point
        unsigned int e_phoff; // Program headers offset
        unsigned int e_shoff; // Section headers offset
        unsigned int e_flags; // Processor flags
        unsigned short e_ehsize; // ELF Header size
        unsigned short e_phentsize; // Single program header size
        unsigned short e_phnum; // Program header number
        unsigned short e_shentsize; // Section header size
        unsigned short e_shnum; // Number of section heders
        unsigned short e_shstrndx; // Index of record describing section names table
    };

    struct ELFProgramHeader {
        unsigned int p_type; // Segment type
        unsigned int p_offset; // Segment offset relative to the beginning of the file
        unsigned int p_vaddr; // Virtual address of segment in memory
        unsigned int p_paddr; // Physical address of segment in memory
        unsigned int p_filesz; // Size of segment in file. Could be zero
        unsigned int p_memsz; // Size of segment in memort. Could be zero
        unsigned int p_flags; // Segment flags
        unsigned int p_align; // Segment alignment
    };

    vector<AssemblyListing> listings;                               // Vector of assembly listings
    unsigned int main;                                              // Position of main listing

    int *
    prepare();                                                 // Do required routines i. e. mark functions, place local and global offsets in jumps and calls
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

    void toNASM(const char *filename);                              // Translate program to Netwide Assembly
    Bytecode toBytecode();                                          // Translate program to plain bytecode
    void toELF(const char *filename);                               // Generate ELF file
};

#endif //X86COMPILERBACKEND_ASSEMBLYTOOLS_HPP
