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
    for(int i = 0;i < num_variables; i++)
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