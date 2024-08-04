#include "lexer/lexer.hpp"

#include <string.h>
#include <string>
#include <iostream>


void Lexer::parse() {
    bool start_line = true;
    while(pos < code_string->length()){
        if(start_line) {
            parse_indentation();
            start_line = false;
        }
        switch(get(pos)) {
            case '\'':
            case '"':
                parse_string();
                break;
            case '#':
                parse_comment();
                break;
            case '\n':
                parse_newline();
                start_line = true;
                break;
            case ' ':
            case '\t':
                increase_position();
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                parse_number();
                break;
            default:
                parse_word();
        }
    }
    pop_indentations();
}

void Lexer::parse_comment() {
    while(parse_char_besides('\n')) {}
}

void Lexer::parse_string() {
    char marker = get(pos);
    Range location = start_location_record();
    do {
        increase_position();
    } while((pos < code_string->length() && get(pos) != marker) || (pos > 0 && get(pos-1) == '\\'));
    submit_location_record(location, Token::String);
    increase_position();
}

bool is_digit(char c) {
    switch(c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true; 
    }
    return false;
}

void Lexer::parse_number() {
    Range location = start_location_record();
    while(parse_digit()) {}
    if(parse_char('.')){
        while(parse_digit()) {}
    }
    decrease_position();
    submit_location_record(location, Token::Number);
    increase_position();
}

void Lexer::parse_indentation() {
    while(pos != code_string->length() - 1 && (get(pos) == '\t' || get(pos) == ' ')){
        if(get(pos) == '\t') {
            Range location = start_location_record();
            submit_location_record(location, Token::Indentation);
            increase_position();
        } else if(get(pos) == ' ') {
            Range location = start_location_record();
            if(pos + 1 < code_string->length()) {
                increase_position();
                if(get(pos) == ' ') {
                    submit_location_record(location, Token::Indentation);
                } else {
                    // TODO: maybe throw an exception here
                }
            }  
        }
    }
}

void Lexer::parse_newline() {
    Range location = start_location_record();
    /* delete indentations before newline */
    pop_indentations();
    submit_location_record(location, Token::Newline);
    increase_line();
}

bool does_char_occur(char c, const char *string) {
    for(const char *cmp = string; *cmp; cmp++){
        if(c == *cmp)
            return true;
    }
    return false;
}

bool Lexer::parse_digit() {
    if(is_digit(get(pos))) {
        increase_position();
        return true;
    }
    return false;
}

void Lexer::parse_word() {
    Range location = start_location_record();
    if(does_char_occur(get(pos), LEXER_WORD_ENDING_CHARS)) {
        submit_location_record(location, Token::Word);
    } else {
        while(pos + 1 < code_string->length() && !does_char_occur(get(pos+1), LEXER_WORD_ENDING_CHARS))
            increase_position();
        submit_location_record(location, Token::Word);
    }
    increase_position();
}

bool Lexer::parse_char(char c) {
    if(pos < code_string->length() && get(pos) == c) {
        increase_position();
        return true;
    }
    return false;
}

bool Lexer::parse_char_besides(char c) {
    if(pos < code_string->length() && get(pos) != c) {
        increase_position();
        return true;
    }
    return false;
}

void Lexer::increase_position(){
    pos++, column++;
}

void Lexer::decrease_position() {
    pos--, column--;
}
void Lexer::increase_line() {
    pos++, column++, lineno++;
}

Range Lexer::start_location_record() {
    return Range(lineno, column, pos);
}

void Lexer::submit_location_record(Range range, Token::Type type) {
    range.endidx = pos;
    range.end.column = column;
    range.end.row = lineno;
    tokens.tokens.push_back(Token(type, range, code_string));
}

char Lexer::get(size_t idx) {
    return code_string->at(idx);
}

void Lexer::pop_indentations() {
    while(!tokens.tokens.empty() && tokens.tokens.back().type == Token::Indentation)
        tokens.tokens.pop_back();
}

std::string Token::literal() {
    return code_string->substr(location.startidx, location.endidx - location.startidx + 1);
}

void Token::print() {
    std::cout << "<" << type_strings[type];
    switch(type) {
        case Indentation:
        case Newline:
            std::cout << ">";
            break;
        case Number:
        case String:
        case Word:
            std::cout << ": \"" << literal() << "\">";
            break;
    }
}

void TokenSequence::print() {
    for(auto token: tokens) {
        token.print();
        if(token.type == Token::Newline) {
            std::cout << std::endl;
        }
    }
}