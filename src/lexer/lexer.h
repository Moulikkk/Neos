#pragma once
#include <string>

enum TokenType
{
    NUMBER,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL_EQUAL,
    BANG_EQUAL,
    EQUAL,
    IDENTIFIER,
    IF,
    ELSE,
    WHILE,
    PRINT,
    FN,
    RETURN,
    COMMA,
    STRING,
    TRUE,
    FALSE,
    NEWLINE,
    END
};

struct Token
{
    TokenType type;
    std::string value;
    int line = 0;
};

class Lexer
{
    std::string input;
    int curr_position = 0;
    int line = 1;

public:
    Lexer(std::string s);
    Token nextToken();
    bool match(char expected);
};

inline bool isLetter(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

inline bool isDigit(char c)
{
    return ((c >= '0') && (c <= '9'));
}

inline bool isIdentifierChar(char c)
{
    return (isLetter(c) || isDigit(c) || c == '_');
}