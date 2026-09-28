# Neos

A minimal scripting language and bytecode virtual machine built from scratch in C++.
No libraries. Just a handwritten lexer, parser, compiler, and VM.

## Status
- [x] Lexer
- [x] Parser
- [x] Bytecode compiler
- [x] Virtual machine
- [x] Variables and control flow (if/else, while loops)
- [x] Functions with return values and recursion
- [x] Strings with concatenation
- [x] Decimal numbers, negation, boolean literals
- [x] File reading (.ns files)
- [x] Descriptive error messages with line numbers
- [x] REPL — interactive prompt
- [x] Disassembler — human readable bytecode output

## How it works
Source code goes through a 4-stage pipeline:

Source → Lexer → Parser → Compiler → Bytecode → VM → Output

- **Lexer** — reads raw text and breaks it into tokens
- **Parser** — builds an Abstract Syntax Tree (AST) from tokens
- **Compiler** — walks the AST and emits bytecode instructions
- **VM** — executes the bytecode on a stack-based virtual machine

## Build & Run
```bash
g++ src/main.cpp src/lexer/lexer.cpp src/parser/parser.cpp src/compiler/compiler.cpp src/vm/vm.cpp src/disassembler/disassembler.cpp -o neos
```

Run a file:
```bash
./neos program.ns
```

Run with disassembler:
```bash
./neos program.ns --disasm
```

Start the REPL:
```bash
./neos
```

## Example
```
fn fib(n) {
    if (n < 2) {
        return n
    }
    return fib(n - 1) + fib(n - 2)
}

print fib(10)
```
Output: 55

## Disassembler Output
```
== program.ns ==
0000  OP_PUSH              constants[0] = 5
0002  OP_STORE             variables[0] = x
0004  OP_LOAD              variables[0] = x
0006  OP_PRINT
0007  OP_HALT
```

## Project Structure
```
Neos/
├── src/
│   ├── lexer/
│   │   ├── lexer.h
│   │   └── lexer.cpp
│   ├── parser/
│   │   ├── parser.h
│   │   └── parser.cpp
│   ├── compiler/
│   │   ├── compiler.h
│   │   └── compiler.cpp
│   ├── vm/
│   │   ├── vm.h
│   │   └── vm.cpp
│   ├── disassembler/
│   │   ├── disassembler.h
│   │   └── disassembler.cpp
│   └── main.cpp
├── .gitignore
└── README.md
```

## Goals
- Minimal footprint — runs on anything, no dependencies
- Clean, readable syntax
- Transparent execution via built-in disassembler
- Fast bytecode execution

## Progress
Built over the summer as a 2nd year CS student.