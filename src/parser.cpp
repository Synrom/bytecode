#include "parser/parser.hpp"

#include <memory>
#include <iostream>

std::shared_ptr<ast::Node> expand_leftmost(std::shared_ptr<ast::Node> root, std::shared_ptr<ast::Node> item) {
    std::shared_ptr<ast::Node> traverse = root->get_left_child();
    while (traverse and traverse->presedence >= item->presedence and traverse->get_left_child() != NULL)
        traverse = traverse->get_left_child();
    if (!traverse or !traverse->parent) {
        item->set_right_child(root);
        return item;
    }
    std::shared_ptr<ast::Node> parent = traverse->parent;
    parent->set_left_child(item);
    item->parent = parent;
    item->set_right_child(traverse);
    traverse->parent = item;
    return root;
}

std::shared_ptr<ast::Expression> Parser::parse_expression() {
    /* Expression = Factor | Factor + Expression | Factor - Expression */
    TokenRange range = start_token_range();
    std::shared_ptr<ast::Expression> factor = parse_factor();
    if(parse_char('+')) {
        std::shared_ptr<ast::Expression> right = parse_expression();
        end_token_range(range);
        std::shared_ptr<ast::Expression> binary = ast::BinaryOp::create(factor, NULL, ast::BinaryOp::Add, range, NULL);
        return std::dynamic_pointer_cast<ast::Expression>(expand_leftmost(right, binary));
    } else if(parse_char('-')) {
        std::shared_ptr<ast::Expression> right = parse_expression();
        end_token_range(range);
        std::shared_ptr<ast::Expression> binary = ast::BinaryOp::create(factor, NULL, ast::BinaryOp::Min, range, NULL);
        return std::dynamic_pointer_cast<ast::Expression>(expand_leftmost(right, binary));
    }
    return factor;
}

std::shared_ptr<ast::Expression> Parser::parse_factor() {
    /* Factor     = UnaryOp | UnaryOp * Factor | UnaryOp / Factor */
    TokenRange range = start_token_range();
    std::shared_ptr<ast::Expression> unaryop = parse_unaryop();
    if(parse_char('*')) {
        std::shared_ptr<ast::Expression> right = parse_factor();
        end_token_range(range);
        return ast::BinaryOp::create(unaryop, right, ast::BinaryOp::Mul, range, NULL);
    } else if(parse_char('/')) {
        std::shared_ptr<ast::Expression> right = parse_factor();
        end_token_range(range);
        return ast::BinaryOp::create(unaryop, right, ast::BinaryOp::Div, range, NULL);
    }
    return unaryop;
}

std::shared_ptr<ast::Expression> Parser::parse_unaryop() {
    /* UnaryOp    = Value | -Value */
    TokenRange range = start_token_range();
    if(parse_char('-')) {
        std::shared_ptr<ast::Expression> value = parse_value();
        end_token_range(range);
        return ast::UnaryOp::create(value, range, NULL);
    }
    return parse_value();
}

std::shared_ptr<ast::Expression> Parser::parse_value() {
    /* Value      = Number | String | Access | (Expression) */
    TokenRange range = start_token_range();
    if(next().type == Token::Number) {
        Token &token = parse_token();
        end_token_range(range);
        return std::shared_ptr<ast::Expression>(new ast::Number(token, range, NULL));
    } else if(next().type == Token::String) {
        Token &token = parse_token();
        end_token_range(range);
        return std::shared_ptr<ast::Expression>(new ast::String(token, range, NULL));
    } else if(parse_char('(')) {
        std::shared_ptr<ast::Expression> expr = parse_expression();
        parse_char_or_panic(')');
        return expr;
    }
    return parse_identifier_access();
}

std::shared_ptr<ast::Access> Parser::parse_identifier_access() {
    /* Access     = IdentifierAccess | Identifier */
    TokenRange range = start_token_range();
    std::shared_ptr<ast::Access> identifier = parse_identifier();
    return parse_access(identifier);
}

std::shared_ptr<ast::Access> Parser::parse_access(std::shared_ptr<ast::Access> chain) {
    /* Access     = None | .IdentifierAccess | [Expression]Access | (Parameters)Access */
    TokenRange range = start_token_range(chain);
    if(parse_char('.')) {
        std::shared_ptr<ast::Access> access = parse_identifier_access();
        end_token_range(range);
        return ast::ClassAccess::create(chain, access, range, NULL);
    } else if(parse_char('[')) {
        std::shared_ptr<ast::Expression> index = parse_expression();
        parse_char_or_panic(']');
        end_token_range(range);
        std::shared_ptr<ast::Access> index_access = ast::IndexAccess::create(chain, index, range, NULL);
        return parse_access(index_access);
    } else if(parse_char('(')) {
        std::vector<std::shared_ptr<ast::Expression>> parameters;
        bool first = true;
        while(!parse_char(')')) {
            if(!first)
                parse_char_or_panic(',');
            else
                first = false;
            parameters.push_back(parse_expression());
        }
        end_token_range(range);
        std::shared_ptr<ast::Access> fcall = ast::FunctionCall::create(chain, range, NULL, parameters);
        return parse_access(fcall);
    }
    return chain;
}

std::shared_ptr<ast::Access> Parser::parse_identifier() {
    TokenRange range = start_token_range();
    Token &token = parse_token();
    end_token_range(range);
    return std::shared_ptr<ast::Access>(new ast::Identifier(token, range, NULL));
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
    return tokens->tokens[position];
}

void Parser::parse_char_or_panic(char c) {
    if(parse_char(c))
        return;
    std::cout << "Panic: Parser error" << std::endl;
}

void Parser::increase() {
    position++;
}

TokenRange Parser::start_token_range() {
    return TokenRange(tokens, position, position);
}

TokenRange Parser::start_token_range(std::shared_ptr<ast::Node> node) {
    return TokenRange(tokens, node->tokens.start, position);
}

void Parser::end_token_range(TokenRange &range) {
    range.end = position - 1;
}

