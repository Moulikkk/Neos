#include "disassembler.h"
#include <iostream>
#include <iomanip>
using namespace std;

static string opcodeToString(int opcode)
{
    switch (opcode)
    {
        case OP_PUSH:            return "OP_PUSH";
        case OP_PUSH_STRING:     return "OP_PUSH_STRING";
        case OP_PUSH_BOOL:       return "OP_PUSH_BOOL";
        case OP_ADD:             return "OP_ADD";
        case OP_SUB:             return "OP_SUB";
        case OP_MUL:             return "OP_MUL";
        case OP_DIV:             return "OP_DIV";
        case OP_NEGATE:          return "OP_NEGATE";
        case OP_LESS:            return "OP_LESS";
        case OP_GREATER:         return "OP_GREATER";
        case OP_LESS_EQUAL:      return "OP_LESS_EQUAL";
        case OP_GREATER_EQUAL:   return "OP_GREATER_EQUAL";
        case OP_EQUAL:           return "OP_EQUAL";
        case OP_NOT_EQUAL:       return "OP_NOT_EQUAL";
        case OP_JUMP:            return "OP_JUMP";
        case OP_JUMP_IF_FALSE:   return "OP_JUMP_IF_FALSE";
        case OP_STORE:           return "OP_STORE";
        case OP_LOAD:            return "OP_LOAD";
        case OP_CALL:            return "OP_CALL";
        case OP_RETURN:          return "OP_RETURN";
        case OP_PRINT:           return "OP_PRINT";
        case OP_HALT:            return "OP_HALT";
        default:                 return "OP_UNKNOWN";
    }
}

void Disassembler::disassemble(Chunk &chunk, const std::string &title)
{
    cout << "== " << title << " ==" << endl;

    int i = 0;
    while (i < (int)chunk.code.size())
    {
        int opcode = chunk.code[i];

        cout << setw(4) << setfill('0') << right << i << "  ";
        cout << left << setw(20) << setfill(' ') << opcodeToString(opcode);

        if (opcode == OP_PUSH)
        {
            int index = chunk.code[i + 1];
            cout << "constants[" << index << "] = " << chunk.constants[index];
            i += 2;
        }
        else if (opcode == OP_PUSH_STRING)
        {
            int index = chunk.code[i + 1];
            cout << "stringConstants[" << index << "] = \"" << chunk.stringConstants[index] << "\"";
            i += 2;
        }
        else if (opcode == OP_PUSH_BOOL)
        {
            int index = chunk.code[i + 1];
            cout << "constants[" << index << "] = " << (chunk.constants[index] ? "true" : "false");
            i += 2;
        }
        else if (opcode == OP_STORE || opcode == OP_LOAD)
        {
            int index = chunk.code[i + 1];
            cout << "variables[" << index << "] = " << chunk.variables[index];
            i += 2;
        }
        else if (opcode == OP_JUMP || opcode == OP_JUMP_IF_FALSE)
        {
            int destination = chunk.code[i + 1];
            cout << "-> " << destination;
            i += 2;
        }
        else if (opcode == OP_CALL)
        {
            int index = chunk.code[i + 1];
            cout << "function[" << index << "] = " << chunk.functions[index].name;
            i += 2;
        }
        else
        {
            i++;
        }

        cout << endl;
    }

    cout << endl;
}