#include "parser/parser.hpp"

#include <memory>
#include <iostream>

std::unique_ptr<ast::Expression> Parser::parse_expression() {
    /* Expression = Factor | Factor + Expression | Factor - Expression */
    std::unique_ptr<ast::Expression> factor = parse_factor();
    if(parse_char('+')) {
        std::unique_ptr<ast::Expression> right = parse_expression();
        return std::unique_ptr<ast::Expression>(new ast::BinaryOp(std::move(factor), std::move(right), ast::BinaryOp::Add, token_range()));
    } else if(parse_char('-')) {
        std::unique_ptr<ast::Expression> right = parse_expression();
        return std::unique_ptr<ast::Expression>(new ast::BinaryOp(std::move(factor), std::move(right), ast::BinaryOp::Min, token_range()));
    }
    return factor;
}

std::unique_ptr<ast::Expression> Parser::parse_factor() {
    /* Factor     = UnaryOp | UnaryOp * Factor | UnaryOp / Factor */
    std::unique_ptr<ast::Expression> unaryop = parse_unaryop();
    if(parse_char('*')) {
        std::unique_ptr<ast::Expression> right = parse_factor();
        return std::unique_ptr<ast::Expression>(new ast::BinaryOp(std::move(unaryop), std::move(right), ast::BinaryOp::Mul, token_range()));
    } else if(parse_char('/')) {
        std::unique_ptr<ast::Expression> right = parse_factor();
        return std::unique_ptr<ast::Expression>(new ast::BinaryOp(std::move(unaryop), std::move(right), ast::BinaryOp::Div, token_range()));
    }
    return unaryop;
}

std::unique_ptr<ast::Expression> Parser::parse_unaryop() {
    /* UnaryOp    = Value | -Value */
    if(parse_char('-'))
        return std::unique_ptr<ast::Expression>(new ast::UnaryOp(parse_value(), token_range()));
    return parse_value();
}

std::unique_ptr<ast::Expression> Parser::parse_value() {
    /* Value      = Number | String | Access */
    if(next().type == Token::Number) {
        return std::unique_ptr<ast::Expression>(new ast::Number(parse_token(), token_range()));
    } else if(next().type == Token::String) {
        return std::unique_ptr<ast::Expression>(new ast::String(parse_token(), token_range()));
    }
    return parse_access();
}

std::unique_ptr<ast::Access> Parser::parse_access() {
    /* Access     = Identifier | Identifier.Access | Identifier[Expression] | Identifier[Expression].Access | Identifier(Parameters) */
    std::unique_ptr<ast::Access> identifier = parse_identifier();
    if(parse_char('.'))
        return std::unique_ptr<ast::Access>(new ast::ClassAccess(std::move(identifier), std::move(parse_access()), token_range()));
    else if(parse_char('[')) {
        std::unique_ptr<ast::Expression> index = parse_expression();
        parse_char_or_panic(']');
        std::unique_ptr<ast::Access> index_access = std::unique_ptr<ast::IndexAccess>(new ast::IndexAccess(std::move(identifier), std::move(index), token_range()));
        if(parse_char('.'))
            return std::unique_ptr<ast::Access>(new ast::ClassAccess(std::move(index_access), std::move(parse_access()), token_range()));
        return index_access;
    } else if(parse_char('(')) {
        std::unique_ptr<ast::FunctionCall> fcall = std::unique_ptr<ast::FunctionCall>(new ast::FunctionCall(std::move(identifier), token_range()));
        bool first = true;
        while(!parse_char(')')) {
            if(!first)
                parse_char_or_panic(',');
            else
                first = false;
            fcall->add_parameter(parse_expression());
        }
        return fcall;
    }
    return identifier;
}

std::unique_ptr<ast::Access> Parser::parse_identifier() {
    return std::unique_ptr<ast::Access>(new ast::Identifier(parse_token(), token_range()));
}

bool Parser::parse_char(char c) {
    if(next().type == Token::Word && next().literal() == std::string{c}) {
        increase();
        return true;
    }
    return false;
}

Token &Parser::parse_token() {
    Token &token = next();
    increase();
    return token;
}

Token &Parser::next() {
    return tokens->tokens[range.end];
}

TokenRange Parser::token_range() {
    TokenRange current = TokenRange(tokens, range.start, range.end - 1); // range.end-1 because end points to the next token
    range.start = range.end;
    return current;
}

void Parser::parse_char_or_panic(char c) {
    if(parse_char(c))
        return;
    std::cout << "Panic: Parser error" << std::endl;
}

void Parser::increase() {
    range.end++;
}