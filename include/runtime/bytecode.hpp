#ifndef BYTECODE_H
#define BYTECODE_H

#include "environment.hpp"

#include <iostream>

namespace runtime {

class Bytecode{
public:
    virtual void execute(Environment &env) = 0;
    virtual void print() = 0;
};

class BinaryOp: public Bytecode{
public:
    void execute(Environment &env) override;
private:
    virtual Value perform_op(const Value &left, const Value &right) = 0;
};

class Add: public BinaryOp {
public:
    void print () {
        std::cout << "Add" << std::endl;
    }
private:
    Value perform_op(const Value &left, const Value &right) override;
};

class PushValue: public Bytecode {
public:
    void execute(Environment &env) override;
    PushValue(Value value) : value(value) {}
    void print () {
        std::cout << "PushValue " << value.to_string() << std::endl;
    }
private:
    Value value;
};

}

#endif