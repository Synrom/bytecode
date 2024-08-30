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
            current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Add()));
            break;
        case ast::BinaryOp::Min:
            current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Minus()));
            break;
        case ast::BinaryOp::Div:
            current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Divide()));
            break;
        case ast::BinaryOp::Mul:
            current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Multiply()));
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
    ssize_t var_offset = current->variable_offset_or_create(identifier);
    current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::SetVariable(var_offset)));
    node.expr->visit(*this);
}

void BytecodeCompiler::visit_return_stmt(ast::Return &node) {
    node.expr->visit(*this);
}

void BytecodeCompiler::visit_identifier(ast::Identifier &node) {
    current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::PushVariable(current->variable_offset_or_create(node))));
}

void BytecodeCompiler::visit_number(ast::Number &number) {
    current->bytecodes.push_back(std::unique_ptr<runtime::PushValue>(new runtime::PushValue(runtime::Value(number.number))));
}

std::shared_ptr<runtime::Code> BytecodeCompiler::code() {
    return compilation_stack.front();
}

ssize_t runtime::Code::variable_offset_or_create(std::shared_ptr<ast::Identifier> node) {
    return variable_offset_or_create(*node.get());
}

ssize_t runtime::Code::variable_offset_or_create(const ast::Identifier &node) {
    std::string identifier = node.token.literal();
    auto it = std::find(variables.begin(), variables.end(), identifier);

    if(it != variables.end()) {
        return std::distance(variables.begin(), it);
    } else {
        variables.push_back(identifier);
        return variables.size() - 1;
    }
}

ssize_t runtime::Function::variable_offset_or_create(const ast::Identifier &node) {
    std::string identifier = node.token.literal();

    /* First look for identifier in parameters */
    auto pit = std::find(parameters.begin(), parameters.end(), identifier);
    if(pit != parameters.end()) {
        return std::distance(parameters.begin(), pit);
    } 

    /* Then look for identifier in variables */
    auto vit = std::find(variables.begin(), variables.end(), identifier);
    if(vit != variables.end()) {
        return std::distance(variables.begin(), vit) + parameters.size();
    } 

    variables.push_back(identifier);
    return variables.size() - 1 + parameters.size();
}

void BytecodeCompiler::visit_block_stmt(ast::Block &block) {
    for(auto stmt = block.statements.rbegin(); stmt != block.statements.rend(); stmt++) {
        stmt->get()->visit(*this);
    }
}

void BytecodeCompiler::visit_file(ast::File &file) {
    for(auto function = file.functions.begin(); function != file.functions.end(); function++) {
        function->get()->visit(*this);
    }
    file.code->visit(*this);
}

void BytecodeCompiler::visit_function_definition(ast::FunctionDefinition &ast_func) {
    std::shared_ptr<runtime::Function> function = std::shared_ptr<runtime::Function>(new runtime::Function(ast_func));
    functions.push_back(function);
    std::shared_ptr<runtime::Code> copy_current = current;
    current = function;
    ast_func.block->visit(*this);
    current = copy_current;
}

void BytecodeCompiler::visit_function_call(ast::FunctionCall &call) {
    std::shared_ptr<ast::Identifier> fname = std::dynamic_pointer_cast<ast::Identifier>(call.name);
    if (fname == NULL) {
        std::cout << "Error: methods are not suppoerted yet\n";
        return;
    }
    std::shared_ptr<runtime::Function> func = find_function(call);
    if (func == NULL) {
        std::cout << "Error: function " << fname << " was used before it was defined\n";
        return;
    }
    current->bytecodes.push_back(std::unique_ptr<runtime::CallFunction>(new runtime::CallFunction(func)));
    for (auto parameter = call.parameters.begin(); parameter != call.parameters.end(); parameter++) {
        parameter->get()->visit(*this);
    }
}

std::shared_ptr<runtime::Function> BytecodeCompiler::find_function(ast::FunctionCall &func) {
    std::shared_ptr<ast::Identifier> fname = std::dynamic_pointer_cast<ast::Identifier>(func.name);
    if (fname == NULL) {
        std::cout << "Error: methods are not suppoerted yet\n";
        return NULL;
    }
    for (auto it = functions.begin(); it != functions.end(); it++) {
        if (it->get()->name == fname->token.literal()) 
            return *it;
    }
    return NULL;
}