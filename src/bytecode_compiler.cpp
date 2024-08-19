#include "runtime/compile.hpp"
#include "ast/ast.hpp"

#include <iostream>

void BytecodeCompiler::visit_binary_op(ast::BinaryOp &op) {
    switch (op.op)
    {
        case ast::BinaryOp::Add:
            bytecode.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Add()));
            break;
        default:
            std::cout << "Error: Can't compile " << op.string_op() << " to bytecode\n";
    }
    op.left->visit(*this);
    op.right->visit(*this);
}

void BytecodeCompiler::visit_number(ast::Number &number) {
    bytecode.push_back(std::unique_ptr<runtime::PushValue>(new runtime::PushValue(runtime::Value(number.number))));
}

runtime::Code BytecodeCompiler::code() {
    return runtime::Code(std::move(bytecode));
}