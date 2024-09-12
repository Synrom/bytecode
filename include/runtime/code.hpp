#ifndef CODE_H
#define CODE_H

#include <vector>

#include "bytecode.hpp"
#include "value.hpp"
#include "ast/ast.hpp"

namespace runtime {

class Function;
class ClassStruct;

class Code {
private:
public:
    std::vector<std::unique_ptr<Bytecode>> bytecodes;
    std::vector<std::string> variables;

    Code(std::vector<std::unique_ptr<Bytecode>> && bytecodes) : bytecodes(std::move(bytecodes)) {}
    Code() = default;

    Value run();
    virtual void print();

    ssize_t variable_offset_or_create(std::shared_ptr<ast::Identifier> );
    virtual ssize_t variable_offset_or_create(const ast::Identifier &);
};

class Function: public Code {
public:
    std::vector<std::string> parameters;
    std::string name;

    void print() override;
    bool has_return_value(const std::unique_ptr<Environment> &);

    Function(const ast::FunctionDefinition &ast) : Code(), name(ast.name->token.literal()) {
        for (auto ast_param = ast.parameters.begin(); ast_param != ast.parameters.end(); ast_param++) {
            parameters.push_back(ast_param->get()->token.literal());
        }
    }

    ssize_t variable_offset_or_create(const ast::Identifier &) override;
};

class Method: public Code {
public:
    std::vector<std::string> parameters;
    std::string name;
    std::shared_ptr<ClassStruct> class_struct;

    void print() override;
    bool has_return_value(const std::unique_ptr<Environment> &);

    Method(const ast::MethodDefinition &ast, std::shared_ptr<ClassStruct> class_) : Code(), name(ast.name->token.literal()), class_struct(class_) {
        if (ast.parameters.front()->token.literal() != std::string("self")) {
            std::cout << "Error: Method " << name << " must start with self parameter\n";
        }
        for (auto ast_param = std::next(ast.parameters.begin()); ast_param != ast.parameters.end(); ast_param++) {
            parameters.push_back(ast_param->get()->token.literal());
        }
    }
    ssize_t variable_offset_or_create(const ast::Identifier &) override;
};

class ClassStruct: public Code {
public:
    std::vector<std::shared_ptr<Method>> methods;
    std::string name;

    void print() override;

    ClassStruct(const ast::ClassDefinition &ast) : Code(), name(ast.name) {}
    std::shared_ptr<Method> constructor();
    std::shared_ptr<Method> find_method(std::string method_name);
};

class CallFunction: public Bytecode {
public:
    void execute(Environment &env) override;
    CallFunction(std::shared_ptr<Function> function): function(function) {}
    void print () {
        std::cout << "CallFunction " << function->name << std::endl;
    }
private:
    std::shared_ptr<Function> function;
};

class CallMethod: public Bytecode {
public:
    void execute(Environment &env) override;
    CallMethod(std::string name, ssize_t num_parameters): num_parameters(num_parameters), name(name) {}
    void print () {
        std::cout << "CallMethod " << name << std::endl;
    }
private:
    ssize_t num_parameters;
    std::string name;
};

class CallConstructor : public Bytecode {
public:
    void execute(Environment &env) override;
    CallConstructor(std::shared_ptr<runtime::ClassStruct> class_struct) : class_struct(class_struct) {}
    void print() {
        std::cout << "CallConstructor " << class_struct->name << std::endl;
    }
private:
    std::shared_ptr<runtime::ClassStruct> class_struct;
};
}

#endif