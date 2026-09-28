#include <iostream>
#include <fstream>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "compiler/compiler.h"
#include "vm/vm.h"
#include "disassembler/disassembler.h"

using namespace std;

// only for debugging
void printAST(ASTNode* node, int indent)
{
    NumberNode* num = dynamic_cast<NumberNode*>(node);

    if(node == nullptr)
    {
        return;
    }

    if (num != nullptr)
    {
        for (int i = 0; i < indent; i++)
        {
            cout << "  ";
        }

        cout << "NUMBER: " << num->value << endl;
        return;
    }

    BinaryOpNode* bin = dynamic_cast<BinaryOpNode*>(node);

    if (bin != nullptr)
    {
        for (int i = 0; i < indent; i++)
        {
            cout << "  ";
        }

        cout << "OP: " << bin->op << endl;

        printAST(bin->left.get(), indent + 1);
        printAST(bin->right.get(), indent + 1);
    }
}

void runRepl()
{
    VM vm;
    cout << "Neos REPL — type 'exit' to quit" << endl;

    while (true)
    {
        cout << "neos> ";
        string line;

        if (!getline(cin, line))
            break;

        if (line == "exit")
            break;

        if (line.empty())
            continue;

        try
        {
            Lexer lexer(line);
            Parser parser(lexer);
            Compiler compiler;
            Chunk chunk = compiler.run(parser.parse());
            vm.executeRepl(chunk);
        }
        catch (const exception &e)
        {
            cout << e.what() << endl;
        }
    }
}

void runFile(const string &path, bool disasm)
{
    ifstream file(path);

    if (!file.is_open())
    {
        cout << "Error: could not open file '" << path << "'" << endl;
        return;
    }

    string source(istreambuf_iterator<char>(file), {});

    try
    {
        Lexer lexer(source);
        Parser parser(lexer);
        Compiler compiler;
        VM vm;

        Chunk chunk = compiler.run(parser.parse());

        if (disasm)
        {
            Disassembler disassembler;
            disassembler.disassemble(chunk, path);
        }

        vm.execute(chunk);
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        runRepl();
        return 0;
    }

    bool disasm = false;
    if (argc >= 3 && string(argv[2]) == "--disasm")
        disasm = true;

    runFile(argv[1], disasm);
    return 0;
}