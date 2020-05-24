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
            listings[i].placeLocalLabelJumpOffsets();
            listingPositions[i] = pos;
            pos += listings[i].getSize();
        } else {
            listingPositions[i] = -1;
        }
    }

    pos = 0;
    for (int i = 0; i < listings.getSize(); i++) {
        pos = listings[i].placeCallOffsets(listingPositions, pos);
    }

    return listingPositions;
}

void AssemblyListing::placeLocalLabelJumpOffsets() {
    int pos = 0;
    for (int i = 0; i < ops.getSize(); i++) {
        pos += ops[i]->getSize();
        if (ops[i]->getType() == JUMP_OP) {
            Jump *op_jump = reinterpret_cast<Jump *>(ops[i]);
            op_jump->setOffset(labels[op_jump->getLabelId()] - pos);
        }
    }
}

int AssemblyListing::placeCallOffsets(const int *listingPositions, int pos) {
    for(int i = 0; i < ops.getSize(); i++) {
        pos += ops[i]->getSize();
        if(ops[i]->getType() == CALL_OP) {
            auto *op_call = reinterpret_cast<class call *>(ops[i]);
            op_call->setOffset(listingPositions[op_call->getListingId()] - pos);
        }
    }
    return pos;
}

Bytecode AssemblyProgram::toBytecode() {
    Bytecode buf; // Buffer for program

    int *listingPositions = prepare();

    // call main_listing
    buf.append_byte(0xe8);
    buf.append(static_cast<unsigned int>(listingPositions[main] + 9));

    // mov eax, 1
    buf.append_byte(0xb8);
    buf.append(static_cast<unsigned int>(1));

    // xor ebx, ebx
    buf.append_byte(0x31);
    buf.append_byte(0xdb);

    // int 80h
    buf.append_byte(0xcd);
    buf.append_byte(0x80);

    for (int i = 0; i < listings.getSize(); i++) {
        if (listingPositions[i] != -1) {
            listings[i].toBytecode(buf);
        }
    }

    delete[] listingPositions;
    return buf;
}

void AssemblyListing::toBytecode(Bytecode &buf) {
    for (int i = 0; i < ops.getSize(); i++) {
        ops[i]->toBytecode(buf);
    }
}

void AssemblyProgram::toELF(char *filename) {
    Bytecode executable = toBytecode(); // Translate executable into bytecode
    unsigned int executableSize = executable.getSize();


    ELFHeader header = {
            .EI_MAG = {0x7f, 'E', 'L', 'F'}, // Signature
            .EI_CLASS = 1, // 32 bit executable
            .EI_DATA = 1, // Little endian
            .EI_VERSION = 1, // Current version
            .EI_OSABI = 0, // UNIX System V ABI
            .EI_ABIVERSION = 0, // ABI version 0
            .EI_PAD0 = 0, // Reserved
            .EI_PAD1 = 0, // Reserved
            .EI_PAD2 = 0, // Reserved
            .EI_PAD3 = 0, // Reserved
            .EI_PAD4 = 0, // Reserved
            .EI_PAD5 = 0, // Reserved
            .EI_PAD6 = 0, // Reserved
            .e_type = 2, // Executable file
            .e_machine = 3, // Intel 80386
            .e_version = 1, // Format vesion: current
            .e_entry = 0x08048074, // Entry point
            .e_phoff = 52, // Header Table is located right after this header
            .e_shoff = 0, // I don't really head section headers tbh
            .e_flags = 0, // Don't need any flags
            .e_ehsize = 52, // ELF Header size for 32-bit file
            .e_phentsize = 32, // Program Header size for 32-bit file
            .e_phnum = 2, // Number of program headers
            .e_shentsize = 40, // Size of section header
            .e_shnum = 0, // No section headers
            .e_shstrndx = 0 // No names

    };

    ELFProgramHeader headers[2] = {};
    headers[0] = {
            .p_type = 1, // Load to memory
            .p_offset = 0, // From the beginning of file
            .p_vaddr = 0x08048000, // Virtual address
            .p_paddr = 0x08048000, // Physical address
            .p_filesz = 0x74, // Size in file
            .p_memsz = 0x74, // Size in memory. Exactly ELF file and two headers
            .p_flags = 4, // Readable
            .p_align = 0x10 // Align by 16 bytes border
    };
    headers[1] = {
            .p_type = 1, // Load
            .p_offset = 0x80, // Right after all the headers
            .p_vaddr = 0x8048080, // Virtual address
            .p_paddr = 0x8048080, // Physical address
            .p_filesz = executableSize, // Size is equal to the size of program
            .p_memsz = executableSize, // Size in memory is the same
            .p_flags = 1 | 4, // Executable and readable,
            .p_align = 0x10 // Align by 16 bytes
    };

    FILE *output = fopen(filename, "wb");

    if(!output)
        throw_exception("Unable to open the file for writing");

    fwrite(&header, sizeof(ELFHeader), 1, output);
    fwrite(headers, sizeof(ELFProgramHeader), 2, output);
    fwrite(executable.data(), sizeof(unsigned char), executable.getSize(), output);
    fclose(output);

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


