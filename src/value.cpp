#include "runtime/value.hpp"
#include "runtime/environment.hpp"
#include "runtime/code.hpp"

#include <iostream>
#include <string>
#include <variant>

using namespace runtime;

std::string Value::to_string() const {
    std::string str;
    switch (type) {
        case Void: str = "Void"; break;
        case Number: 
            str = "Number " + std::to_string(std::get<float>(value)); 
            break;
        case String: 
            str = "String " + std::get<std::string>(value);
            break;
        case StackLValue:
            str = "StackLValue %" + std::get<ssize_t>(value);
            break;
        case HeapLValue:
            str = "HeapLValue " + std::get<Value *>(value)->to_string();
            break;
        case Object:
            str = std::get<std::shared_ptr<runtime::Object>>(value)->to_string();
            break;
        default:
            std::cout << "Error: Unknown type" << std::endl;
    }
    return str;
}

Value *Value::location(Environment &env) {
    if (type == StackLValue) {
        return &env.stack[std::get<ssize_t>(value)];
    } else if(type == HeapLValue) {
        return std::get<Value *>(value);
    } else {
        std::cout << "Error: lvalue is neither StackLValue nor HeapLValue\n";
    }
    return NULL;
}

Value Value::create_stack_lvalue(ssize_t offset) {
    return Value(offset, StackLValue);
}

Value Value::create_heap_lvalue(Value *pointer) {
    return Value(pointer, HeapLValue);
}

std::shared_ptr<Object> Value::object(Environment &env) {
    if (type == StackLValue or type == HeapLValue) {
        Value *value = location(env);
        return value->object(env);
    }
    if (type != Value::Object) {
        std::cout << "Error: Trying to access attribute of " << to_string() << std::endl;
        return NULL;
    }
    return std::get<std::shared_ptr<runtime::Object>>(value);
}

Value Value::add(const Value &a, const Value &b) {
    if (a.type == Number and b.type == Number)
        return Value(std::get<float>(a.value) + std::get<float>(b.value));
    if (a.type == String and b.type == String)
        return Value(std::get<std::string>(a.value) + std::get<std::string>(b.value));
    std::cout << "Error: trying to add " << a.to_string() << " and " << b.to_string() << std::endl;
    return Value::create_void();
}

Value Value::minus(const Value &a, const Value &b) {
    if (a.type == Number and b.type == Number)
        return Value(std::get<float>(a.value) - std::get<float>(b.value));
    std::cout << "Error: trying to minus " << a.to_string() << " and " << b.to_string() << std::endl;
    return Value::create_void();
}

Value Value::div(const Value &a, const Value &b) {
    if (a.type == Number and b.type == Number)
        return Value(std::get<float>(a.value) / std::get<float>(b.value));
    std::cout << "Error: trying to divide " << a.to_string() << " and " << b.to_string() << std::endl;
    return Value::create_void();
}

Value Value::mul(const Value &a, const Value &b) {
    if (a.type == Number and b.type == Number)
        return Value(std::get<float>(a.value) * std::get<float>(b.value));
    std::cout << "Error: trying to multiply " << a.to_string() << " and " << b.to_string() << std::endl;
    return Value::create_void();
}

Value Value::create_void() {
    return Value(0.0f, Void);
}


Value runtime::Object::find_attribute(std::string name) {
    auto entry = attributes.find(name);
    if (entry != attributes.end())
        return entry->second;
    return Value::create_void();
}


Value *runtime::Object::find_attribute_lvalue(std::string name) {
   auto entry = attributes.find(name);
    if (entry != attributes.end())
        return &entry->second;
    auto result = attributes.insert({name, Value::create_void()});
    if (result.second) {
        return &result.first->second;
    }
    std::cout << "Error: failed to insert entry for " << name << " into object\n";
    return NULL;
}

std::string runtime::Object::to_string() const {
    std::string str = class_struct->name +  "(";
    for (auto it = attributes.begin(); it != attributes.end(); it++) {
        str += it->first + "=" + it->second.to_string();
        if (std::next(it) != attributes.end()) {
            str += ", ";
        }
    }
    str += ")";
    return str;
}