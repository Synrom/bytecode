#ifndef COMPILE_RUNTIME_H
#define COMPILE_RUNTIME_H

#include "ast/ast.hpp"
#include "runtime/code.hpp"

class BytecodeCompiler: public ast::Visitor {
    std::vector<std::string> variables;
    std::vector<std::unique_ptr<runtime::Bytecode>> bytecode;
    void visit_binary_op(ast::BinaryOp &) override;
    void visit_number(ast::Number &) override;
    void visit_assign_stmt(ast::Assign &) override;
    void visit_return_stmt(ast::Return &) override;
    void visit_identifier(ast::Identifier &) override;
    ssize_t variable_offset_or_create(std::shared_ptr<ast::Identifier> );
    ssize_t variable_offset_or_create(const ast::Identifier &);
public:
    void compile(std::vector<std::shared_ptr<ast::Statement>> );
    runtime::Code code();
};

#endif