#include "parser/parser.hpp"

#include <memory>

std::unique_ptr<ast::Expression> Parser::parse_expression() {
    /* Expression = Factor | Factor + Expression | Factor - Expression */
    std::unique_ptr<ast::Expression> factor = parse_factor();
    if(parse_char('+')) {
        std::unique_ptr<ast::Expression> right = parse_expression();
        return ast::BinaryOp::create_add_operation(std::move(factor), std::move(right));
    } else if(parse_char('-')) {
        std::unique_ptr<ast::Expression> right = parse_expression();
        return ast::BinaryOp::create_minus_operation(std::move(factor), std::move(right));
    }
    return factor;
}

std::unique_ptr<ast::Expression> Parser::parse_factor() {
    /* Factor     = UnaryOp | UnaryOp * Factor | UnaryOp / Factor */
    std::unique_ptr<ast::Expression> unaryop = parse_unaryop();
    if(parse_char('*')) {
        std::unique_ptr<ast::Expression> right = parse_factor();
        return ast::BinaryOp::create_mul_operation(std::move(unaryop), std::move(right));
    } else if(parse_char('/')) {
        std::unique_ptr<ast::Expression> right = parse_factor();
        return ast::BinaryOp::create_div_operation(std::move(unaryop), std::move(right));
    }
    return unaryop;
}

std::unique_ptr<ast::Expression> Parser::parse_unaryop() {
    /* UnaryOp    = Value | -Value */
    if(parse_char('-'))
        return std::unique_ptr<ast::Expression>(new ast::UnaryOp(parse_value()));
    return parse_value();
}

std::unique_ptr<ast::Expression> Parser::parse_value() {
    /* Value      = Number | String | Access */
    if(next().type == Token::Number) {
        return std::unique_ptr<ast::Expression>(new ast::Number(parse_token()));
    } else if(next().type == Token::String) {
        return std::unique_ptr<ast::Expression>(new ast::String(parse_token()));
    }
    return parse_access();
}

std::unique_ptr<ast::Identifier> Parser::parse_access() {
    /* Access     = Identifier | Identifier.Access | Identifier[Expression] | Identifier[Expression].Access | Identifier(Parameters) */
    std::unique_ptr<ast::Identifier> identifier = parse_identifier();
    if(parse_char('.'))
        return std::unique_ptr<ast::Identifier>(new ast::ClassAccess(std::move(identifier), std::move(parse_access())));
    else if(parse_char('[')) {
        std::unique_ptr<ast::Expression> index = parse_expression();
        parse_char_or_panic(']');
        std::unique_ptr<ast::Identifier> index_access = std::unique_ptr<ast::IndexAccess>(new ast::IndexAccess(std::move(identifier), std::move(index)));
        if(parse_char('.'))
            return std::unique_ptr<ast::Identifier>(new ast::ClassAccess(std::move(index_access), std::move(parse_access())));
        return index_access;
    } else if(parse_char('(')) {
        std::unique_ptr<ast::FunctionCall> fcall = std::unique_ptr<ast::FunctionCall>(new ast::FunctionCall());
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

std::unique_ptr<ast::Identifier> Parser::parse_identifier() {
    return std::unique_ptr<ast::Identifier>(new ast::Identifier(parse_token()));
}