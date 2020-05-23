//
// Created by alexey on 17.05.2020.
//

#include "AssemblyTools.hpp"
#include "utilities.hpp"

const int ADDED = 1;
const int PROCESSED = 2;

#define CASE_REGNAME(X) case X: return #X

constexpr const char *regToText(REGISTER reg) {
    switch (reg) {
        CASE_REGNAME(EAX);
        CASE_REGNAME(EBX);
        CASE_REGNAME(ECX);
        CASE_REGNAME(EDX);
        CASE_REGNAME(EBP);
        CASE_REGNAME(ESP);
        CASE_REGNAME(ESI);
        CASE_REGNAME(EDI);
        CASE_REGNAME(DL);
    }
    return "INVALID_REG";
}

#undef CASE_REGNAME

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

void AssemblyListing::mov(REGISTER ptr, char offset, char byte) {
    addOperation(new mov_rm_imm8_off8(ptr, offset, byte));
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
    addOperation(new label(num));
    return num;
}

void AssemblyListing::interrupt(unsigned char int_num) {
    addOperation(new class interrupt(int_num));
}

void AssemblyListing::call(int functionId) {
    requiredListings.push_back(functionId);
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

void AssemblyListing::add(REGISTER to, REGISTER what) {
    addOperation(new add_reg_reg(to, what));
}

void AssemblyListing::add(REGISTER to, int imm) {
    addOperation(new add_reg_imm(to, imm));
}

void AssemblyListing::add(REGISTER ptr, char offset, int imm) {
    addOperation(new add_rm_imm_off8(ptr, offset, imm));
}

void AssemblyListing::add(REGISTER ptr, int offset, int imm) {
    addOperation(new add_rm_imm_off32(ptr, offset, imm));
}

void AssemblyListing::sub(REGISTER to, REGISTER what) {
    addOperation(new sub_reg_reg(to, what));
}

void AssemblyListing::sub(REGISTER to, int imm) {
    addOperation(new sub_reg_imm(to, imm));
}

void AssemblyListing::sub(REGISTER ptr, char offset, int imm) {
    addOperation(new sub_rm_imm_off8(ptr, offset, imm));
}

void AssemblyListing::sub(REGISTER ptr, int offset, int imm) {
    addOperation(new sub_rm_imm_off32(ptr, offset, imm));
}

void AssemblyListing::push(REGISTER reg) {
    addOperation(new push_reg(reg));
}

void AssemblyListing::pop(REGISTER reg) {
    addOperation(new pop_reg(reg));
}

void AssemblyListing::cmp(REGISTER fst, REGISTER snd) {
    addOperation(new cmp_reg_reg(fst, snd));
}

void AssemblyListing::cmp(REGISTER fst, int snd) {
    addOperation(new cmp_reg_imm(fst, snd));
}

void AssemblyListing::neg(REGISTER what) {
    addOperation(new neg_reg(what));
}

void AssemblyListing::logical_and(REGISTER what, unsigned int imm) {
    addOperation(new and_reg_imm(what, imm));
}

int AssemblyListing::getLabelCount() {
    return labels.getSize();
}


AssemblyListing::~AssemblyListing() {
    for (int i = 0; i < ops.getSize(); i++) {
        delete ops[i];
    }
}

AssemblyListing::AssemblyListing() : ops(), labels(), pos(0) {}

AssemblyListing::AssemblyListing(AssemblyListing &&other) noexcept : pos(other.pos), ops(std::move(other.ops)),
                                                                     labels(std::move(other.labels)), requiredListings(
                std::move(other.requiredListings)) {}

AssemblyListing &AssemblyListing::operator=(AssemblyListing &&other) noexcept {
    pos = other.pos;
    ops = std::move(other.ops);
    labels = std::move(other.labels);
    requiredListings = std::move(other.requiredListings);


    return *this;
}

int AssemblyListing::getSize() {
    return pos;
}

void AssemblyListing::toNASM(FILE *output) {
    for (int i = 0; i < ops.getSize(); i++) {
        ops[i]->toNASM(output);
    }
}

bool AssemblyListing::markRequiredFunctions(int *listingPositions) {
    bool updated = false;

    for (int i = 0; i < requiredListings.getSize(); i++) {
        if (!listingPositions[requiredListings[i]]) {
            listingPositions[requiredListings[i]] = ADDED;
            updated = true;
        }
    }

    return updated;
}

AssemblyProgram::AssemblyProgram() : listings(), main(0) {}

AssemblyProgram::AssemblyProgram(AssemblyProgram &&other) noexcept {
    swap(*this, other);
}

AssemblyProgram &AssemblyProgram::operator=(AssemblyProgram &&other) noexcept {
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

int *AssemblyProgram::prepare() {
    int *listingPositions = new int[listings.getSize()];

    listingPositions[main] = ADDED;

    bool modified = true;
    while (modified) {                               // Mark functions that are required to run the program
        modified = false;
        for (int i = 0; i < listings.getSize(); i++) {
            if (listingPositions[i] == ADDED) {
                modified |= listings[i].markRequiredFunctions(listingPositions);
                listingPositions[i] = PROCESSED;
            }
        }
    }

    int pos = 0;                        // Calculate position of each listing

    for (int i = 0; i < listings.getSize(); i++) {
        if (listingPositions[i]) {
//            listings[i].placeLocalLabelJumpOffsets();   // Better to relocate to AssemblyListing::toNASM, though it is not required for translation to nasm tbh
            listingPositions[i] = pos;
            pos += listings[i].getSize();
        } else {
            listingPositions[i] = -1;
        }
    }

    for (int i = 0; i < listings.getSize(); i++) {
//        listings[i].placeCallOffsets(listingPositions);
    }

    return listingPositions;
}

void AssemblyProgram::toNASM(char *filename) {
    FILE *output = fopen(filename, "w");

    if (!output)
        throw_exception("Unable to open file");

    fprintf(output, "SEGMENT .text\n"
                    "GLOBAL _start\n\n"
                    "_start:\n"
                    "    call listing%d\n"
                    "    mov eax, 1\n"
                    "    xor ebx, ebx\n"
                    "    int 80h\n\n", main);

    int *listingPositions = prepare();

    for (int i = 0; i < listings.getSize(); i++) {
        if (listingPositions[i] != -1) {
            fprintf(output, "listing%d:\n", i);
            listings[i].toNASM(output);
            fprintf(output, "\n\n");
        }
    }
    delete[] listingPositions;

    // TODO deal with data section

    fclose(output);
}


