#ifndef AST_H
#define AST_H

#include "lexer/tokens.hpp"

#include <memory>
#include <vector>

namespace ast {

class BinaryOp;
class UnaryOp;
class Number;
class String;
class Identifier;
class ClassAccess;
class IndexAccess;
class FunctionCall;

class Visitor{
public:
    virtual void visit_binary_op(BinaryOp &) {}
    virtual void visit_unary_op(UnaryOp &) {}
    virtual void visit_number(Number &) {}
    virtual void visit_string(String &) {}
    virtual void visit_identifier(Identifier &) {}
    virtual void visit_class_access(ClassAccess &) {}
    virtual void visit_index_access(IndexAccess &) {}
    virtual void visit_function_call(FunctionCall &) {}
};

class Node{
public:
    Node (TokenRange tokens) : tokens(tokens), parent(NULL) {}
    Node (TokenRange tokens, std::shared_ptr<Node> parent) : tokens(tokens), parent(parent) {}
    TokenRange tokens;
    std::shared_ptr<Node> parent;

    virtual void visit(Visitor &) = 0;
    void print();
};

class Expression : public Node {
public:
    Expression (TokenRange tokens) : Node(tokens) {}
    Expression (TokenRange tokens, std::shared_ptr<Node> parent) : Node(tokens, parent) {}

};

class BinaryOp : public Expression {
public:
    enum Operation {
        Mul,
        Div,
        Add,
        Min,
    };

    static std::shared_ptr<BinaryOp> create(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, Operation op, TokenRange tokens, std::shared_ptr<Node> parent);
    std::shared_ptr<Expression> left, right;
    Operation op;

    void visit(Visitor &visitor) override {
        visitor.visit_binary_op(*this);
    }

    std::string string_op();

private:
    BinaryOp(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, Operation op, TokenRange tokens, std::shared_ptr<Node> parent) : Expression(tokens, parent), left(left), right(right), op(op) {}
};

class UnaryOp : public Expression {
public:
    static std::shared_ptr<UnaryOp> create(std::shared_ptr<Expression> expr, TokenRange tokens, std::shared_ptr<Node> parent);
    std::shared_ptr<Expression> expr;

    void visit(Visitor &visitor) override {
        visitor.visit_unary_op(*this);
    }

private:
    UnaryOp(std::shared_ptr<Expression> expr, TokenRange tokens, std::shared_ptr<Node> parent) :  Expression(tokens, parent), expr(expr) {}
};

class Literal: public Expression{
public:
    const Token &token;
    Literal(const Token &token, TokenRange tokens, std::shared_ptr<Node> parent) :  Expression(tokens, parent), token(token) {}

};

class Number : public Literal {
public:
    Number(const Token &token, TokenRange tokens, std::shared_ptr<Node> parent) : Literal(token, tokens, parent) {}
    int number;

    void visit(Visitor &visitor) override {
        visitor.visit_number(*this);
    }
};

class String : public Literal {
public:
    String(const Token &token, TokenRange tokens, std::shared_ptr<Node> parent) : Literal(token, tokens, parent) {}

    void visit(Visitor &visitor) override {
        visitor.visit_string(*this);
    }
};

class Access: public Expression {
public:
    Access(TokenRange tokens, std::shared_ptr<Node> parent) : Expression(tokens, parent) {}

};

class Identifier : public Access, public Literal {
public:
    Identifier(const Token &token, TokenRange tokens, std::shared_ptr<Node> parent) : Access(tokens, parent), Literal(token, tokens, parent) {}

    void visit(Visitor &visitor) override {
        visitor.visit_identifier(*this);
    }
};

class ClassAccess : public Access {
public:
    static std::shared_ptr<ClassAccess> create(std::shared_ptr<Access> left, std::shared_ptr<Access> right, TokenRange tokens, std::shared_ptr<Node> parent);
    std::shared_ptr<Access> left, right;

    void visit(Visitor &visitor) override {
        visitor.visit_class_access(*this);
    }
private:
    ClassAccess(std::shared_ptr<Access> left, std::shared_ptr<Access> right, TokenRange tokens, std::shared_ptr<Node> parent) : Access(tokens, parent), left(left), right(right) {}
};

class IndexAccess : public Access {
public:
    static std::shared_ptr<IndexAccess> create(std::shared_ptr<Access> left, std::shared_ptr<Expression> index, TokenRange tokens, std::shared_ptr<Node> parent);
    std::shared_ptr<Access> left;
    std::shared_ptr<Expression> index;

    void visit(Visitor &visitor) override {
        visitor.visit_index_access(*this);
    }

private:
    IndexAccess(std::shared_ptr<Access> left, std::shared_ptr<Expression> index, TokenRange tokens, std::shared_ptr<Node> parent) : Access(tokens,parent), left(left), index(index) {}
};

class FunctionCall : public Access {
public:
    static std::shared_ptr<FunctionCall> create(std::shared_ptr<Access> name, TokenRange tokens, std::shared_ptr<Node> parent, std::vector<std::shared_ptr<Expression>> parameters);

    std::shared_ptr<Access> name;
    std::vector<std::shared_ptr<Expression>> parameters;

    void visit(Visitor &visitor) override {
        visitor.visit_function_call(*this);
    }

private:
    FunctionCall(std::shared_ptr<Access> name, TokenRange tokens, std::shared_ptr<Node> parent, std::vector<std::shared_ptr<Expression>> parameters) : Access(tokens, parent), name(name), parameters(parameters) {}
};

}

#endif