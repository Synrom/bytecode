#include "runtime/compile.hpp"
#include "ast/ast.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

void BytecodeCompiler::visit_binary_op(ast::BinaryOp &op) {
    switch (op.op)
    {
        case ast::BinaryOp::Add:
            bytecode.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Add()));
            break;
        case ast::BinaryOp::Min:
            bytecode.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Minus()));
            break;
        case ast::BinaryOp::Div:
            bytecode.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Divide()));
            break;
        case ast::BinaryOp::Mul:
            bytecode.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Multiply()));
            break;
        default:
            std::cout << "Error: Can't compile " << op.string_op() << " to bytecode\n";
    }
    op.left->visit(*this);
    op.right->visit(*this);
}
void BytecodeCompiler::visit_assign_stmt(ast::Assign &node) {
    std::shared_ptr<ast::Identifier> identifier = std::dynamic_pointer_cast<ast::Identifier>(node.location);
    if (identifier == NULL) {
        std::cout << "Error: Can't compile identifier accesses yet\n";
        return;
    }
    ssize_t var_offset = variable_offset_or_create(identifier);
    bytecode.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::SetVariable(var_offset)));
    node.expr->visit(*this);
}

void BytecodeCompiler::visit_return_stmt(ast::Return &node) {
    node.expr->visit(*this);
}

void BytecodeCompiler::visit_identifier(ast::Identifier &node) {
    bytecode.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::PushVariable(variable_offset_or_create(node))));
}

void BytecodeCompiler::visit_number(ast::Number &number) {
    bytecode.push_back(std::unique_ptr<runtime::PushValue>(new runtime::PushValue(runtime::Value(number.number))));
}

runtime::Code BytecodeCompiler::code() {
    return runtime::Code(std::move(bytecode), variables.size());
}

ssize_t BytecodeCompiler::variable_offset_or_create(std::shared_ptr<ast::Identifier> node) {
    return variable_offset_or_create(*node.get());
}

ssize_t BytecodeCompiler::variable_offset_or_create(const ast::Identifier &node) {
    std::string identifier = node.token.literal();
    auto it = std::find(variables.begin(), variables.end(), identifier);

    if(it != variables.end()) {
        return std::distance(variables.begin(), it);
    } else {
        variables.push_back(identifier);
        return variables.size() - 1;
    }
}

void BytecodeCompiler::compile(std::vector<std::shared_ptr<ast::Statement>> block) {
    for(auto stmt = block.rbegin(); stmt != block.rend(); stmt++) {
        stmt->get()->visit(*this);
    }
}