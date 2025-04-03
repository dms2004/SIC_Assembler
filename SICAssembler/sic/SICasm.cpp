#include <string>      
#include <algorithm>  
#include <iomanip>
#include <cctype>     
#include <sstream>     
#include <stdexcept>  
#include <functional> 
#include "SICasm.h"


int SIC_assembler::addressTranslation(const std::string& opcode, const std::string& line) {
    // Convert opcode to uppercase for case-insensitive comparison
    std::string upperOpcode = opcode;
    std::transform(upperOpcode.begin(), upperOpcode.end(), upperOpcode.begin(), ::toupper);
    
    // Get the operand portion of the instruction
    std::string operand;
    std::istringstream iss(line);
    std::string temp;
    iss >> temp; // Skip the opcode
    iss >> operand; // Get the operand
    
    if (upperOpcode == "WORD") {
        // WORD directive - always 3 bytes
        return 3;
    } 
    else if (upperOpcode == "BYTE") {
        // BYTE directive - length depends on the operand
        if (operand.empty()) {
            throw std::runtime_error("BYTE directive requires an operand");
        }
        
        if (operand[0] == 'C' || operand[0] == 'c') {
            // Character constant
            if (operand.length() < 3 || operand[1] != '\'' || operand[operand.length()-1] != '\'') {
                throw std::runtime_error("Invalid format for character constant in BYTE directive");
            }
            // Extract the string between the quotes
            std::string charConstant = operand.substr(2, operand.length() - 3);
            return charConstant.length();
        } 
        else if (operand[0] == 'X' || operand[0] == 'x') {
            // Hexadecimal constant
            if (operand.length() < 3 || operand[1] != '\'' || operand[operand.length()-1] != '\'') {
                throw std::runtime_error("Invalid format for hex constant in BYTE directive");
            }
            // Extract the hex string between the quotes
            std::string hexConstant = operand.substr(2, operand.length() - 3);
            // Each pair of hex digits represents one byte
            return (hexConstant.length() + 1) / 2; // Ceiling division
        } 
        else {
            throw std::runtime_error("BYTE directive requires 'C' or 'X' type specifier");
        }
    } 
    else if (upperOpcode == "RESW") {
        // RESW directive - 3 bytes per word reserved
        if (operand.empty()) {
            throw std::runtime_error("RESW directive requires an operand");
        }
        
        try {
            int value = std::stoi(operand);
            if (value < 0) {
                throw std::runtime_error("RESW operand must be non-negative");
            }
            return 3 * value;
        } catch (const std::invalid_argument&) {
            throw std::runtime_error("RESW operand must be a valid integer");
        }
    } 
    else if (upperOpcode == "RESB") {
        // RESB directive - reserves specified number of bytes
        if (operand.empty()) {
            throw std::runtime_error("RESB directive requires an operand");
        }
        
        try {
            int value = std::stoi(operand);
            if (value < 0) {
                throw std::runtime_error("RESB operand must be non-negative");
            }
            return value;
        } catch (const std::invalid_argument&) {
            throw std::runtime_error("RESB operand must be a valid integer");
        }
    } 
    else {
        // Check if the opcode is valid by looking it up in optab
        if (optab.value(upperOpcode, "value").empty()) 
            throw std::runtime_error("Invalid opcode: " + opcode);
        return 3; // Standard instruction - 3 bytes
    }
}

std::tuple<std::string, int, bool> SIC_assembler::generateObjectCode(const std::string& opcode, const std::string& operand, std::string opcodeValue, std::string operandAddress) {
    std::string objectCode = "";
    int objectCodeLength = 0;
    bool isReserveDirective = false;
    
    if (opcodeValue.empty()) {
        // Handle directives like BYTE, WORD, RESB, RESW
        if (opcode == "BYTE") {
            if (operand[0] == 'C') {
                // Convert character constant to hex
                for (size_t i = 2; i < operand.length() - 1; i++) {
                    char c = operand[i];
                    std::stringstream ss;
                    ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(c);
                    objectCode += ss.str();
                }
            } else if (operand[0] == 'X') {
                // Extract hex constant
                objectCode = operand.substr(2, operand.length() - 3);
            }
            
            objectCodeLength = objectCode.length() / 2;
        } 
        else if (opcode == "WORD") {
            // Convert decimal to 6-digit hex
            int value = std::stoi(operand);
            std::stringstream ss;
            ss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << value;
            objectCode = ss.str();
            objectCodeLength = 3; // WORD = 3 bytes
        } 
        else if ( opcode == "RESW") {
            // Reserve bytes/words - marked for ending the current text record
            int value = std::stoi(operand);
            objectCodeLength = 3 * value;
            isReserveDirective = true;
        }
        else if ( opcode == "RESB" ) {
            int value = std::stoi(operand);
            objectCodeLength = value;
            isReserveDirective = true;
        }
    } 
    else {
        // Regular instruction with opcode and operand
        objectCode = opcodeValue;
        
        if (!operand.empty()) {
            // Check if it's indexed addressing mode (operand ends with ,X)
            bool isIndexed = false;
            std::string actualOperand = operand;
            
            // Check for ,X at the end of the operand
            size_t commaPos = operand.find(",X");
            if (commaPos != std::string::npos) {
                isIndexed = true;
                actualOperand = operand.substr(0, commaPos); // Remove ,X from operand
            }
            
            if (operandAddress.empty()) {
                throw std::runtime_error("Undefined symbol: " + operand);
            }
            
            // For indexed addressing, add 8000(hex) to the address
            if (isIndexed) {
                // Convert hex string to integer
                int addr = std::stoi(operandAddress, nullptr, 16);
                // Add 8000 hex (32768 decimal)
                addr |= 0x8000;
                // Convert back to hex string
                std::stringstream ss;
                ss << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << addr;
                operandAddress = ss.str();
            }
            
            // Combine opcode and operand address
            objectCode += operandAddress;
        } 
        else {
            // No operand, pad with zeros
            objectCode += "0000";
        }
        
        objectCodeLength = objectCode.length() / 2;
    }
    
    return std::make_tuple(objectCode, objectCodeLength, isReserveDirective);
}


