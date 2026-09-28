#pragma once
#include "../compiler/compiler.h"
#include <string>

class Disassembler
{
public:
    void disassemble(Chunk &chunk, const std::string &title);
};