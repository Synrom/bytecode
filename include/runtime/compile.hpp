#ifndef COMPILE_RUNTIME_H
#define COMPILE_RUNTIME_H

#include "ast/ast.hpp"
#include "runtime/code.hpp"

class BytecodeCompiler: public ast::Visitor {
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
    void visit_class_definition(ast::ClassDefinition &) override;
    void visit_method_definition(ast::MethodDefinition &) override;
    void visit_class_access(ast::ClassAccess &) override;

    void visit_constructor(ast::FunctionCall &, std::string);

    std::shared_ptr<runtime::Function> find_function(ast::FunctionCall &);
    std::shared_ptr<runtime::ClassStruct> find_class(const std::string &);
    void activate_lvalue();
    void deactivate_lvalue();
    bool lvalue;
    void activate_class_access() {
        class_access++;
    }
    void deactivate_class_access() {
        class_access--;
    }
    unsigned int class_access;
public:
    std::vector<std::shared_ptr<runtime::Function>> functions;
    std::vector<std::shared_ptr<runtime::ClassStruct>> classes;
    std::shared_ptr<runtime::Code> code();
    BytecodeCompiler() : current(std::shared_ptr<runtime::Code>(new runtime::Code())), lvalue(false), class_access(0) {}
};

#endif