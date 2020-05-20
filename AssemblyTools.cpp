//
// Created by alexey on 17.05.2020.
//

#include "AssemblyTools.hpp"
#include "utilities.hpp"



void AssemblyListing::addOperation(Operation *op) {
    ops.push_back(op);
    pos += op->getSize();
}

void AssemblyListing::mov(REGISTER to, int imm) {
    addOperation(new mov_reg_imm(to, imm));
}

void AssemblyListing::mov(REGISTER to, REGISTER from) {
    addOperation(new mov_reg_reg(to, from));
}

void AssemblyListing::mov(REGISTER to, REGISTER ptr, int offset) {
    addOperation(new mov_reg_rm_off32(to, ptr, offset));
}

void AssemblyListing::mov(REGISTER to, REGISTER ptr, char offset) {
    addOperation(new mov_reg_rm_off8(to, ptr, offset));
}

void AssemblyListing::mov(REGISTER ptr, char offset, REGISTER from) {
    addOperation(new mov_rm_reg_off8(ptr, offset, from));
}

void AssemblyListing::mov(REGISTER ptr, int offset, REGISTER from) {
    addOperation(new mov_rm_reg_off32(ptr, offset, from));
}

void AssemblyListing::nop() {
    addOperation(new class nop());
}

void AssemblyListing::ret() {
    addOperation(new class ret());
}

void AssemblyListing::ret(unsigned short to_pop) {
    addOperation(new ret_pop(to_pop));
}

int AssemblyListing::addLocalLabel() {
    int num = labels.getSize();
    labels.push_back(pos);
    addOperation(new label(pos));
    return num;
}

void AssemblyListing::interrupt(char int_num) {
    addOperation(new class interrupt(int_num));
}

void AssemblyListing::call(int functionId) {
    addOperation(new class call(functionId));
}

void AssemblyListing::jmp(int labelId) {
    addOperation(new class jmp(labelId));
}

void AssemblyListing::je(int labelId) {
    addOperation(new class je(labelId));
}

void AssemblyListing::jne(int labelId) {
    addOperation(new class jne(labelId));
}

void AssemblyListing::jl(int labelId) {
    addOperation(new class jl(labelId));
}

void AssemblyListing::jle(int labelId) {
    addOperation(new class jle(labelId));
}

void AssemblyListing::jg(int labelId) {
    addOperation(new class jg(labelId));
}

void AssemblyListing::jge(int labelId) {
    addOperation(new class jge(labelId));
}

void AssemblyListing::comment(const char *msg) {
    addOperation(new class comment(msg));
}

void AssemblyListing::inc(REGISTER what) {
    addOperation(new inc_reg(what));
}

void AssemblyListing::inc(REGISTER ptr, char offset) {
    addOperation(new inc_rm_off8(ptr, offset));
}

void AssemblyListing::inc(REGISTER ptr, int offset) {
    addOperation(new inc_rm_off32(ptr, offset));
}

void AssemblyListing::dec(REGISTER what) {
    addOperation(new dec_reg(what));
}

void AssemblyListing::dec(REGISTER ptr, char offset) {
    addOperation(new dec_rm_off8(ptr, offset));
}

void AssemblyListing::dec(REGISTER ptr, int offset) {
    addOperation(new dec_rm_off32(ptr, offset));
}

void AssemblyListing::imul(REGISTER multiplier) {
    addOperation(new imul_reg(multiplier));
}

void AssemblyListing::idiv(REGISTER divisor) {
    addOperation(new idiv_reg(divisor));
}

AssemblyListing::~AssemblyListing() {
    for(int i = 0; i < ops.getSize(); i++) {
        delete ops[i];
    }
}

AssemblyListing::AssemblyListing() : ops(), labels(), pos(0) {}

AssemblyListing::AssemblyListing(AssemblyListing &&other) noexcept {
    std::swap(*this, other);
}

AssemblyListing& AssemblyListing::operator=(AssemblyListing &&other) noexcept {
    std::swap(*this, other);
    return *this;
}


AssemblyProgram::AssemblyProgram() : listings(), main(0) {}

AssemblyProgram::AssemblyProgram(AssemblyProgram &&other) noexcept {
    swap(*this, other);
}

AssemblyProgram& AssemblyProgram::operator=(AssemblyProgram &&other) noexcept {
    swap(*this, other);
    return *this;
}


void AssemblyProgram::setMainListing(int pos) {
    main = pos;
}

int AssemblyProgram::pushListing(AssemblyListing &&lst) {
    listings.push_back(forward<AssemblyListing>(lst));
    return listings.getSize() - 1;
}

void AssemblyProgram::toNASM(char *filename) {
    FILE *output = fopen(filename, "w");

    if(!output)
        throw_exception("Unable to open file");

    fprintf(output, "SEGMENT .text\n"
                            "GLOBAL _start\n\n"
                            "_start:\n"
                            "    call listing%d\n"
                            "    mov eax, 1\n"
                            "    xor ebx, ebx\n"
                            "    int 80h\n\n", main);


}


