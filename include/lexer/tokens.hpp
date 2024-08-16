#ifndef TOKENS_H
#define TOKENS_H

#include "location.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <string>

class Token {
public: 
    enum Type {
        String, // everything in between '' or ""
        Number, // floats are allowed
        Indentation, // \t or "  "
        Newline, // \n
        Word, // Every keyword or identifier
    };
    Token(Type type, Range location, std::shared_ptr<std::string> code_string) : type(type), location(location), code_string(code_string) {}
    Type type;
    std::string literal() const;
    void print();
    Range location;
private:
    std::map<Type, std::string> type_strings = {
        {Type::String, "string"},
        {Type::Number, "number"},
        {Type::Indentation, "indent"},
        {Type::Newline, "newline"},
        {Type::Word, "word"},
    };
    std::shared_ptr<std::string> code_string;
};

class TokenSequence {
public:
    TokenSequence(std::string filepath) : filepath(filepath) {}
    TokenSequence(char *filepath) : filepath(filepath) {}
    std::vector<Token> tokens;
    std::string filepath;
    void print();
};

class TokenRange {
public:
    TokenRange(std::shared_ptr<TokenSequence> tokens, size_t start, size_t end) : tokens(tokens), start(start), end(end) {}
    std::shared_ptr<TokenSequence> tokens;
    size_t start, end;
};

#endif