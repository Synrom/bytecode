#include <iostream>

#include "ast/ast.hpp"
#include "ast/print.hpp"

using namespace ast;

std::shared_ptr<BinaryOp> BinaryOp::create(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, Operation op, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<BinaryOp> ast = std::shared_ptr<BinaryOp>(new BinaryOp(left, right, op, tokens, parent));
    left->parent = ast;
    right->parent = ast;
    return ast;
}

std::shared_ptr<UnaryOp> UnaryOp::create(std::shared_ptr<Expression> expr, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<UnaryOp> ast = std::shared_ptr<UnaryOp>(new UnaryOp(expr, tokens, parent));
    expr->parent = ast;
    return ast;
}

std::shared_ptr<ClassAccess> ClassAccess::create(std::shared_ptr<Access> left, std::shared_ptr<Access> right, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<ClassAccess> ast = std::shared_ptr<ClassAccess>(new ClassAccess(left, right, tokens, parent));
    left->parent = ast;
    right->parent = ast;
    return ast;
}


std::shared_ptr<IndexAccess> IndexAccess::create(std::shared_ptr<Access> left, std::shared_ptr<Expression> index, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<IndexAccess> ast = std::shared_ptr<IndexAccess>(new IndexAccess(left, index, tokens, parent));
    left->parent = ast;
    index->parent = ast;
    return ast;
}

std::shared_ptr<FunctionCall> FunctionCall::create(std::shared_ptr<Access> name, TokenRange tokens, std::shared_ptr<Node> parent, std::vector<std::shared_ptr<Expression>> parameters) {
    std::shared_ptr<FunctionCall> ast = std::shared_ptr<FunctionCall>(new FunctionCall(name, tokens, parent, parameters));
    name->parent = ast;
    return ast;
}

std::string BinaryOp::string_op() {
    switch (op) {
        case Mul: return std::string("Multiply");
        case Add: return std::string("Add");
        case Div: return std::string("Divide");
        case Min: return std::string("Minus");
    }
    return std::string("Unknown");
}

void AstPrinter::print_node(const char *name, const TokenRange &location) {
    std::cout << indentation << name << "<";
    print_location(location);
    std::cout << ">\n";
}

void AstPrinter::print_location(const TokenRange &location) {
    std::cout << location.tokens->tokens[location.start].location.start.row;
    std::cout << ":";
    std::cout << location.tokens->tokens[location.start].location.start.column;
    std::cout << "-";
    std::cout << location.tokens->tokens[location.end].location.end.row;
    std::cout << ":";
    std::cout << location.tokens->tokens[location.end].location.end.column;
}

void AstPrinter::print_literal(const Literal &node) {
    std::cout << indentation << node.token.literal() << "<";
    print_location(node.tokens);
    std::cout << ">\n";
}

void AstPrinter::print(const std::shared_ptr<ast::Node> &node) {
    node->visit(*this);
}

void AstPrinter::visit_binary_op(BinaryOp &node) {
    std::string identifier = std::string("BinaryOp(") + node.string_op() + ")";
    print_node(identifier.c_str(), node.tokens);
    increase_indentation();
    node.left->visit(*this);
    node.right->visit(*this);
    decrease_indentation();
}

void AstPrinter::visit_unary_op(UnaryOp &node) {
    print_node("UnaryOp", node.tokens);
    increase_indentation();
    node.expr->visit(*this);
    decrease_indentation();
}

void AstPrinter::visit_number(Number &node) {
    print_literal(node);
}

void AstPrinter::visit_string(String &node) {
    print_literal(node);
}

void AstPrinter::visit_identifier(Identifier &node) {
    print_literal(node);
}

void AstPrinter::visit_class_access(ClassAccess &node) {
    print_node("ClassAccess", node.tokens);
    increase_indentation();
    node.left->visit(*this);
    node.right->visit(*this);
    decrease_indentation();
}

void AstPrinter::visit_index_access(IndexAccess &node) {
    print_node("IndexAccess", node.tokens);
    increase_indentation();
    node.left->visit(*this);
    node.index->visit(*this);
    decrease_indentation();
}

void AstPrinter::visit_function_call(FunctionCall &node) {
    print_node("FunctionCall", node.tokens);
    increase_indentation();
    node.name->visit(*this);
    for(auto parameter: node.parameters)  {
        parameter->visit(*this);
    }
    decrease_indentation();
}

void Node::print() {
    AstPrinter printer;
    this->visit(printer);
}