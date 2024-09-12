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
    current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::Set()));
    activate_lvalue();
    node.location->visit(*this);
    deactivate_lvalue();
    node.expr->visit(*this);
}


void BytecodeCompiler::visit_return_stmt(ast::Return &node) {
    node.expr->visit(*this);
}

void BytecodeCompiler::visit_identifier(ast::Identifier &node) {
    if (lvalue) {
        if (class_access) {
            current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::ObjectAccessLValue(node)));
        } else {
            ssize_t var_offset = current->variable_offset_or_create(node);
            current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::PushLValue(var_offset)));
        }
    } else if (class_access) {
        current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::ObjectAccess(node)));
    } else
        current->bytecodes.push_back(std::unique_ptr<runtime::Bytecode>(new runtime::PushVariable(current->variable_offset_or_create(node))));
}

void BytecodeCompiler::visit_class_access(ast::ClassAccess &node) {
    activate_class_access();
    node.right->visit(*this); 
    deactivate_class_access();
    node.left->visit(*this); 
}

void BytecodeCompiler::activate_lvalue() {
    lvalue = true;
}

void BytecodeCompiler::deactivate_lvalue() {
    lvalue = false;
}

void BytecodeCompiler::visit_number(ast::Number &number) {
    current->bytecodes.push_back(std::unique_ptr<runtime::PushValue>(new runtime::PushValue(runtime::Value(number.number))));
}

std::shared_ptr<runtime::Code> BytecodeCompiler::code() {
    return current;
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
ssize_t runtime::Method::variable_offset_or_create(const ast::Identifier &node) {
    std::string identifier = node.token.literal();

    if (identifier == std::string("self"))
        return 0;

    /* First look for identifier in parameters */
    auto pit = std::find(parameters.begin(), parameters.end(), identifier);
    if(pit != parameters.end()) {
        return std::distance(parameters.begin(), pit) + 1;
    } 

    /* Then look for identifier in variables */
    auto vit = std::find(variables.begin(), variables.end(), identifier);
    if(vit != variables.end()) {
        return std::distance(variables.begin(), vit) + parameters.size() + 1;
    } 

    variables.push_back(identifier);
    return variables.size() + parameters.size();
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
    for(auto class_ = file.classes.begin(); class_ != file.classes.end(); class_++) {
        class_->get()->visit(*this);
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
    if (class_access) {
        current->bytecodes.push_back(std::unique_ptr<runtime::CallMethod>(new runtime::CallMethod(call.name->token.literal(), call.parameters.size())));
    } else {
        std::shared_ptr<runtime::Function> func = find_function(call);
        if (func == NULL) {
            visit_constructor(call, call.name->token.literal());
            return;
        }
        current->bytecodes.push_back(std::unique_ptr<runtime::CallFunction>(new runtime::CallFunction(func)));
    }
    for (auto parameter = call.parameters.begin(); parameter != call.parameters.end(); parameter++) {
        parameter->get()->visit(*this);
    }
}

void BytecodeCompiler::visit_constructor(ast::FunctionCall &call, std::string name) {
    std::shared_ptr<runtime::ClassStruct> class_struct = find_class(name);
    if (class_struct == NULL)  {
        std::cout << "Error: function " << name << " was used before it was defined\n";
        return;
    }
    current->bytecodes.push_back(std::unique_ptr<runtime::CallConstructor>(new runtime::CallConstructor(class_struct)));
    for (auto parameter = call.parameters.begin(); parameter != call.parameters.end(); parameter++) {
        parameter->get()->visit(*this);
    }
}

void BytecodeCompiler::visit_class_definition(ast::ClassDefinition &class_definition) {
    std::shared_ptr<runtime::ClassStruct> class_struct = std::shared_ptr<runtime::ClassStruct>(new runtime::ClassStruct(class_definition));
    std::shared_ptr<runtime::Code> copy_current = current;
    current = class_struct;
    classes.push_back(class_struct);
    for (auto method : class_definition.methods) {
        method->visit(*this);
    }
    current = copy_current;
}

void BytecodeCompiler::visit_method_definition(ast::MethodDefinition &method_definition) {
    std::shared_ptr<runtime::ClassStruct> class_struct = std::dynamic_pointer_cast<runtime::ClassStruct>(current);
    if (class_struct == NULL) {
        std::cout << "Error: method " << method_definition.name->token.literal() << " is not in class scope\n";
        return;
    }
    std::shared_ptr<runtime::Method> method = std::shared_ptr<runtime::Method>(new runtime::Method(method_definition, class_struct));
    class_struct->methods.push_back(method);
    std::shared_ptr<runtime::Code> copy_current = current;
    current = method;
    method_definition.block->visit(*this);
    current = copy_current;
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

std::shared_ptr<runtime::ClassStruct> BytecodeCompiler::find_class(const std::string &name) {
    for (auto it = classes.begin(); it != classes.end(); it++) {
        if (it->get()->name == name) 
            return *it;
    }
    return NULL;

}