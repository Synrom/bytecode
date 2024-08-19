#include "runtime/value.hpp"

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
        default:
            std::cout << "Error: Unknown type" << std::endl;
    }
    return str;
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
