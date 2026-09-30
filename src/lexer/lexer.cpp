#include <stdexcept>
#include <cctype>
#include "lexer.h"
using namespace std;

Lexer::Lexer(string s)
{
    input = s;
}

bool Lexer::match(char expected)
{
    if (curr_position + 1 >= input.size())
        return false;
    if (input[curr_position + 1] != expected)
        return false;
    curr_position += 2;
    return true;
}

Token Lexer::nextToken()
{
    while (curr_position < input.size() && input[curr_position] == ' ')
        curr_position++;

    if (curr_position >= input.size())
        return {TokenType::END, "", line};

    if (input[curr_position] == '+') { curr_position++; return {TokenType::PLUS, "+", line}; }
    if (input[curr_position] == '-') { curr_position++; return {TokenType::MINUS, "-", line}; }
    if (input[curr_position] == '*') { curr_position++; return {TokenType::STAR, "*", line}; }

    if (input[curr_position] == '/')
    {
        if (curr_position + 1 < input.size() && input[curr_position + 1] == '/')
        {
            while (curr_position < input.size() && input[curr_position] != '\n')
                curr_position++;
            return nextToken();
        }
        curr_position++;
        return {TokenType::SLASH, "/", line};
    }

    if (input[curr_position] == '(') { curr_position++; return {TokenType::LPAREN, "(", line}; }
    if (input[curr_position] == ')') { curr_position++; return {TokenType::RPAREN, ")", line}; }
    if (input[curr_position] == '{') { curr_position++; return {TokenType::LBRACE, "{", line}; }
    if (input[curr_position] == '}') { curr_position++; return {TokenType::RBRACE, "}", line}; }
    if (input[curr_position] == ',') { curr_position++; return {TokenType::COMMA, ",", line}; }

    if (input[curr_position] == '<')
    {
        if (match('=')) return {TokenType::LESS_EQUAL, "<=", line};
        curr_position++;
        return {TokenType::LESS, "<", line};
    }

    if (input[curr_position] == '>')
    {
        if (match('=')) return {TokenType::GREATER_EQUAL, ">=", line};
        curr_position++;
        return {TokenType::GREATER, ">", line};
    }

    if (input[curr_position] == '=')
    {
        if (match('=')) return {TokenType::EQUAL_EQUAL, "==", line};
        curr_position++;
        return {TokenType::EQUAL, "=", line};
    }

    if (input[curr_position] == '!')
    {
        if (match('=')) return {TokenType::BANG_EQUAL, "!=", line};
        throw runtime_error("Unexpected '!' on line " + to_string(line));
    }

    if (input[curr_position] == '"')
    {
        curr_position++;
        Token String;
        String.type = TokenType::STRING;
        String.line = line;

        while (curr_position < input.size() && input[curr_position] != '"')
        {
            String.value.push_back(input[curr_position]);
            curr_position++;
        }

        if (curr_position >= input.size())
            throw runtime_error("unclosed string on line " + to_string(line));

        curr_position++;
        return String;
    }

    if (isDigit(input[curr_position]))
    {
        Token Number;
        Number.type = TokenType::NUMBER;
        Number.line = line;

        while (curr_position < input.size() && isDigit(input[curr_position]))
        {
            Number.value.push_back(input[curr_position]);
            curr_position++;
        }

        if (curr_position < input.size() && input[curr_position] == '.')
        {
            Number.value.push_back('.');
            curr_position++;

            while (curr_position < input.size() && isDigit(input[curr_position]))
            {
                Number.value.push_back(input[curr_position]);
                curr_position++;
            }
        }

        return Number;
    }

    if (isLetter(input[curr_position]) || input[curr_position] == '_')
    {
        Token Variable;
        Variable.type = TokenType::IDENTIFIER;
        Variable.line = line;

        while (curr_position < input.size() && isIdentifierChar(input[curr_position]))
        {
            Variable.value.push_back(input[curr_position]);
            curr_position++;
        }

        if (Variable.value == "if")         Variable.type = TokenType::IF;
        else if (Variable.value == "else")   Variable.type = TokenType::ELSE;
        else if (Variable.value == "while")  Variable.type = TokenType::WHILE;
        else if (Variable.value == "fn")     Variable.type = TokenType::FN;
        else if (Variable.value == "return") Variable.type = TokenType::RETURN;
        else if (Variable.value == "print")  Variable.type = TokenType::PRINT;
        else if (Variable.value == "true")   Variable.type = TokenType::TRUE;
        else if (Variable.value == "false")  Variable.type = TokenType::FALSE;

        return Variable;
    }

    if (input[curr_position] == '\n')
    {
        curr_position++;
        line++;
        return {TokenType::NEWLINE, "\\n", line};
    }

    throw runtime_error("Unknown character on line " + to_string(line));
}