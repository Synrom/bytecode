#ifndef PARSER_H
#define PARSER_H

#include "lexer/tokens.hpp"
#include "ast/ast.hpp"

#include <memory>

/*
 * Statement        = IdentifierAcess = Expression | return Expression
 * Expression       = Factor | Factor + Expression | Factor - Expression
 * Factor           = UnaryOp | UnaryOp * Factor | UnaryOp / Factor
 * UnaryOp          = Value | -Value
 * Value            = Number | String | IdentifierAccess | (Expression)
 * IdentifierAccess = <Identifier><Access> | Identifier
 * Access           = None | .IdentifierAccess | [Expression]Access | (Parameters)Access
 */

class Parser{
public:
    Parser(std::shared_ptr<TokenSequence> tokens) : tokens(tokens), position(0), indentation(0) {}
    std::vector<std::shared_ptr<ast::Statement>> parse_block();

private:
    std::shared_ptr<ast::Statement> parse_statement();
    std::shared_ptr<ast::Expression> parse_expression();
    std::shared_ptr<ast::Expression> parse_factor();
    std::shared_ptr<ast::Expression> parse_unaryop();
    std::shared_ptr<ast::Expression> parse_value();
    std::shared_ptr<ast::Access> parse_identifier_access();
    std::shared_ptr<ast::Access> parse_access(std::shared_ptr<ast::Access> chain);
    std::shared_ptr<ast::Access> parse_identifier();

    bool parse_char(char c);
    bool parse_string(const std::string &s);
    bool parse_newline();
    void parse_char_or_panic(char c);
    Token &parse_token();
    Token &next();

    void increase();
    TokenRange start_token_range();
    TokenRange start_token_range(std::shared_ptr<ast::Node> node);
    void end_token_range(TokenRange &range); /* should end with posiition-1*/

    std::shared_ptr<TokenSequence> tokens;
    size_t position;
    /* We use the variable index as index on the stack */
    std::vector<std::string> variables;
    ssize_t indentation;
};

#endif