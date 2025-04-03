#include <string>       
#include <fstream>      
#include <iostream>     
#include <sstream>      
#include <iomanip>      
#include <algorithm>    
#include <exception>    
#include "Assembler.h"
#include "error.h"
#include "table.h"


bool Assembler::pass1(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) 
        return error("Could not open file " + filename);

    // Create intermediate file
    size_t dotPos = filename.find('.'); // Find first occurrence of '.'
    std::string baseName = (dotPos == std::string::npos) ? filename : filename.substr(0, dotPos);
    std::string intermediateFilename = baseName + ".intermediate";
    std::ofstream intermediateFile(intermediateFilename);
    if (!intermediateFile.is_open()) 
        return error("Could not create intermediate file " + intermediateFilename);
    
    std::string line;
    int start_loc = 0, max_block_num = 0;
    start_address = "0";
    program_name = "";
    
    int current_block_num = 0; // Current block (default is 0)
    int instruction_size, size_pg  = std::stoi(start_address, nullptr, 16);
    
    // Add default block (block 0) to block table
    block_table.add("0", "name", "DEFAULT");
    block_table.add("0", "start_address", "0");
    block_table.add("0", "length", "0");
    
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Trim leading whitespace
        size_t first_non_space = line.find_first_not_of(" \t");
        if (first_non_space == std::string::npos) {
            continue;  // Line is all whitespace
        }
        line = line.substr(first_non_space);

        // Original line for intermediate file
        std::string originalLine = line;
        std::string symbol = "";

        // Check if there's a symbol in the line (ending with ':')
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            // Extract symbol (and trim any whitespace)
            symbol = line.substr(0, colon_pos);
            symbol.erase(symbol.find_last_not_of(" \t") + 1);  // Remove trailing spaces
            
            // Convert current block's location counter to a 4-digit hex string
            std::stringstream hexLoc;
            hexLoc << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << loc_counter[current_block_num];
            
            // Add symbol to symbol table with current location counter and block number
            symtab.add(symbol, "value", hexLoc.str());
            symtab.add(symbol, "block", std::to_string(current_block_num));
            
            // Remove the symbol part from the line for further processing
            line = line.substr(colon_pos + 1);
            // Trim leading whitespace again
            first_non_space = line.find_first_not_of(" \t");
            if (first_non_space == std::string::npos) {
                continue;  // No operation after symbol
            }
            line = line.substr(first_non_space);
        }

        // At this point, line contains operation (and possibly operands)
        // Parse operation and update location counter
        std::string operation;
        std::string operand = "";
        std::istringstream iss(line);
        iss >> operation;
        
        if (operation.empty()) {
            continue;  // No operation found
        }

        // Get the operand if present
        std::getline(iss >> std::ws, operand);
        
        int current_loc = loc_counter[current_block_num];  // Store current location for intermediate file
        
        // Check for START directive
        if (operation == "START") {
            // Just copy the original line to the intermediate file
            if (!operand.empty()) {
                start_address = operand;
                start_loc = std::stoi(operand, nullptr, 16); // Convert hex string to int
                loc_counter[current_block_num] = start_loc; // Initialize default block's loc counter
            }
            if (!operand.empty() && !symbol.empty()){
                program_name = symbol;
            }
                
            intermediateFile << originalLine << std::endl;
            continue;  // Skip to next line
        }
        
        // Handle USE directive for block management
        if (operation == "USE") {
            // If operand is empty, switch to default block
            if (operand.empty() || operand == "DEFAULT") {
                current_block_num = 0;
            } else {
                // Check if this block already exists in the block table
                bool block_exists = false;
                for (int i = 0; i <= max_block_num; i++) {
                    std::string block_name;
                    try {
                        block_name = block_table.value(std::to_string(i), "name");
                        if (block_name == operand) {
                            current_block_num = i;
                            block_exists = true;
                            break;
                        }
                    } catch (...) {
                        // Block doesn't exist, continue checking
                    }
                }
                
                // If block doesn't exist, create a new one
                if (!block_exists) {
                    int new_block_num = max_block_num+1;
                    current_block_num = new_block_num;
                    block_table.add(std::to_string(new_block_num), "name", operand);
                    block_table.add(std::to_string(new_block_num), "start_address", "0"); // Temporary, will update later
                    block_table.add(std::to_string(new_block_num), "length", "0"); // Temporary, will update later
                    
                    // Initialize the new block's location counter to the same as START
                    loc_counter[new_block_num] = start_loc;
                    current_block_num = new_block_num;
                    max_block_num = new_block_num;
                }
            }
            
            // Write USE directive to intermediate file 
            intermediateFile << "XXXX" << " " << operation << " " << operand << std::endl;
            continue;  // Skip to next line
        }
        
        if (operation == "END") {
            // Calculate program length as sum of all block lengths
            int total_length = 0;
            int next_block_addr = start_loc;
            
            // First, calculate the length of each block
            for (int i = 0; i <= max_block_num; i++) {
                int block_length = 0;
                
                // If block has instructions, calculate its length
                if (loc_counter.find(i) != loc_counter.end()) {
                    block_length = loc_counter[i] - start_loc ;
                }
                
                // Convert length to hex string
                std::stringstream hexLength;
                hexLength << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << block_length;
                block_table.add(std::to_string(i), "length", hexLength.str());
                
                // Set start addresses for each block
                std::stringstream hexStart;
                hexStart << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << next_block_addr;
                block_table.add(std::to_string(i), "start_address", hexStart.str());
                
                // Next block starts after this one
                next_block_addr += block_length;
                
                // Add to total program length
                total_length += block_length;
            }
    
            std::stringstream newstream;
            newstream << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << loc_counter[current_block_num];
            std::string temp = newstream.str();
            intermediateFile << temp << " " << operation << " " << operand << std::endl;
            break; // Stop processing at END directive
        }
        // Update location counter based on operation
        try {
            // Call the address translation function to get the size of the instruction
            instruction_size = addressTranslation(operation, line);
            loc_counter[current_block_num] += instruction_size;
            
            // Format: <address> <opcode> <operand> <block_num>
            intermediateFile << std::hex << std::setw(4) << std::setfill('0') << current_loc << " "
                              << operation << " " << operand << std::endl;
        } catch (const std::exception& e){
            return error(e.what());  
        }
        size_pg += instruction_size;
    }
    std::stringstream hexStream;
    hexStream << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << size_pg;
    program_length = hexStream.str();
    
    file.close();
    intermediateFile.close();
    return true;
}

