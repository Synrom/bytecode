#ifndef CODE_H
#define CODE_H

#include <vector>

#include "bytecode.hpp"
#include "value.hpp"
#include "ast/ast.hpp"

namespace runtime {

class Code {
private:
public:
    std::vector<std::unique_ptr<Bytecode>> bytecodes;
    std::vector<std::string> variables;

    Code(std::vector<std::unique_ptr<Bytecode>> && bytecodes) : bytecodes(std::move(bytecodes)) {}
    Code() = default;

    Value run();
    void print();

    ssize_t variable_offset_or_create(std::shared_ptr<ast::Identifier> );
    virtual ssize_t variable_offset_or_create(const ast::Identifier &);

};

class Function: public Code {
public:
    std::vector<std::string> parameters;
    std::string name;

    Function(const ast::FunctionDefinition &ast) : Code(), name(ast.name->token.literal()) {
        for (auto ast_param = ast.parameters.begin(); ast_param != ast.parameters.end(); ast_param++) {
            parameters.push_back(ast_param->get()->token.literal());
        }
    }

    ssize_t variable_offset_or_create(const ast::Identifier &) override;
};

class CallFunction: public Bytecode {
public:
    void execute(Environment &env) override;
    CallFunction(std::shared_ptr<Function> function): function(function) {}
    std::shared_ptr<Function> function;
    void print () {
        std::cout << "CallFunction " << function->name << std::endl;
    }
};
}

#endif