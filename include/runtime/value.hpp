#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <variant>

namespace runtime {

class Value {
private:
    enum Type {
        String,
        Number,
        Void 
    };
    std::variant<std::string, float> value;
public:
    Type type;
    Value(float f) : value(f), type(Number) {}
    Value(std::string s) : value(s), type(String) {}
    Value(std::variant<std::string, float> value, Type type) : value(value), type(type) {}

    std::string to_string() const;
    static Value add(const Value &a, const Value &b);
    static Value minus(const Value &a, const Value &b);
    static Value div(const Value &a, const Value &b);
    static Value mul(const Value &a, const Value &b);
    static Value create_void();
};

}

#endif