#include "vm.h"
#include <iostream>
using namespace std;

void VM::execute(Chunk chunk)
{
  int ip = 0;

  while (ip < chunk.code.size())
  {
    if (chunk.code[ip] == OP_PUSH)
    {
      ip++;
      stack.push_back(chunk.constants[chunk.code[ip]]);
      ip++;
    }
    else if (chunk.code[ip] == OP_PUSH_STRING)
    {
      ip++;
      stack.push_back(chunk.stringConstants[chunk.code[ip]]);
      ip++;
    }
    else if (chunk.code[ip] == OP_ADD)
    {
      Value right = stack.back();
      stack.pop_back();

      Value left = stack.back();
      stack.pop_back();

      if (holds_alternative<string>(left) && holds_alternative<string>(right))
      {
        string leftString = get<string>(left);
        string rightString = get<string>(right);

        stack.push_back(leftString + rightString);
      }
      else if (holds_alternative<double>(left) && holds_alternative<double>(right))
      {
        double leftNumber = get<double>(left);
        double rightNumber = get<double>(right);

        stack.push_back(leftNumber + rightNumber);
      }
      else
      {
        throw runtime_error("cannot add string and number");
      }

      ip++;
    }
    else if (chunk.code[ip] == OP_SUB)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(left - right);
      ip++;
    }
    else if (chunk.code[ip] == OP_MUL)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(left * right);
      ip++;
    }
    else if (chunk.code[ip] == OP_DIV)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(left / right);
      ip++;
    }
    else if (chunk.code[ip] == OP_LESS)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(static_cast<double>(left < right));
      ip++;
    }
    else if (chunk.code[ip] == OP_GREATER)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(static_cast<double>(left > right));
      ip++;
    }
    else if (chunk.code[ip] == OP_LESS_EQUAL)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(static_cast<double>(left <= right));
      ip++;
    }
    else if (chunk.code[ip] == OP_GREATER_EQUAL)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(static_cast<double>(left >= right));
      ip++;
    }
    else if (chunk.code[ip] == OP_EQUAL)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(static_cast<double>(left == right));
      ip++;
    }
    else if (chunk.code[ip] == OP_NOT_EQUAL)
    {
      double right = get<double>(stack.back());
      stack.pop_back();

      double left = get<double>(stack.back());
      stack.pop_back();

      stack.push_back(static_cast<double>(left != right));
      ip++;
    }
    else if(chunk.code[ip] == OP_JUMP)
    {
      ip++;
      int destination = chunk.code[ip];
      ip = destination;
    }
    else if(chunk.code[ip] == OP_JUMP_IF_FALSE)
    {
      double condition = get<double>(stack.back());
      stack.pop_back();

      ip++;

      int destination = chunk.code[ip];

      if(!condition)
      {
        ip = destination;
      }
      else
      {
        ip++;
      }
    }
    else if (chunk.code[ip] == OP_STORE)
    {
      Value value = stack.back();
      stack.pop_back();

      ip++;
      int index = chunk.code[ip];

      string name = chunk.variables[index];

      if (callStack.empty())
      {
        variables[name] = value;
      }
      else
      {
        callStack.back().variables[name] = value;
      }

      ip++;
    }
    else if (chunk.code[ip] == OP_LOAD)
    {
      ip++;
      int index = chunk.code[ip];

      string name = chunk.variables[index];

      if (callStack.empty())
      {
        stack.push_back(variables[name]);
      }
      else
      {
        stack.push_back(callStack.back().variables[name]);
      }

      ip++;
    }
    else if (chunk.code[ip] == OP_CALL)
    {
      ip++;

      int functionIndex = chunk.code[ip];

      FunctionInfo function = chunk.functions[functionIndex];

      CallFrame frame;

      frame.returnAddress = ip + 1;

      for (int i = function.parameterCount - 1; i >= 0; i--)
      {
        Value argument = stack.back();
        stack.pop_back();

        frame.variables[function.parameters[i]] = argument;
      }

      callStack.push_back(frame);

      ip = function.address;
    }
    else if (chunk.code[ip] == OP_RETURN)
    {
      Value returnValue = stack.back();
      stack.pop_back();

      int returnAddress = callStack.back().returnAddress;

      callStack.pop_back();

      stack.push_back(returnValue);

      ip = returnAddress;
    }
    else if (chunk.code[ip] == OP_PRINT)
    {
      Value value = stack.back();
      stack.pop_back();

      if (holds_alternative<double>(value))
      {
        cout << get<double>(value) << endl;
      }
      else
      {
        cout << get<string>(value) << endl;
      }

      ip++;
    }
    else if (chunk.code[ip] == OP_NEGATE)
    {
      double val = get<double>(stack.back());
      stack.pop_back();
      stack.push_back(-val);
      ip++;
    }
    else if (chunk.code[ip] == OP_HALT)
    {
      return;
    }
    else
    {
      throw runtime_error("Unknown opcode");
    }
  }
}