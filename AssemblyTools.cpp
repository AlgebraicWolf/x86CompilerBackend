//
// Created by alexey on 17.05.2020.
//

#include "AssemblyTools.hpp"

Operation::Operation(OP_TYPE op, REGISTER reg1, REGISTER reg2, char rm_off_8, int rm_off_32, int id, size_t dataOffset)
        : op(op), reg1(reg1), reg2(reg2), rm_off_8(rm_off_8), rm_off_32(rm_off_32), id(id), dataOffset(dataOffset) {}




AssemblyListing::AssemblyListing() = default;

AssemblyListing::AssemblyListing(AssemblyListing &&other) noexcept {
    std::swap(*this, other);
}

AssemblyListing& AssemblyListing::operator=(AssemblyListing &&other) noexcept {
    std::swap(*this, other);
    return *this;
}

