#ifndef PARSER_H
#define PARSER_H

#include "lexer/tokens.hpp"
#include "ast/ast.hpp"

#include <memory>

/*
 * Expression = Factor | Factor + Expression | Factor - Expression
 * Factor     = UnaryOp | UnaryOp * Factor | UnaryOp / Factor
 * UnaryOp    = Value | -Value
 * Value      = Number | String | Access
 * Access     = Identifier | Identifier.Access | Identifier[Expression].Access | Identifier(Parameters)
 */

class Parser{
public:
    Parser(std::shared_ptr<TokenSequence> tokens) : tokens(tokens), range(tokens, 0, 0) {}
    std::unique_ptr<ast::Expression> parse_expression(); // TODO: make private after testing

private:
    std::unique_ptr<ast::Expression> parse_factor();
    std::unique_ptr<ast::Expression> parse_unaryop();
    std::unique_ptr<ast::Expression> parse_value();
    std::unique_ptr<ast::Access> parse_access();
    std::unique_ptr<ast::Access> parse_identifier();

    bool parse_char(char c);
    void parse_char_or_panic(char c);
    Token &parse_token();
    Token &next();
    TokenRange token_range(); // automatically restart token range and return range up so far
    void increase();

    std::shared_ptr<TokenSequence> tokens;
    TokenRange range;
};

#endif