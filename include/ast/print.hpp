#ifndef AST_PRINT_H
#define AST_PRINT_H

#include <memory>

#include "ast/ast.hpp"

using namespace ast;

class AstPrinter : public ast::Visitor {
private:
    std::string indentation;
    void print_location(const TokenRange &location);
    void print_node(const char *name, const TokenRange &location);
    void print_literal(const Literal &node);
    void increase_indentation() {
        indentation += "  ";
    }
    void decrease_indentation() {
        indentation.erase(indentation.size()-2);
    }
public:
    AstPrinter() : indentation("") {}
    void print(const std::shared_ptr<ast::Node> &node);
    void visit_binary_op(BinaryOp &node) override;
    void visit_unary_op(UnaryOp &node) override;
    void visit_number(Number &node) override;
    void visit_string(String &node) override;
    void visit_identifier(Identifier &node) override;
    void visit_class_access(ClassAccess &node) override;
    void visit_index_access(IndexAccess &node) override;
    void visit_function_call(FunctionCall &node) override;
    void visit_return_stmt(Return &node) override;
    void visit_assign_stmt(Assign &node) override;
    void visit_block_stmt(ast::Block &) override;
    void visit_function_definition(ast::FunctionDefinition &) override;
    void visit_file(ast::File &) override;
};

#endif