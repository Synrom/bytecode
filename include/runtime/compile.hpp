#ifndef COMPILE_RUNTIME_H
#define COMPILE_RUNTIME_H

#include "ast/ast.hpp"
#include "runtime/code.hpp"

class BytecodeCompiler: public ast::Visitor {
    std::vector<std::unique_ptr<runtime::Bytecode>> bytecode;
    void visit_binary_op(ast::BinaryOp &) override;
    void visit_number(ast::Number &) override;
public:
    runtime::Code code();
};

#endif