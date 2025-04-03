#include "SICasm.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Invalid Argument";
        return 1;
    }
    SIC_assembler sic("../sic/opcode");
    sic.assemble(argv[1]);
    return 0;
}
