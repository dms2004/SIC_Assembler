# SIC Assembler

A modular SIC assembler built in C++ that can be extended for custom architectures. This assembler provides essential features for processing assembly programs, including pass1 and pass2 operations, symbol table management, and error handling.

## Directory Structure

```
SICAssembler/
│── assembler/
│    ├── assembler.cpp                   
│    └── assembler.h
│
│── driver/
│    ├── main.cpp                  
│  
│── error/
│    ├── error.cpp
│    └── error.h
│
│── sic/
│    ├── SICasm.cpp         
│    ├── SICasm.h
│    └── opcode/
│
│── table/
│    ├── table.cpp
│    └── table.h          
│
│── bin/ 
│    ├── Assembler.o
│    ├── SICasm.o
│    ├── error.o
│    ├── main.o
│    └── table.o 
│
│── test/ 
│    ├── filecpy.asm 
│    ├── strcpy.asm 
│    ├── sic_assembler (executable) 
│
└── MakeFile
```

## Features
- **Two-pass Assembly Process**: Generates an intermediate file in Pass 1 and an object file in Pass 2.
- **Symbol Table Management**: Maintains a structured symbol table for label resolution.
- **Error Handling**: Provides mechanisms for handling assembly errors.
- **Modular Design**: Allows easy extension for custom architectures.

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/SICAssembler.git
   cd SICAssembler
   ```
2. Build the project using Makefile:
   ```bash
   make
   ```

## Usage
To assemble a SIC assembly file, run:
```bash
./test/sic_assembler test/filecpy.asm
```

## Extending the Assembler
To create an assembler for a new architecture:
1. Extend the `Assembler` class in `assembler/`.
2. Implement architecture-specific features in `sic/`.
3. Modify opcode handling in `sic/opcode/`.

