#include <iostream>
#include <exception>
#include <getopt.h>

#include "utilities.hpp"
#include "AbstractSyntaxTree.hpp"
#include "AssemblyTools.hpp"


void parseArgs(int argc, char *argv[], bool &toNasm, char *&input, char *&output);

int main(int argc, char *argv[]) {
    char *input = nullptr;
    char *output = nullptr;
    bool toNasm = false;

    parseArgs(argc, argv, toNasm, input, output);

    if(!input) {
        printf("\nInput file is not specified\n");
        return 0;
    }

    if(!output) {
        output = "output";
    }

    AbstractSyntaxTree prog;
    prog.load(input);

    AssemblyProgram compiled = prog.compile(); // Compile program

    if(toNasm) {
        compiled.toNASM(output);
    } else {
        compiled.toELF(output);
    }

    return 0;
}

void parseArgs(int argc, char *argv[], bool &toNasm, char *&input, char *&output) {
    int res = 0;
    while ((res = getopt(argc, argv, "i:o:n")) != -1) {
        switch (res) {
            case 'i':
                input = optarg;
                break;

            case 'o':
                output = optarg;
                break;

            case 'n':
                toNasm = true;
                break;

            case '?':
                printf("\nInvalid argument: %c\n", res);
                exit(0);
        }
    }
}