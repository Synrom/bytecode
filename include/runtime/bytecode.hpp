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
    Value perform_op(const Value &left, const Value &right) override {
        return Value::add(left, right);
    }
};

class Minus: public BinaryOp {
public:
    void print () {
        std::cout << "Minus" << std::endl;
    }
private:
    Value perform_op(const Value &left, const Value &right) override {
        return Value::minus(left, right);
    }
};

class Divide: public BinaryOp {
public:
    void print () {
        std::cout << "Divide" << std::endl;
    }
private:
    Value perform_op(const Value &left, const Value &right) override {
        return Value::div(left, right);
    }
};

class Multiply: public BinaryOp {
public:
    void print () {
        std::cout << "Multiply" << std::endl;
    }
private:
    Value perform_op(const Value &left, const Value &right) override {
        return Value::mul(left, right);
    }
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

class PushVariable: public Bytecode {
public:
    void execute(Environment &env) override;
    PushVariable(ssize_t offset) : offset(offset) {}
    void print () {
        std::cout << "PushVariable " << offset << std::endl;
    }

private:
    size_t offset;
};

class SetVariable: public Bytecode {
public:
    void execute(Environment &env) override;
    SetVariable(ssize_t offset) : offset(offset) {}
    void print () {
        std::cout << "SetVariable " << offset << std::endl;
    }

private:
    size_t offset;
};

}

#endif