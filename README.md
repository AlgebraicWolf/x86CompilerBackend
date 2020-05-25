# Compiler backend for x86 Architechture

## Intoduction 
In the previous semester I've created processor emulator along with its own assembler and, later, programming language that supports this custom processor as compilation target. Now my goal is to design backend that support x86 as target platform. 

## Usage 

`x86CompilerBackend -i <input AST file> -o <output AST file> [-n]`

+ `-i` &ndash; Input file specifier
+ `-o` &ndash; Output file specifier
+ `-n` allows translation into Netwide Assembly instead of binary code

## Architechture of compiler backend

There are two main parts of this backend: one is assembly generation library that allows to easily and conveniently generate assembly or binary files. It provides two main classes: `AssemblyProgram` and `AssemblyListing`.

`AssemblyListing` resembles individual function. It allows for use of local labels. Label can be added using `AssemblyListing::addLocalLabel()` function. Local labels are reffered to by their number, and previous function returns number of label. For instructions there is convenient interface: for instance, if you want to write `int 80h`, you can use `AssemblyListing::interrupt()` function. The same goes for many others.

`AssemblyProgram` is a containter for `AssemblyListing` objects. It allows to add them, select `main` and translate the whole thing into NASM, Binary file or ELF file. It supports lazy compilation of functions i. e. functions that are not called using `call` will not be compiled.

`AbstractSyntaxTree` library supports loading of AST and compile them using previous library.