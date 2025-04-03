#include "table.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>      
#include <sstream>      
#include <algorithm>    

 void Table::add(const std::string& label, const std::string &field, const std::string& value) {
        tab[label][field] = value;
    }
    
    std::string Table::value(const std::string& label, const std::string &field) const {
        auto labelIt = tab.find(label);
        if (labelIt != tab.end()) {
            auto fieldIt = labelIt->second.find(field);
            if (fieldIt != labelIt->second.end()) {
                return fieldIt->second;
            }
        }
        return ""; // Return empty string if label or field not found
    }
    
    bool Table::dump(const std::string& filename) const {
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
            return false;
        }
        
        // Collect all unique field names
        std::vector<std::string> fields;
        for (const auto& row : tab) {
            for (const auto& field_pair : row.second) {
                if (std::find(fields.begin(), fields.end(), field_pair.first) == fields.end()) {
                    fields.push_back(field_pair.first);
                }
            }
        }
        
        // First line contains all field names with "label" as the first field
        outfile << "label";
        for (const auto& field : fields) {
            outfile << " " << field;
        }
        outfile << std::endl;
        
        // Write data rows
        for (const auto& row : tab) {
            outfile << row.first; // Label
            for (const auto& field : fields) {
                auto it = row.second.find(field);
                std::string cell_value = (it != row.second.end()) ? it->second : "";
                outfile << " " << cell_value;
            }
            outfile << std::endl;
        }
        
        outfile.close();
        return true;
    }
    
    bool Table::init(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << " for reading." << std::endl;
            return false;
        }
        
        // Clear existing data
        tab.clear();
        
        // Read the header line with field names
        std::string header;
        if (!std::getline(infile, header)) {
            std::cerr << "Error: File is empty or couldn't read header." << std::endl;
            return false;
        }
        
        std::istringstream iss_header(header);
        std::string field_name;
        std::vector<std::string> fields;
        
        // First field should be "label" but we'll skip it in the processing
        iss_header >> field_name; // Skip the "label" field or whatever is in the first column
        
        // Read remaining field names
        while (iss_header >> field_name) {
            fields.push_back(field_name);
        }
        
        // Read data rows
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss_line(line);
            std::string label;
            iss_line >> label; // First column is the label
            
            std::string value;
            for (size_t i = 0; i < fields.size() && iss_line >> value; ++i) {
                add(label, fields[i], value);
            }
        }
        
        infile.close();
        return true;
    }



