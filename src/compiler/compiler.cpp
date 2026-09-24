#include "compiler.h"
#include <iostream>
using namespace std;

int Compiler::getVariableIndex(const string &name)
{
    for (int i = 0; i < chunk.variables.size(); i++)
        if (name == chunk.variables[i]) return i;
    chunk.variables.push_back(name);
    return chunk.variables.size() - 1;
}

int Compiler::getFunctionIndex(const string &name)
{
    for (int i = 0; i < chunk.functions.size(); i++)
        if (chunk.functions[i].name == name) return i;
    throw runtime_error("Unknown function: " + name);
}

void Compiler::compile(ASTNode *node)
{
    if (node == nullptr) return;

    if (auto *program = dynamic_cast<ProgramNode *>(node))
    {
        for (auto &s : program->statements)
            if (!dynamic_cast<FunctionNode *>(s.get()))
                compile(s.get());
        return;
    }

    if (auto *num = dynamic_cast<NumberNode *>(node))
    {
        chunk.constants.push_back(num->value);
        chunk.code.push_back(OP_PUSH);
        chunk.code.push_back(chunk.constants.size() - 1);
        return;
    }

    if (auto *b = dynamic_cast<BoolNode *>(node))
    {
        chunk.constants.push_back(b->value ? 1.0 : 0.0);
        chunk.code.push_back(OP_PUSH);
        chunk.code.push_back(chunk.constants.size() - 1);
        return;
    }

    if (auto *s = dynamic_cast<StringNode *>(node))
    {
        chunk.stringConstants.push_back(s->value);
        chunk.code.push_back(OP_PUSH_STRING);
        chunk.code.push_back(chunk.stringConstants.size() - 1);
        return;
    }

    if (auto *neg = dynamic_cast<NegateNode *>(node))
    {
        compile(neg->operand.get());
        chunk.code.push_back(OP_NEGATE);
        return;
    }

    if (auto *bin = dynamic_cast<BinaryOpNode *>(node))
    {
        compile(bin->left.get());
        compile(bin->right.get());
        if      (bin->op == "+")  chunk.code.push_back(OP_ADD);
        else if (bin->op == "-")  chunk.code.push_back(OP_SUB);
        else if (bin->op == "*")  chunk.code.push_back(OP_MUL);
        else if (bin->op == "/")  chunk.code.push_back(OP_DIV);
        else if (bin->op == "<")  chunk.code.push_back(OP_LESS);
        else if (bin->op == ">")  chunk.code.push_back(OP_GREATER);
        else if (bin->op == "<=") chunk.code.push_back(OP_LESS_EQUAL);
        else if (bin->op == ">=") chunk.code.push_back(OP_GREATER_EQUAL);
        else if (bin->op == "==") chunk.code.push_back(OP_EQUAL);
        else if (bin->op == "!=") chunk.code.push_back(OP_NOT_EQUAL);
        else throw runtime_error("Unknown operator: " + bin->op);
        return;
    }

    if (auto *var = dynamic_cast<VariableNode *>(node))
    {
        chunk.code.push_back(OP_LOAD);
        chunk.code.push_back(getVariableIndex(var->variableName));
        return;
    }

    if (auto *assign = dynamic_cast<AssignmentNode *>(node))
    {
        compile(assign->right.get());
        auto *variable = dynamic_cast<VariableNode *>(assign->left.get());
        chunk.code.push_back(OP_STORE);
        chunk.code.push_back(getVariableIndex(variable->variableName));
        return;
    }

    if (auto *ifNode = dynamic_cast<IfNode *>(node))
    {
        compile(ifNode->condition.get());
        chunk.code.push_back(OP_JUMP_IF_FALSE);
        int jumpIfFalsePos = chunk.code.size();
        chunk.code.push_back(0);
        compile(ifNode->body.get());
        if (ifNode->elseBranch != nullptr)
        {
            chunk.code.push_back(OP_JUMP);
            int jumpPos = chunk.code.size();
            chunk.code.push_back(0);
            chunk.code[jumpIfFalsePos] = chunk.code.size();
            compile(ifNode->elseBranch.get());
            chunk.code[jumpPos] = chunk.code.size();
        }
        else
        {
            chunk.code[jumpIfFalsePos] = chunk.code.size();
        }
        return;
    }

    if (auto *whileNode = dynamic_cast<WhileNode *>(node))
    {
        int loopStart = chunk.code.size();
        compile(whileNode->condition.get());
        chunk.code.push_back(OP_JUMP_IF_FALSE);
        int jumpIfFalsePos = chunk.code.size();
        chunk.code.push_back(0);
        compile(whileNode->body.get());
        chunk.code.push_back(OP_JUMP);
        chunk.code.push_back(loopStart);
        chunk.code[jumpIfFalsePos] = chunk.code.size();
        return;
    }

    if (auto *functionNode = dynamic_cast<FunctionNode *>(node))
    {
        int functionIndex = getFunctionIndex(functionNode->functionName);
        chunk.functions[functionIndex].address = chunk.code.size();
        for (auto &p : functionNode->parameters) getVariableIndex(p);
        compile(functionNode->body.get());
        chunk.constants.push_back(0.0);
        chunk.code.push_back(OP_PUSH);
        chunk.code.push_back(chunk.constants.size() - 1);
        chunk.code.push_back(OP_RETURN);
        return;
    }

    if (auto *returnNode = dynamic_cast<ReturnNode *>(node))
    {
        compile(returnNode->expression.get());
        chunk.code.push_back(OP_RETURN);
        return;
    }

    if (auto *callNode = dynamic_cast<CallNode *>(node))
    {
        for (auto &arg : callNode->arguments) compile(arg.get());
        chunk.code.push_back(OP_CALL);
        chunk.code.push_back(getFunctionIndex(callNode->functionName));
        return;
    }

    if (auto *printNode = dynamic_cast<PrintNode *>(node))
    {
        compile(printNode->expression.get());
        chunk.code.push_back(OP_PRINT);
        return;
    }

    throw runtime_error("Unknown AST node");
}

Chunk Compiler::run(unique_ptr<ASTNode> root)
{
    chunk = Chunk();
    ProgramNode *program = dynamic_cast<ProgramNode *>(root.get());

    if (program != nullptr)
    {
        for (auto &s : program->statements)
        {
            if (auto *fn = dynamic_cast<FunctionNode *>(s.get()))
            {
                FunctionInfo info;
                info.name = fn->functionName;
                info.address = -1;
                info.parameterCount = fn->parameters.size();
                info.parameters = fn->parameters;
                chunk.functions.push_back(info);
            }
        }
    }

    if (!chunk.functions.empty())
    {
        chunk.code.push_back(OP_JUMP);
        int jumpPos = chunk.code.size();
        chunk.code.push_back(0);
        for (auto &s : program->statements)
            if (dynamic_cast<FunctionNode *>(s.get()))
                compile(s.get());
        chunk.code[jumpPos] = chunk.code.size();
    }

    compile(root.get());
    chunk.code.push_back(OP_HALT);
    return chunk;
}