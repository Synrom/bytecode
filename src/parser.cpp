#include "parser/parser.hpp"

#include <memory>
#include <iostream>

std::shared_ptr<ast::Node> expand_leftmost(std::shared_ptr<ast::Node> root, std::shared_ptr<ast::Node> item) {
    std::shared_ptr<ast::Node> traverse;
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

std::shared_ptr<ast::FunctionDefinition> Parser::parse_function() {
    TokenRange range = start_token_range();
    if (!parse_indentation() or !parse_string("def"))
        return NULL;
    std::shared_ptr<ast::Identifier> fname = parse_identifier();
    parse_char_or_panic('(');
    std::vector<std::shared_ptr<ast::Identifier>> fparameters;
    while (!parse_char(')')) {
        if (not fparameters.empty())
            parse_char_or_panic(',');
        fparameters.push_back(parse_identifier());
    }
    parse_char_or_panic(':');
    parse_newline_or_panic();
    increase_indentation();
    std::shared_ptr<ast::Block> fblock = parse_block();
    decrease_indentation();
    end_token_range(range);
    return ast::FunctionDefinition::create(
        fblock,
        fparameters,
        fname,
        range,
        NULL
    );
}

std::shared_ptr<ast::File> Parser::parse_file() {
    while (position < tokens->tokens.size()) {
        if (parse_string("def")) {
            decrease();
            std::shared_ptr<ast::FunctionDefinition> function = parse_function();
            file->functions.push_back(function);
            function->parent = file;
        } else if (parse_string("class")) {
            TokenRange range = start_token_range();
            std::shared_ptr<ast::Identifier> cname = parse_identifier();
            parse_char_or_panic(':');
            parse_newline_or_panic();
            increase_indentation();
            std::vector<std::shared_ptr<ast::FunctionDefinition>> functions;
            /* TODO: parse class methods */
            while (parse_newline());
            std::shared_ptr<ast::FunctionDefinition> func;
            while (func = parse_function()) {
                functions.push_back(func);
                while (parse_newline());
            }
            decrease_indentation();
            end_token_range(range);
            std::shared_ptr<ast::ClassDefinition> class_definition = ast::ClassDefinition::create(
                functions,
                cname->token.literal(),
                range,
                file
            );
            file->classes.push_back(class_definition);
        }else if (parse_newline()) {
        } else {
            file->code->statements.push_back(parse_statement());
            if (position < tokens->tokens.size())
                parse_newline();
        }
    }
    return file;
}

std::shared_ptr<ast::Block> Parser::parse_block() {
    bool seperated = true;
    TokenRange range = start_token_range();
    std::vector<std::shared_ptr<ast::Statement>> block;
    while (seperated) {
        seperated = false;
        if (!parse_indentation())
            break;
        block.push_back(parse_statement());
        seperated = parse_newline();
        while(parse_newline()) ;
    }
    end_token_range(range);
    return ast::Block::create(block, range, NULL);
}

std::shared_ptr<ast::Statement> Parser::parse_statement() {
    /* Statement        = IdentifierAcess = Expression | return Expression */
    TokenRange range = start_token_range();
    if (parse_string(std::string("return"))) {
        std::shared_ptr<ast::Expression> expr = parse_expression();
        end_token_range(range);
        return ast::Return::create(expr, range, NULL);
    }
    /* TODO: If, For, While-statements*/
    std::shared_ptr<ast::Expression> expr = Parser::parse_expression();
    if (std::shared_ptr<ast::Access> identifier = std::dynamic_pointer_cast<ast::Access>(expr)) {
        if (parse_char('=')) {
            std::shared_ptr<ast::Expression> left = parse_expression();
            end_token_range(range);
            return ast::Assign::create(identifier, left, NULL, range);
        }
    }
    return expr;
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
        std::shared_ptr<ast::Expression> binary = ast::BinaryOp::create(unaryop, NULL, ast::BinaryOp::Mul, range, NULL);
        return std::dynamic_pointer_cast<ast::Expression>(expand_leftmost(right, binary));
    } else if(parse_char('/')) {
        std::shared_ptr<ast::Expression> right = parse_factor();
        end_token_range(range);
        std::shared_ptr<ast::Expression> binary = ast::BinaryOp::create(unaryop, NULL, ast::BinaryOp::Div, range, NULL);
        return std::dynamic_pointer_cast<ast::Expression>(expand_leftmost(right, binary));
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
        if (std::shared_ptr<ast::Identifier> identifier = std::dynamic_pointer_cast<ast::Identifier>(chain)) {
            std::shared_ptr<ast::Access> fcall = ast::FunctionCall::create(identifier, range, NULL, parameters);
            return parse_access(fcall);
        } else {
            std::cout << "Error: Parser tries to create function call without identifier for:\n";
            chain->print();
        }
    }
    return chain;
}

std::shared_ptr<ast::Identifier> Parser::parse_identifier() {
    TokenRange range = start_token_range();
    Token &token = parse_token();
    end_token_range(range);
    return std::shared_ptr<ast::Identifier>(new ast::Identifier(token, range, NULL));
}

bool Parser::parse_char(char c) {
    if(next().type == Token::Word && next().literal() == std::string{c}) {
        increase();
        return true;
    }
    return false;
}

bool Parser::parse_string(const std::string &s) {
    if(next().type == Token::Word && next().literal() == s) {
        increase();
        return true;
    }
    return false;
}

Token & Parser::parse_token() {
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

void Parser::parse_newline_or_panic() {
    if(parse_newline())
        return;
    std::cout << "Panic: Parser error" << std::endl;
}

void Parser::increase() {
    position++;
}

void Parser::decrease() {
    position--;
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

bool Parser::parse_newline() {
    if (position <= tokens->tokens.size() and tokens->tokens[position].type == Token::Newline) {
        increase();
        return true;
    }
    return false;
}

bool Parser::parse_indentation() {
    /* First check that we have enough indentation tokens */
    for (int i = 0; i < indentation; i++) {
        if (position + i >= tokens->tokens.size() or tokens->tokens[position + i].type != Token::Indentation) 
            return false;
    }
    /* Then actually parse the indentation tokens */
    for (int i = 0; i < indentation; i++) parse_token();
    return true;
}

void Parser::increase_indentation() {
    indentation++;
}

void Parser::decrease_indentation() {
    indentation--;
}