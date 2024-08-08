#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>

namespace ast {

class Node{
public:
    Node (TokenRange tokens) : tokens(tokens) {}
    TokenRange tokens;
};

class Expression : public Node {
public:
    Expression (TokenRange tokens) : Node(tokens) {}
};

class BinaryOp : public Expression {
public:
    enum Operation {
        Mul,
        Div,
        Add,
        Min,
    };
    BinaryOp(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, Operation op, TokenRange tokens) : left(std::move(left)), right(std::move(right)), op(op), Expression(tokens) {}
    std::unique_ptr<Expression> left, right;
    Operation op;
};

class UnaryOp : public Expression {
public:
    UnaryOp(std::unique_ptr<Expression> expr, TokenRange tokens) : expr(std::move(expr)), Expression(tokens) {}
    std::unique_ptr<Expression> expr;
};

class Number : public Expression {
public:
    Number(const Token &token, TokenRange tokens) : token(token), Expression(tokens) {}
    const Token &token;
};

class String : public Expression {
public:
    String(const Token &token, TokenRange tokens) : token(token), Expression(tokens) {}
    const Token &token;
};

class Access: public Expression {
public:
    Access(TokenRange tokens) : Expression(tokens) {}
};

class Identifier : public Access {
public:
    Identifier(const Token &token, TokenRange tokens) : token(token), Access(tokens) {}
    const Token &token;
};

class ClassAccess : public Access {
public:
    ClassAccess(std::unique_ptr<Access> left, std::unique_ptr<Access> right, TokenRange tokens) : left(std::move(left)), right(std::move(right)), Access(tokens) {}
    std::unique_ptr<Access> left, right;
};

class IndexAccess : public Access {
public:
    IndexAccess(std::unique_ptr<Access> left, std::unique_ptr<Expression> index, TokenRange tokens) : left(std::move(left)), index(std::move(index)), Access(tokens) {}
    std::unique_ptr<Access> left;
    std::unique_ptr<Expression> index;

};

class FunctionCall : public Access {
public:
    FunctionCall(std::unique_ptr<Access> name, TokenRange tokens) : name(std::move(name)), Access(tokens) {}
    void add_parameter(std::unique_ptr<Expression> parameter);
    std::unique_ptr<Access> name;
    std::vector<std::unique_ptr<Expression>> parameters;
};

}

#endif