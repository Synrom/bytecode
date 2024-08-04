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
    std::string literal();
    void print();
private:
    std::map<Type, std::string> type_strings = {
        {Type::String, "string"},
        {Type::Number, "number"},
        {Type::Indentation, "indent"},
        {Type::Newline, "newline"},
        {Type::Word, "word"},
    };
    Range location;
    std::shared_ptr<std::string> code_string;
};

class TokenSequence {
public:
    std::vector<Token> tokens;
    void print();
};