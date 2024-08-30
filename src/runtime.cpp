#include "runtime/bytecode.hpp"
#include "runtime/code.hpp"
#include "runtime/environment.hpp"

using namespace runtime;

void BinaryOp::execute(Environment &env) {
    Value left = env.pop(); 
    Value right = env.pop(); 
    env.push(perform_op(left, right));
}

void PushValue::execute(Environment &env) {
    env.push(value);
}

Value Code::run() {
    Environment env;
    for(long unsigned int i = 0;i < variables.size(); i++)
        env.stack.push_back(Value::create_void());
    for(auto bytecode = bytecodes.rbegin(); bytecode != bytecodes.rend(); bytecode++)
        bytecode->get()->execute(env);
    if (!env.is_empty())
        return env.pop();
    return Value::create_void();
}

bool Environment::is_empty() {
    return stack.empty();
}

void Environment::push(Value v) {
    stack.push_back(v);
}

Value Environment::pop() {
    Value head = stack.back();
    stack.pop_back();
    return head;
}

void Code::print() {
    for(auto bytecode = bytecodes.rbegin(); bytecode != bytecodes.rend(); bytecode++)
        bytecode->get()->print();
}

void PushVariable::execute(Environment &env) {
    Value v = env.stack[offset];
    env.push(v);
}

void SetVariable::execute(Environment &env) {
    Value v = env.pop();
    env.stack[offset] = v;
}

void CallFunction::execute(Environment &env) {
    /* Init env of called function */
    env.child = std::unique_ptr<Environment>(new Environment());
    env.child->parent = &env;
    /* push parameters */
    for (unsigned int idx_param = 0; idx_param < function->parameters.size(); idx_param++)
        env.child->push(env.pop());
    /* init variables on stack */
    for(long unsigned int i = 0;i < function->variables.size(); i++)
        env.child->stack.push_back(Value::create_void());
    /* execute bytecode */
    for(auto bytecode = function->bytecodes.rbegin(); bytecode != function->bytecodes.rend(); bytecode++)
        bytecode->get()->execute(*env.child);
    /* check for return values*/
    if (!env.child->is_empty())
        env.push(env.child->pop());
    /* delete env of called function*/
    env.child = NULL; 
}

void Environment::print_stack() {
    std::cout << "Stack:\n";
    for (auto v = stack.begin(); v != stack.end(); v++)
        std::cout << v->to_string() << std::endl;
}