bool Assembler::pass2(const std::string& filename) {
    // Open the intermediate file
    size_t dotPos = filename.find('.'); // Find first occurrence of '.'
    std::string baseName = (dotPos == std::string::npos) ? filename : filename.substr(0, dotPos);
    std::string intermediateFilename = baseName + ".intermediate";
    std::ifstream intermediateFile(intermediateFilename);
    if (!intermediateFile.is_open()) {
        return error("Could not open intermediate file " + intermediateFilename);
    }

    // Create the object file
    
    std::string objectFilename = baseName + ".obj";
    std::ofstream objectFile(objectFilename);
    if (!objectFile.is_open()) {
        return error("Could not create object file " + objectFilename);
    }

    // Ensure start_address is 6 characters
    std::string paddedStartAddress = start_address;
    while (paddedStartAddress.length() < 6) {
        paddedStartAddress = "0" + paddedStartAddress;
    }

    // Ensure program_length is 6 characters
    std::string paddedProgramLength = program_length;
    while (paddedProgramLength.length() < 6) {
        paddedProgramLength = "0" + paddedProgramLength;
    }

    // Write header record - ensure program_name is exactly 6 characters
    std::string paddedProgramName = program_name;
    if (paddedProgramName.empty()) {
        paddedProgramName = "      "; // 6 spaces if empty
    } else {
        // Pad or truncate to exactly 6 characters
        paddedProgramName = std::string(paddedProgramName, 0, 6);
        if (paddedProgramName.length() < 6) {
            paddedProgramName.append(6 - paddedProgramName.length(), ' ');
        }
    }
    
    objectFile << "H " << paddedProgramName << " " 
               << paddedStartAddress << " " << paddedProgramLength << std::endl;

    // Variables for text record
    std::string currentTextRecord = "";
    std::string block_number;
    std::string textStartAddress = "";
    int textRecordLength = 0;
    int track_length = std::stoi(start_address, nullptr, 16);
    const int MAX_TEXT_RECORD_LENGTH = 30; // Maximum bytes per text record (60 hex chars)
    bool firstObjectCodeInRecord = true; // Track if this is the first object code in the current record

    std::string line;
    // Process intermediate file to generate text records
    while (std::getline(intermediateFile, line)) {
        std::istringstream iss(line);
        std::string address, opcode, operand;
        
        // Skip lines without proper formatting
        if (line.empty() || !isxdigit(line[0])) {
            continue;
        }
        
        iss >> address >> opcode >> operand;
        
        // Skip START and END directives for object code generation
        if (opcode == "START" || opcode == "END" || opcode == "USE") {
            continue;
        }
        
        // Fetch opcode from optab
        std::string opcodeValue = optab.value(opcode, "value");
        std::string temp_operand = operand, actualOperand;
        size_t commaPos = temp_operand.find(",X");
        if (commaPos != std::string::npos) {
              actualOperand = temp_operand.substr(0, commaPos); // Remove ,X from operand
        }
        else
              actualOperand = operand;
        
        // Fetch symbol from symtab if exists
        std::string operandAddress = symtab.value(actualOperand, "value");
        block_number = symtab.value(actualOperand, "block");
        if(operandAddress != ""){
            std::string block_start = block_table.value(block_number, "start_address");
            
            unsigned long addr1 = std::stoul(operandAddress, nullptr, 16);
            unsigned long addr2 = std::stoul(block_start, nullptr, 16);
            unsigned long addr3 = std::stoul(start_address, nullptr, 16);

            // Perform addition
            unsigned long result = addr1 + (addr2 - addr3);

            // Convert result back to hex string
            std::stringstream ss;
            ss << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << result;
            
            operandAddress = ss.str();
        }
        
        // Generate object code for this instruction or directive
        std::string objectCode;
        int objectCodeLength = 0;
        bool isReserveDirective = false;

        try {
            std::tie(objectCode, objectCodeLength, isReserveDirective) = generateObjectCode(opcode, operand, opcodeValue, operandAddress);
        } catch (const std::exception& e) {
            return error(e.what());
        }
 
        // Handle reserve directives by ending current text record
        if (isReserveDirective) {
            if (!currentTextRecord.empty()) {
                objectFile << "T " << textStartAddress << " " 
                          << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                          << textRecordLength << " " << currentTextRecord << std::endl;
                currentTextRecord = "";
                textRecordLength = 0;
                firstObjectCodeInRecord = true;
            }
            track_length += objectCodeLength;
            continue;
        }

        // Check if new object code would exceed text record length
        if (!objectCode.empty() && textRecordLength + objectCodeLength > MAX_TEXT_RECORD_LENGTH) {
            // Write current text record
            if (!currentTextRecord.empty()) {
                objectFile << "T " << textStartAddress << " " 
                          << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                          << textRecordLength << " " << currentTextRecord << std::endl;
                currentTextRecord = "";
                textRecordLength = 0;
                firstObjectCodeInRecord = true;
            }
        }
        
        // Start new text record if needed
        if (!objectCode.empty() && currentTextRecord.empty()) {
            std::stringstream ss;
            ss << std::hex << std::uppercase << std::setfill('0') << std::setw(6) << track_length;
            
            textStartAddress = ss.str();
            while (textStartAddress.length() < 6) {
                textStartAddress = "0" + textStartAddress;
            }
            std::transform(textStartAddress.begin(), textStartAddress.end(), textStartAddress.begin(), ::toupper);
        }
        
        // Add the object code to the current text record
        if (!objectCode.empty()) {
            // Add space before object code if not the first in record
            if (!firstObjectCodeInRecord) {
                currentTextRecord += " ";
            }
            
            currentTextRecord += objectCode;
            textRecordLength += objectCodeLength;
            firstObjectCodeInRecord = false;
        }
        track_length += objectCodeLength;
    }
    
    // Write final text record if not empty
    if (!currentTextRecord.empty()) {
        objectFile << "T " << textStartAddress << " " 
                  << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                  << textRecordLength << " " << currentTextRecord << std::endl;
    }
    
    // Write end record with start address
    objectFile << "E " << paddedStartAddress << std::endl;
    
    intermediateFile.close();
    objectFile.close();
    return true;
}

void Assembler::assemble(const std::string& filename) {
    pass1(filename);
    size_t dotPos = filename.find('.'); // Find first occurrence of '.'
    std::string baseName = (dotPos == std::string::npos) ? filename : filename.substr(0, dotPos);
    symtab.dump(baseName + ".symbol.dump");
    block_table.dump(baseName + ".block.dump");
    pass2(filename);
}


