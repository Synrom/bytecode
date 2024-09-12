#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <variant>
#include <unordered_map>

namespace runtime {

class Object;
class Environment;
class ClassStruct;

class Value {
private:
    enum Type {
        String,
        Number,
        Object,
        StackLValue,
        HeapLValue,
        Void 
    };
    using ValueType = std::variant<std::string, float, std::shared_ptr<runtime::Object>, Value *, ssize_t>;
    ValueType value;
    Value(ValueType value, Type type) : value(value), type(type) {}
public:
    Type type;
    Value(float f) : value(f), type(Number) {}
    Value(std::string s) : value(s), type(String) {}
    Value(std::shared_ptr<runtime::Object> obj) : value(obj), type(Object) {}

    Value *location(Environment &env);
    std::shared_ptr<runtime::Object> object(Environment &env);
    std::string to_string() const; // include Lvalues and Objects here
    static Value add(const Value &a, const Value &b);
    static Value minus(const Value &a, const Value &b);
    static Value div(const Value &a, const Value &b);
    static Value mul(const Value &a, const Value &b);
    static Value create_void();
    static Value create_stack_lvalue(ssize_t offset);
    static Value create_heap_lvalue(Value *);
};

class Object {
public:
    std::shared_ptr<runtime::ClassStruct> class_struct;
    std::unordered_map<std::string, Value> attributes;
    Value find_attribute(std::string name);
    Value *find_attribute_lvalue(std::string name);
    std::string to_string() const;
    Object(std::shared_ptr<runtime::ClassStruct> class_) : class_struct(class_) {}
};

}

#endif