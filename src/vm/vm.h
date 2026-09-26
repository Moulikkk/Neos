#pragma once
#include <vector>
#include <map>
#include <variant>
#include <string>
#include "../compiler/compiler.h"
using namespace std;

using Value = std::variant<double, std::string>;

struct CallFrame
{
    int returnAddress;
    map<string, Value> variables;
};

class VM
{
    vector<Value> stack;
    map<string, Value> variables;
    vector<CallFrame> callStack;

public:
    void execute(Chunk chunk);
    void executeRepl(Chunk chunk);
};