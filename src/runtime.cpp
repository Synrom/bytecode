#include "runtime/bytecode.hpp"
#include "runtime/code.hpp"
#include "runtime/environment.hpp"

using namespace runtime;

void BinaryOp::execute(Environment &env) {
    Value left = env.pop(); 
    Value right = env.pop(); 
    env.push(perform_op(left, right));
}

Value Add::perform_op(const Value &left, const Value &right) {
    return Value::add(left, right);
}

void PushValue::execute(Environment &env) {
    env.push(value);
}

Value Code::run() {
    Environment env;
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