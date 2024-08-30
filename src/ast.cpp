#include <iostream>

#include "ast/ast.hpp"
#include "ast/print.hpp"

using namespace ast;

std::shared_ptr<BinaryOp> BinaryOp::create(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, Operation op, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<BinaryOp> ast = std::shared_ptr<BinaryOp>(new BinaryOp(left, right, op, tokens, parent));
    if (left)
        left->parent = ast;
    if (right)
        right->parent = ast;
    return ast;
}

std::shared_ptr<UnaryOp> UnaryOp::create(std::shared_ptr<Expression> expr, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<UnaryOp> ast = std::shared_ptr<UnaryOp>(new UnaryOp(expr, tokens, parent));
    if (expr)
        expr->parent = ast;
    return ast;
}

std::shared_ptr<Return> Return::create(std::shared_ptr<Expression> expr, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<Return> ast = std::shared_ptr<Return>(new Return(expr, parent, tokens));
    if (expr)
        expr->parent = ast;
    return ast;
}

std::shared_ptr<Assign> Assign::create(std::shared_ptr<Access> location, std::shared_ptr<Expression> expr, std::shared_ptr<Node> parent, TokenRange tokens) {
    std::shared_ptr<Assign> ast = std::shared_ptr<Assign>(new Assign(location, expr, parent, tokens));
    if (expr)
        expr->parent = ast;
    if (location)
        location->parent = ast;
    return ast;
}

std::shared_ptr<Block> Block::create(std::vector<std::shared_ptr<Statement>> statements, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<Block> ast = std::shared_ptr<Block>(new Block(statements, tokens, parent));
    for (auto statement = statements.begin(); statement != statements.end(); statement++)
        statement->get()->parent = ast;
    return ast;
}

std::shared_ptr<FunctionDefinition> FunctionDefinition::create(std::shared_ptr<Block> block, std::vector<std::shared_ptr<Identifier>> parameters, std::shared_ptr<Identifier> name, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<FunctionDefinition> ast = std::shared_ptr<FunctionDefinition>(new FunctionDefinition(block, parameters, name, tokens, parent));
    if (block)
        block->parent = ast;
    return ast;    
}

std::shared_ptr<ClassAccess> ClassAccess::create(std::shared_ptr<Access> left, std::shared_ptr<Access> right, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<ClassAccess> ast = std::shared_ptr<ClassAccess>(new ClassAccess(left, right, tokens, parent));
    if (left)
        left->parent = ast;
    if (right)
        right->parent = ast;
    return ast;
}

std::shared_ptr<File> File::create(std::string filename, TokenRange tokens) {
    std::shared_ptr<File> file = std::shared_ptr<File>(new File(filename, tokens));
    file->code = Block::create(tokens, file);
    return file;
}

std::shared_ptr<IndexAccess> IndexAccess::create(std::shared_ptr<Access> left, std::shared_ptr<Expression> index, TokenRange tokens, std::shared_ptr<Node> parent) {
    std::shared_ptr<IndexAccess> ast = std::shared_ptr<IndexAccess>(new IndexAccess(left, index, tokens, parent));
    if (left)
        left->parent = ast;
    if (index)
        index->parent = ast;
    return ast;
}

std::shared_ptr<FunctionCall> FunctionCall::create(std::shared_ptr<Access> name, TokenRange tokens, std::shared_ptr<Node> parent, std::vector<std::shared_ptr<Expression>> parameters) {
    std::shared_ptr<FunctionCall> ast = std::shared_ptr<FunctionCall>(new FunctionCall(name, tokens, parent, parameters));
    if (name)
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

void AstPrinter::visit_return_stmt(Return &node) {
    print_node("Return", node.tokens);
    increase_indentation();
    node.expr->visit(*this);
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

void AstPrinter::visit_assign_stmt(Assign &node) {
    print_node("Assign", node.tokens);
    increase_indentation();
    node.location->visit(*this);
    node.expr->visit(*this);
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

void AstPrinter::visit_function_definition(FunctionDefinition &node) {
    std::string identifier = std::string("FunctionDefinition \"") + node.name->token.literal() + "\"";
    print_node(identifier.c_str(), node.tokens);
    std::cout << "(\n";
    increase_indentation();
    for (auto parameter = node.parameters.begin(); parameter != node.parameters.end(); parameter++) {
        parameter->get()->visit(*this);
    }
    std::cout << ")\n";
    node.block->visit(*this);
    decrease_indentation();
}

void AstPrinter::visit_block_stmt(Block &node) {
    print_node("Block", node.tokens);
    increase_indentation();
    for(auto stmt : node.statements) {
        stmt->visit(*this);
    }
    decrease_indentation();
}

void AstPrinter::visit_file(ast::File &node) {
    std::cout << "AST of file " << node.filename << ":\n";
    if (not node.functions.empty())
        std::cout << "Functions:\n\n";
    for (auto function = node.functions.begin(); function != node.functions.end(); function++) {
        function->get()->visit(*this);
        std::cout << "\n";
    }
    std::cout << "Root scope code:\n";
    node.code->visit(*this);
}

void Node::print() {
    AstPrinter printer;
    this->visit(printer);
}