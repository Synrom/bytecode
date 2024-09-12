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

void Function::print() {
    std::cout << "Bytecodes for Function " << name << ":\n";
    Code::print();
}

bool Method::has_return_value(const std::unique_ptr<Environment> &env) {
    auto buffer_size = parameters.size() + variables.size() + 1; // + 1 for self
    return env->stack.size() > buffer_size;
}

bool Function::has_return_value(const std::unique_ptr<Environment> &env) {
    auto buffer_size = parameters.size() + variables.size();
    return env->stack.size() > buffer_size;
}

void Method::print() {
    std::cout << "Bytecodes for Method " << name << ":\n";
    Code::print();
}

void ClassStruct::print() {
    std::cout << "Methods of Class " << name << ":\n";
    for(auto method: methods)
        method->print();
}

std::shared_ptr<Method> ClassStruct::find_method(std::string method_name) {
    for (auto method: methods) {
        if (method->name == method_name) {
            return method;
        }
    }
    std::cout << "Error: Cant find method " << method_name << " for class " << name << std::endl;
    return NULL;
}

std::shared_ptr<Method> ClassStruct::constructor() {
    return find_method(std::string("__init__"));
}

void PushVariable::execute(Environment &env) {
    Value v = env.stack[offset];
    env.push(v);
}

void Set::execute(Environment &env) {
    Value location = env.pop();
    Value v = env.pop();
    *location.location(env) = v;
}

void PushLValue::execute(Environment &env) {
    env.push(Value::create_stack_lvalue(offset));
}

void ObjectAccess::execute(Environment &env) {
    Value object = env.pop();
    env.push(object.object(env)->find_attribute(identifier));
}

void ObjectAccessLValue::execute(Environment &env) {
    Value object = env.pop();
    env.push(Value::create_heap_lvalue(object.object(env)->find_attribute_lvalue(identifier)));
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
    if (function->has_return_value(env.child))
        env.push(env.child->pop());
    /* delete env of called function*/
    env.child = NULL; 
}

void CallMethod::execute(Environment &env) {
    std::vector<Value> parameters;
    for(ssize_t idx_param = 0; idx_param < num_parameters; idx_param++) {
        parameters.push_back(env.pop());
    }
    std::shared_ptr<Object> obj = env.pop().object(env);
    std::shared_ptr<Method> method = obj->class_struct->find_method(name);

    env.child = std::unique_ptr<Environment>(new Environment());
    env.child->parent = &env;

    env.child->push(Value(obj));
    for(auto parameter = parameters.begin(); parameter != parameters.end(); parameter++) {
        env.child->push(*parameter);
    }
    for(long unsigned int i = 0;i < method->variables.size(); i++)
        env.child->stack.push_back(Value::create_void());

    for(auto bytecode = method->bytecodes.rbegin(); bytecode != method->bytecodes.rend(); bytecode++)
        bytecode->get()->execute(*env.child);
    
    if (method->has_return_value(env.child))
        env.push(env.child->pop());

    env.child = NULL; 
}

void CallConstructor::execute(Environment &env) {
    /* Init env of called function */
    env.child = std::unique_ptr<Environment>(new Environment());
    env.child->parent = &env;
    /* construct and push self object */
    env.child->push(runtime::Value(std::shared_ptr<runtime::Object>(new runtime::Object(class_struct))));
    /* push parameters */
    std::shared_ptr<runtime::Method> method = class_struct->constructor();
    for (unsigned int idx_param = 0; idx_param < method->parameters.size(); idx_param++)
        env.child->push(env.pop());
    /* init variables on stack */
    for(long unsigned int i = 0;i < method->variables.size(); i++)
        env.child->stack.push_back(Value::create_void());
    /* execute bytecode */
    for(auto bytecode = method->bytecodes.rbegin(); bytecode != method->bytecodes.rend(); bytecode++)
        bytecode->get()->execute(*env.child);

    /* push newly created object on stack */
    if (env.child->is_empty()) {
        std::cout << "Error: constructor has emptry stack!\n";
        return;
    }
    Value obj = env.child->stack.front();
    env.push(obj);
    /* delete env of called function*/
    env.child = NULL; 
}

void Environment::print_stack() {
    std::cout << "Stack:\n";
    for (auto v = stack.begin(); v != stack.end(); v++)
        std::cout << v->to_string() << std::endl;
}