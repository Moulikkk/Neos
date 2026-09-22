#include <stdexcept>
#include <string>
#include "parser.h"
using namespace std;

std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
        case NUMBER: return "number";
        case STRING: return "string";
        case PLUS: return "+";
        case MINUS: return "-";
        case STAR: return "*";
        case SLASH: return "/";
        case LPAREN: return "(";
        case RPAREN: return ")";
        case LBRACE: return "{";
        case RBRACE: return "}";
        case EQUAL: return "=";
        case EQUAL_EQUAL: return "==";
        case BANG_EQUAL: return "!=";
        case LESS: return "<";
        case GREATER: return ">";
        case LESS_EQUAL: return "<=";
        case GREATER_EQUAL: return ">=";
        case IDENTIFIER: return "identifier";
        case IF: return "if";
        case ELSE: return "else";
        case WHILE: return "while";
        case FN: return "fn";
        case RETURN: return "return";
        case PRINT: return "print";
        case COMMA: return ",";
        case NEWLINE: return "newline";
        case END: return "end of file";
        default: return "unknown";
    }
}

Parser::Parser(Lexer &l) : lexer(l)
{
    curr_Token = lexer.nextToken();
    peek_Token = lexer.nextToken();
}

void Parser::advance()
{
    curr_Token = peek_Token;
    peek_Token = lexer.nextToken();
}

void Parser::consume(TokenType expectedType)
{
    if (curr_Token.type != expectedType)
    {
        throw std::runtime_error(
            "[Neos Error] Line " + std::to_string(curr_Token.line) +
            ": expected '" + tokenTypeToString(expectedType) +
            "' but got '" + tokenTypeToString(curr_Token.type) + "'"
        );
    }

    advance();
}

std::unique_ptr<ASTNode> Parser::parseFactor()
{
    if (curr_Token.type == NUMBER)
    {
        int value = std::stod(curr_Token.value);
        consume(NUMBER);

        return std::make_unique<NumberNode>(value);
    }

    if (curr_Token.type == STRING)
    {
        std::string value = curr_Token.value;
        consume(STRING);

        return std::make_unique<StringNode>(value);
    }

    if (curr_Token.type == LPAREN)
    {
        consume(LPAREN);

        unique_ptr<ASTNode> result = parseComparison();

        consume(RPAREN);

        return result;
    }

    if (curr_Token.type == TokenType::IDENTIFIER)
    {
        if (peek_Token.type == TokenType::LPAREN)
        {
            return parseCall();
        }

        std::string name = curr_Token.value;
        advance();

        return std::make_unique<VariableNode>(name);
    }

    std::string tokenValue = curr_Token.value.empty() ? tokenTypeToString(curr_Token.type) : curr_Token.value;

    throw std::runtime_error(
        "[Neos Error] Line " + std::to_string(curr_Token.line) +
        ": unexpected token '" + tokenValue + "'"
    );
}

std::unique_ptr<ASTNode> Parser::parseTerm()
{
    unique_ptr<ASTNode> result = parseFactor();

    while (curr_Token.type == STAR || curr_Token.type == SLASH)
    {
        TokenType op = curr_Token.type;

        if (op == STAR)
        {
            consume(STAR);
            result = make_unique<BinaryOpNode>("*", move(result), parseFactor());
        }
        else
        {
            consume(SLASH);
            result = make_unique<BinaryOpNode>("/", move(result), parseFactor());
        }
    }

    return result;
}

std::unique_ptr<ASTNode> Parser::parseExpression()
{
    unique_ptr<ASTNode> result = parseTerm();

    while (curr_Token.type == PLUS || curr_Token.type == MINUS)
    {
        TokenType op = curr_Token.type;

        if (op == PLUS)
        {
            consume(PLUS);
            result = make_unique<BinaryOpNode>("+", move(result), parseTerm());
        }
        else
        {
            consume(MINUS);
            result = make_unique<BinaryOpNode>("-", move(result), parseTerm());
        }
    }

    return result;
}

std::unique_ptr<ASTNode> Parser::parseComparison()
{
    std::unique_ptr<ASTNode> result = parseExpression();

    while (curr_Token.type == LESS || curr_Token.type == GREATER || curr_Token.type == LESS_EQUAL || curr_Token.type == GREATER_EQUAL || curr_Token.type == EQUAL_EQUAL || curr_Token.type == BANG_EQUAL)
    {
        TokenType op = curr_Token.type;

        if (op == LESS)
        {
            consume(LESS);
            result = std::make_unique<BinaryOpNode>("<", std::move(result), parseExpression());
        }
        else if (op == GREATER)
        {
            consume(GREATER);
            result = std::make_unique<BinaryOpNode>(">", std::move(result), parseExpression());
        }
        else if (op == LESS_EQUAL)
        {
            consume(LESS_EQUAL);
            result = std::make_unique<BinaryOpNode>("<=", std::move(result), parseExpression());
        }
        else if (op == GREATER_EQUAL)
        {
            consume(GREATER_EQUAL);
            result = std::make_unique<BinaryOpNode>(">=", std::move(result), parseExpression());
        }
        else if (op == EQUAL_EQUAL)
        {
            consume(EQUAL_EQUAL);
            result = std::make_unique<BinaryOpNode>("==", std::move(result), parseExpression());
        }
        else
        {
            consume(BANG_EQUAL);
            result = std::make_unique<BinaryOpNode>("!=", std::move(result), parseExpression());
        }
    }

    return result;
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    unique_ptr<ASTNode> result;

    if (curr_Token.type == IDENTIFIER)
    {
        unique_ptr<ASTNode> variable = make_unique<VariableNode>(curr_Token.value);
        consume(IDENTIFIER);
        consume(EQUAL);
        unique_ptr<ASTNode> value = parseComparison();

        result = make_unique<AssignmentNode>(move(variable), move(value));
    }

    return result;
}

