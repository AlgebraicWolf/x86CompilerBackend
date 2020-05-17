#include <cstring>
#include <cstdio>
#include <cctype>
#include <exception>

#include "utilities.hpp"

//
// Created by alexey on 17.05.2020.
//

size_t getFilesize(FILE *f) {
    if (!f)
        throw_exception("Invalid file pointer is provided");

    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    return sz;
}

char *skipSpaces(char *str) {
    while (isblank(*str) && *str != '\0')
        str++;

    return str;
}

const char *skipSpaces(const char *str) {
    while (isblank(*str) && *str != '\0')
        str++;

    return str;
}

const char *getNum(const char *serializedString, int &dst) {
    if (!serializedString)
        throw_exception("Invalid string pointer is provided to number parser");

    dst = 0;
    int scanned = 0;
    sscanf(serializedString, "%d%n", &dst, &scanned);

    return serializedString + scanned;
}

const char *getIdentifier(const char *serialized, int& length) {
    if(!serialized)
        throw_exception("Invalid string pointer is provided to identifier parser");

    const char *identifier = nullptr;
    sscanf(serialized, "%m[a-zA-Z_]%n", &identifier, &length);

    return identifier;
}

