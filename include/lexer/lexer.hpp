#ifndef LEXER_H
#define LEXER_H

#include "lexer/tokens.hpp"
#include <memory>
#include <string>

#define LEXER_WORD_ENDING_CHARS ".(){}[],1234567890\n\"'\t #-+*/%:\n"

class Lexer{
public:
    Lexer(std::shared_ptr<std::string> code_string) : code_string(code_string), pos(0), lineno(0), column(0), tokens(new TokenSequence()) {}
    void parse();
    std::shared_ptr<TokenSequence> tokens;

private:
    std::shared_ptr<std::string> code_string;
    size_t pos, lineno, column;

    void parse_comment();
    void parse_string();
    void parse_number();
    void parse_indentation();
    void parse_newline();
    void parse_word();

    bool parse_char(char c);
    bool parse_char_besides(char c);
    bool parse_digit();

    void increase_position();
    void decrease_position();
    void increase_line();
    Range start_location_record();
    void submit_location_record(Range range, Token::Type type);
    char get(size_t idx);
    void pop_indentations();
};

#endif