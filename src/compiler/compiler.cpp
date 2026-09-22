#include "compiler.h"
#include <iostream>
using namespace std;

int Compiler::getVariableIndex(const string &name)
{
    int index = -1;

    for (int i = 0; i < chunk.variables.size(); i++)
    {
        if (name == chunk.variables[i])
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        chunk.variables.push_back(name);
        index = chunk.variables.size() - 1;
    }

    return index;
}

int Compiler::getFunctionIndex(const string &name)
{
    for (int i = 0; i < chunk.functions.size(); i++)
    {
        if (chunk.functions[i].name == name)
        {
            return i;
        }
    }

    throw runtime_error("Unknown function: " + name);
}

void Compiler::compile(ASTNode *node)
{
    if (node == nullptr)
    {
        return;
    }

    ProgramNode *program = dynamic_cast<ProgramNode *>(node);
    NumberNode *num = dynamic_cast<NumberNode *>(node);
    StringNode *stringNode = dynamic_cast<StringNode *>(node);
    BinaryOpNode *bin = dynamic_cast<BinaryOpNode *>(node);
    VariableNode *var = dynamic_cast<VariableNode *>(node);
    AssignmentNode *assign = dynamic_cast<AssignmentNode *>(node);
    IfNode *ifNode = dynamic_cast<IfNode *>(node);
    WhileNode *whileNode = dynamic_cast<WhileNode *>(node);
    FunctionNode *functionNode = dynamic_cast<FunctionNode *>(node);
    ReturnNode *returnNode = dynamic_cast<ReturnNode *>(node);
    CallNode *callNode = dynamic_cast<CallNode *>(node);
    PrintNode *printNode = dynamic_cast<PrintNode *>(node);

    if (program != nullptr)
    {
        for (int i = 0; i < program->statements.size(); i++)
        {
            if (dynamic_cast<FunctionNode *>(program->statements[i].get()) != nullptr)
            {
                continue;
            }

            compile(program->statements[i].get());
        }

        return;
    }

    else if (num != nullptr)
    {
        chunk.constants.push_back(num->value);

        int index = chunk.constants.size() - 1;

        chunk.code.push_back(OP_PUSH);
        chunk.code.push_back(index);

        return;
    }

    else if (stringNode != nullptr)
    {
        chunk.stringConstants.push_back(stringNode->value);

        int index = chunk.stringConstants.size() - 1;

        chunk.code.push_back(OP_PUSH_STRING);
        chunk.code.push_back(index);

        return;
    }

    else if (bin != nullptr)
    {
        compile(bin->left.get());
        compile(bin->right.get());

        if (bin->op == "+")
        {
            chunk.code.push_back(OP_ADD);
        }
        else if (bin->op == "-")
        {
            chunk.code.push_back(OP_SUB);
        }
        else if (bin->op == "*")
        {
            chunk.code.push_back(OP_MUL);
        }
        else if (bin->op == "/")
        {
            chunk.code.push_back(OP_DIV);
        }
        else if (bin->op == "<")
        {
            chunk.code.push_back(OP_LESS);
        }
        else if (bin->op == ">")
        {
            chunk.code.push_back(OP_GREATER);
        }
        else if (bin->op == "<=")
        {
            chunk.code.push_back(OP_LESS_EQUAL);
        }
        else if (bin->op == ">=")
        {
            chunk.code.push_back(OP_GREATER_EQUAL);
        }
        else if (bin->op == "==")
        {
            chunk.code.push_back(OP_EQUAL);
        }
        else if (bin->op == "!=")
        {
            chunk.code.push_back(OP_NOT_EQUAL);
        }
        else
        {
            throw runtime_error("Unknown operator: " + bin->op);
        }

        return;
    }

    else if (var != nullptr)
    {
        int index = getVariableIndex(var->variableName);

        chunk.code.push_back(OP_LOAD);
        chunk.code.push_back(index);

        return;
    }

    else if (assign != nullptr)
    {
        compile(assign->right.get());

        VariableNode *variable = dynamic_cast<VariableNode *>(assign->left.get());

        int index = getVariableIndex(variable->variableName);

        chunk.code.push_back(OP_STORE);
        chunk.code.push_back(index);

        return;
    }

    else if (ifNode != nullptr)
    {
        compile(ifNode->condition.get());

        chunk.code.push_back(OP_JUMP_IF_FALSE);

        int jumpIfFalsePosition = chunk.code.size();

        chunk.code.push_back(0);

        compile(ifNode->body.get());

        if (ifNode->elseBranch != nullptr)
        {
            chunk.code.push_back(OP_JUMP);

            int jumpPosition = chunk.code.size();

            chunk.code.push_back(0);

            chunk.code[jumpIfFalsePosition] = chunk.code.size();

            compile(ifNode->elseBranch.get());

            chunk.code[jumpPosition] = chunk.code.size();
        }
        else
        {
            chunk.code[jumpIfFalsePosition] = chunk.code.size();
        }

        return;
    }

    else if (whileNode != nullptr)
    {
        int loopStart = chunk.code.size();

        compile(whileNode->condition.get());

        chunk.code.push_back(OP_JUMP_IF_FALSE);

        int jumpIfFalsePosition = chunk.code.size();

        chunk.code.push_back(0);

        compile(whileNode->body.get());

        chunk.code.push_back(OP_JUMP);
        chunk.code.push_back(loopStart);

        chunk.code[jumpIfFalsePosition] = chunk.code.size();

        return;
    }

    else if (functionNode != nullptr)
    {
        int functionIndex = getFunctionIndex(functionNode->functionName);

        chunk.functions[functionIndex].address = chunk.code.size();

        for (int i = 0; i < functionNode->parameters.size(); i++)
        {
            getVariableIndex(functionNode->parameters[i]);
        }

        compile(functionNode->body.get());

        chunk.code.push_back(OP_PUSH);
        chunk.code.push_back(0);
        chunk.code.push_back(OP_RETURN);

        return;
    }

    else if (returnNode != nullptr)
    {
        compile(returnNode->expression.get());
        chunk.code.push_back(OP_RETURN);

        return;
    }

    else if (callNode != nullptr)
    {
        for (int i = 0; i < callNode->arguments.size(); i++)
        {
            compile(callNode->arguments[i].get());
        }

        int functionIndex = getFunctionIndex(callNode->functionName);

        chunk.code.push_back(OP_CALL);
        chunk.code.push_back(functionIndex);

        return;
    }

    else if (printNode != nullptr)
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
        for (int i = 0; i < program->statements.size(); i++)
        {
            FunctionNode *functionNode = dynamic_cast<FunctionNode *>(program->statements[i].get());

            if (functionNode != nullptr)
            {
                FunctionInfo function;

                function.name = functionNode->functionName;
                function.address = -1;
                function.parameterCount = functionNode->parameters.size();
                function.parameters = functionNode->parameters;

                chunk.functions.push_back(function);
            }
        }
    }

    int jumpOverFunctions = -1;

    if (chunk.functions.size() > 0)
    {
        chunk.code.push_back(OP_JUMP);
        jumpOverFunctions = chunk.code.size();
        chunk.code.push_back(0);

        for (int i = 0; i < program->statements.size(); i++)
        {
            FunctionNode *functionNode = dynamic_cast<FunctionNode *>(program->statements[i].get());

            if (functionNode != nullptr)
            {
                compile(functionNode);
            }
        }

        chunk.code[jumpOverFunctions] = chunk.code.size();
    }

    compile(root.get());

    chunk.code.push_back(OP_HALT);

    return chunk;
}