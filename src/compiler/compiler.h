#pragma once
#include <vector>
#include <map>
#include "../parser/parser.h"
using namespace std;

enum OpCode
{
    OP_PUSH,
    OP_PUSH_STRING,
    OP_PUSH_BOOL,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEGATE,

    OP_LESS,
    OP_GREATER,
    OP_LESS_EQUAL,
    OP_GREATER_EQUAL,
    OP_EQUAL,
    OP_NOT_EQUAL,

    OP_JUMP,
    OP_JUMP_IF_FALSE,

    OP_STORE,
    OP_LOAD,

    OP_CALL,
    OP_RETURN,

    OP_PRINT,
    OP_HALT
};

struct FunctionInfo
{
    string name;
    int address;
    int parameterCount;
    vector<string> parameters;
};

struct Chunk
{
    vector<int> code;
    vector<double> constants;
    vector<string> stringConstants;
    vector<string> variables;
    vector<FunctionInfo> functions;
};

class Compiler
{
    Chunk chunk;
    void compile(ASTNode *node);
    int getVariableIndex(const string &name);
    int getFunctionIndex(const string &name);

public:
    Chunk run(unique_ptr<ASTNode> root);
};