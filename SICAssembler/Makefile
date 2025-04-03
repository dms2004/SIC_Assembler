# Makefile for SIC Assembler

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++23
# Add include paths for all directories containing header files
INCLUDES = -I$(ASSEMBLER_DIR) -I$(DRIVER_DIR) -I$(ERROR_DIR) -I$(SIC_DIR) -I$(TABLE_DIR)

# Directories
ASSEMBLER_DIR = assembler
DRIVER_DIR = driver
ERROR_DIR = error
SIC_DIR = sic
TABLE_DIR = table
TEST_DIR = bin

# Source files
ASSEMBLER_SRC = $(ASSEMBLER_DIR)/Assembler.cpp
DRIVER_SRC = $(DRIVER_DIR)/main.cpp
ERROR_SRC = $(ERROR_DIR)/error.cpp
SIC_SRC = $(SIC_DIR)/SICasm.cpp
TABLE_SRC = $(TABLE_DIR)/table.cpp

# Object files
ASSEMBLER_OBJ = $(TEST_DIR)/Assembler.o
DRIVER_OBJ = $(TEST_DIR)/main.o
ERROR_OBJ = $(TEST_DIR)/error.o
SIC_OBJ = $(TEST_DIR)/SICasm.o
TABLE_OBJ = $(TEST_DIR)/table.o

# All object files
OBJ_FILES = $(ASSEMBLER_OBJ) $(DRIVER_OBJ) $(ERROR_OBJ) $(SIC_OBJ) $(TABLE_OBJ)

# Executable
EXECUTABLE = sic_assembler

# Main target
all: $(TEST_DIR) $(EXECUTABLE)

# Create test directory if it doesn't exist
$(TEST_DIR):
	mkdir -p $(TEST_DIR)

# Linking all object files to create the executable
$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compiling assembler source files
$(ASSEMBLER_OBJ): $(ASSEMBLER_SRC) $(ASSEMBLER_DIR)/Assembler.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compiling driver source files
$(DRIVER_OBJ): $(DRIVER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compiling error source files
$(ERROR_OBJ): $(ERROR_SRC) $(ERROR_DIR)/error.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compiling SIC source files
$(SIC_OBJ): $(SIC_SRC) $(SIC_DIR)/SICasm.h $(SIC_DIR)/opcode
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compiling table source files
$(TABLE_OBJ): $(TABLE_SRC) $(TABLE_DIR)/table.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ_FILES) $(EXECUTABLE)

# Phony targets
.PHONY: all clean
