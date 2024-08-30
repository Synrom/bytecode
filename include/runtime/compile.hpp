#ifndef COMPILE_RUNTIME_H
#define COMPILE_RUNTIME_H

#include "ast/ast.hpp"
#include "runtime/code.hpp"

class BytecodeCompiler: public ast::Visitor {
    std::vector<std::shared_ptr<runtime::Code>> compilation_stack;
    std::shared_ptr<runtime::Code> current;

    void visit_binary_op(ast::BinaryOp &) override;
    void visit_number(ast::Number &) override;
    void visit_assign_stmt(ast::Assign &) override;
    void visit_return_stmt(ast::Return &) override;
    void visit_identifier(ast::Identifier &) override;
    void visit_block_stmt(ast::Block &) override;
    void visit_function_definition(ast::FunctionDefinition &) override;
    void visit_file(ast::File &) override;
    void visit_function_call(ast::FunctionCall &) override;

    std::shared_ptr<runtime::Function> find_function(ast::FunctionCall &);
public:
    std::vector<std::shared_ptr<runtime::Function>> functions;
    std::shared_ptr<runtime::Code> code();
    BytecodeCompiler() : current(std::shared_ptr<runtime::Code>(new runtime::Code())) {
        compilation_stack.push_back(current);
    }
};

#endif