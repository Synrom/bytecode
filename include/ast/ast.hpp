#ifndef AST_H
#define AST_H

#include <memory>

namespace ast {

class Node{
public:
    TokenRange tokens;
};

class Expression : public Node {
public:
};

class BinaryOp : public Expression {
public:
    enum Operation {
        Mul,
        Div,
        Add,
        Min,
    };
    BinaryOp(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, Operation op, TokenRange tokens);
    std::unique_ptr<Expression> left, right;
    Operation op;
};

class UnaryOp : public Expression {
public:
    UnaryOp(std::unique_ptr<Expression> expr);
};

class Number : public Expression {
public:
    Number(const Token &token);
};

class String : public Expression {
public:
    String(const Token &token);
};

class Identifier : public Expression {
public:
    Identifier(const Token &token);
};

class ClassAccess : public Identifier {
public:
    ClassAccess(std::unique_ptr<Identifier> identifier, std::unique_ptr<Identifier> chain);
};

class IndexAccess : public Identifier {
public:
    IndexAccess(std::unique_ptr<Identifier> identifier, std::unique_ptr<Expression> index);
};

class FunctionCall : public Identifier {
public:
    FunctionCall();
    void add_parameter(std::unique_ptr<Expression> parameter);
};

}

#endif