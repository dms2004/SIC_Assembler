#ifndef SIC_ASSEMBLER_H
#define SIC_ASSEMBLER_H

#include "Assembler.h"
#include <string>
#include <functional> 

class SIC_assembler : public Assembler {
    public:
        SIC_assembler(const std::string& opcode_file)
            : Assembler(opcode_file) {}

    protected: 
        int addressTranslation(const std::string& opcode, const std::string& line) override; 
        std::tuple<std::string, int, bool> generateObjectCode(const std::string& opcode, const std::string& operand, std::string opcodeValue, std::string operandAddress) override;
};

#endif // SIC_ASSEMBLER_H
