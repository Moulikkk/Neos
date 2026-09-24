#pragma once
#include "../lexer/lexer.h"
#include <memory>
#include <vector>

struct ASTNode
{
    virtual ~ASTNode() = default;
};

struct NumberNode : ASTNode
{
    double value;
    NumberNode(double number) { value = number; }
};

struct StringNode : ASTNode
{
    std::string value;
    StringNode(std::string s) { value = s; }
};

struct BoolNode : ASTNode
{
    bool value;
    BoolNode(bool b) { value = b; }
};

struct NegateNode : ASTNode
{
    std::unique_ptr<ASTNode> operand;
    NegateNode(std::unique_ptr<ASTNode> o) : operand(std::move(o)) {}
};

struct BinaryOpNode : ASTNode
{
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryOpNode(std::string s, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(s), left(std::move(l)), right(std::move(r)) {}
};

struct VariableNode : ASTNode
{
    std::string variableName;
    VariableNode(std::string s) : variableName(s) {}
};

struct AssignmentNode : ASTNode
{
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    AssignmentNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : left(std::move(l)), right(std::move(r)) {}
};

struct ProgramNode : ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> statements;
};

struct IfNode : ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ProgramNode> body;
    std::unique_ptr<ASTNode> elseBranch;
    IfNode(std::unique_ptr<ASTNode> c, std::unique_ptr<ProgramNode> b, std::unique_ptr<ASTNode> e = nullptr)
        : condition(std::move(c)), body(std::move(b)), elseBranch(std::move(e)) {}
};

struct WhileNode : ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ProgramNode> body;
    WhileNode(std::unique_ptr<ASTNode> c, std::unique_ptr<ProgramNode> b)
        : condition(std::move(c)), body(std::move(b)) {}
};

struct FunctionNode : ASTNode
{
    std::string functionName;
    std::vector<std::string> parameters;
    std::unique_ptr<ProgramNode> body;
    FunctionNode(std::string name, std::vector<std::string> params, std::unique_ptr<ProgramNode> b)
        : functionName(name), parameters(std::move(params)), body(std::move(b)) {}
};

struct ReturnNode : ASTNode
{
    std::unique_ptr<ASTNode> expression;
    ReturnNode(std::unique_ptr<ASTNode> e) : expression(std::move(e)) {}
};

struct CallNode : ASTNode
{
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    CallNode(std::string name, std::vector<std::unique_ptr<ASTNode>> args)
        : functionName(name), arguments(std::move(args)) {}
};

struct PrintNode : ASTNode
{
    std::unique_ptr<ASTNode> expression;
    PrintNode(std::unique_ptr<ASTNode> e) : expression(std::move(e)) {}
};

class Parser
{
    Lexer lexer;
    Token curr_Token;
    Token peek_Token;

public:
    Parser(Lexer &l);
    void advance();
    void consume(TokenType expectedType);
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parseFunction();
    std::unique_ptr<ASTNode> parseReturn();
    std::unique_ptr<ASTNode> parseCall();
    std::unique_ptr<ASTNode> parsePrint();
    std::unique_ptr<ASTNode> parse();
};