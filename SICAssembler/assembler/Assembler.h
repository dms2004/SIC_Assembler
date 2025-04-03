#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <map>
#include "table.h"

class Assembler {
public:
    Assembler(const std::string& opcode_file) {
        optab.init(opcode_file);
    }
    virtual ~Assembler() = default; // Virtual destructor for proper cleanup
    void assemble(const std::string& filename);

protected:
    // Data Structures (can be used by derived classes)
    Table symtab;
    Table optab;
    Table block_table;
    std::map<int, int> loc_counter;
    std::string program_length;
    std::string start_address;
    std::string program_name;
    
    virtual int addressTranslation(const std::string& opcode, const std::string& line) = 0;
    virtual std::tuple<std::string, int, bool> generateObjectCode(const std::string& opcode, const std::string& operand, std::string opcodeValue, std::string operandAddress) = 0;
    bool pass1(const std::string& filename);
    bool pass2(const std::string& filename);
};

#endif // ASSEMBLER_H