std::unique_ptr<ASTNode> Parser::parseIf()
{
    consume(IF);

    consume(LPAREN);
    std::unique_ptr<ASTNode> condition = parseComparison();
    consume(RPAREN);

    consume(LBRACE);

    std::unique_ptr<ProgramNode> body = std::make_unique<ProgramNode>();

    while(curr_Token.type != RBRACE)
    {
        if (curr_Token.type == NEWLINE)
        {
            consume(NEWLINE);
            continue;
        }

        body->statements.push_back(parseStatement());

        if (curr_Token.type == NEWLINE)
        {
            consume(NEWLINE);
        }
    }

    consume(RBRACE);

    if(curr_Token.type == ELSE)
    {
        consume(ELSE);

        if(curr_Token.type == IF)
        {
            std::unique_ptr<ASTNode> result = parseIf();
            return std::make_unique<IfNode>(std::move(condition),std::move(body),std::move(result));
        }
        else
        {
            std::unique_ptr<ProgramNode> elsebranch = std::make_unique<ProgramNode>();

            consume(LBRACE);

            while(curr_Token.type != RBRACE)
            {
                if (curr_Token.type == NEWLINE)
                {
                    consume(NEWLINE);
                    continue;
                }

                elsebranch->statements.push_back(parseStatement());

                if (curr_Token.type == NEWLINE)
                {
                    consume(NEWLINE);
                }
            }

            consume(RBRACE);

            return std::make_unique<IfNode>(std::move(condition),std::move(body),std::move(elsebranch));
        }
    }

    return std::make_unique<IfNode>(std::move(condition),std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{
    if(curr_Token.type == IF)
    {
        return parseIf();
    }
    else if(curr_Token.type == WHILE)
    {
        return parseWhile();
    }
    else if(curr_Token.type == IDENTIFIER)
    {
        if(peek_Token.type == EQUAL)
        {
            return parseAssignment();
        }
        else
        {
            return parseComparison();
        }
    }
    else if (curr_Token.type == TokenType::FN)
    {
        return parseFunction();
    }
    else if (curr_Token.type == TokenType::RETURN)
    {
        return parseReturn();
    }
    else if(curr_Token.type == PRINT)
    {
        return parsePrint();
    }
    else
    {
        return parseComparison();
    }
}

std::unique_ptr<ASTNode> Parser::parseWhile()
{
    consume(WHILE);

    consume(LPAREN);
    std::unique_ptr<ASTNode> condition = parseComparison();
    consume(RPAREN);

    consume(LBRACE);

    std::unique_ptr<ProgramNode> body = std::make_unique<ProgramNode>();

    while(curr_Token.type != RBRACE)
    {
        if (curr_Token.type == NEWLINE)
        {
            consume(NEWLINE);
            continue;
        }

        body->statements.push_back(parseStatement());

        if (curr_Token.type == NEWLINE)
        {
            consume(NEWLINE);
        }
    }

    consume(RBRACE);

    return std::make_unique<WhileNode>(
        std::move(condition),
        std::move(body)
    );
}

std::unique_ptr<ASTNode> Parser::parseReturn()
{
    advance();

    auto expression = parseExpression();

    return std::make_unique<ReturnNode>(std::move(expression));
}

std::unique_ptr<ASTNode> Parser::parseCall()
{
    std::string functionName = curr_Token.value;

    advance();
    consume(TokenType::LPAREN);

    std::vector<std::unique_ptr<ASTNode>> arguments;

    if (curr_Token.type != TokenType::RPAREN)
    {
        arguments.push_back(parseExpression());

        while (curr_Token.type == TokenType::COMMA)
        {
            advance();
            arguments.push_back(parseExpression());
        }
    }

    consume(TokenType::RPAREN);

    return std::make_unique<CallNode>(functionName, std::move(arguments));
}

std::unique_ptr<ASTNode> Parser::parseFunction()
{
    advance();

    std::string functionName = curr_Token.value;
    consume(TokenType::IDENTIFIER);

    consume(TokenType::LPAREN);

    std::vector<std::string> parameters;

    if (curr_Token.type != TokenType::RPAREN)
    {
        parameters.push_back(curr_Token.value);
        consume(TokenType::IDENTIFIER);

        while (curr_Token.type == TokenType::COMMA)
        {
            advance();

            parameters.push_back(curr_Token.value);
            consume(TokenType::IDENTIFIER);
        }
    }

    consume(TokenType::RPAREN);

    consume(TokenType::LBRACE);

    std::unique_ptr<ProgramNode> body = std::make_unique<ProgramNode>();

    while (curr_Token.type != RBRACE)
    {
        if (curr_Token.type == TokenType::NEWLINE)
        {
            advance();
            continue;
        }

        body->statements.push_back(parseStatement());
    }

    consume(TokenType::RBRACE);

    return std::make_unique<FunctionNode>(
        functionName,
        std::move(parameters),
        std::move(body)
    );
}

std::unique_ptr<ASTNode> Parser::parsePrint()
{
    consume(PRINT);

    std::unique_ptr<ASTNode> expression = parseComparison();

    return std::make_unique<PrintNode>(std::move(expression));
}

std::unique_ptr<ASTNode> Parser::parse()
{
    std::unique_ptr<ProgramNode> program = std::make_unique<ProgramNode>();

    while (curr_Token.type != END)
    {
        if (curr_Token.type == NEWLINE)
        {
            consume(NEWLINE);
            continue;
        }

        program->statements.push_back(parseStatement());

        if (curr_Token.type == NEWLINE)
        {
            consume(NEWLINE);
        }
    }

    return program;
}