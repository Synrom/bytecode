#ifndef AST_H
#define AST_H

#include "lexer/tokens.hpp"
#include "runtime/bytecode.hpp"

#include <memory>
#include <vector>

#define ACCESS_PRESEDENCDE 1
#define LITERAL_PRESEDENCDE 1
#define UNARY_OP_PRESEDENCE 2
#define FACTOR_PRESEDENCE 3
#define SUM_PRESEDENCE 3

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
    Node (TokenRange tokens, std::shared_ptr<Node> parent, unsigned int presedence) : tokens(tokens), parent(parent), presedence(presedence) {}
    TokenRange tokens;
    std::shared_ptr<Node> parent;
    unsigned int presedence;

    virtual void visit(Visitor &) = 0;
    virtual std::shared_ptr<Node> get_left_child() = 0;
    virtual std::shared_ptr<Node> get_right_child() = 0;
    virtual void set_left_child(std::shared_ptr<Node> ) {
        std::cout << "Error: Trying to set left child of terminal node\n";
    }
    virtual void set_right_child(std::shared_ptr<Node> ) {
        std::cout << "Error: Trying to set right child of terminal node\n";
    }
    void print();
};

class Expression : public Node {
public:
    Expression (TokenRange tokens) : Node(tokens) {}
    Expression (TokenRange tokens, std::shared_ptr<Node> parent, unsigned int presedence) : Node(tokens, parent, presedence) {}

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

    std::shared_ptr<Node> get_left_child() override {
        return left;
    }
    std::shared_ptr<Node> get_right_child() override {
        return right;
    }
    void set_left_child(std::shared_ptr<Node> node) override {
        left = std::dynamic_pointer_cast<Expression>(node);
    }
    void set_right_child(std::shared_ptr<Node> node) override {
        right = std::dynamic_pointer_cast<Expression>(node);
    }
private:
    BinaryOp(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, Operation op, TokenRange tokens, std::shared_ptr<Node> parent) : Expression(tokens, parent, op_to_presedence(op)), left(left), right(right), op(op) {}
    static unsigned int op_to_presedence(Operation op) {
        switch(op) {
            case Mul:
            case Div:
                return FACTOR_PRESEDENCE;
            case Add:
            case Min:
                return SUM_PRESEDENCE;
            default:
                std::cout << "Error: Unknown binary op\n";
                return SUM_PRESEDENCE;
        }
    }
};

class UnaryOp : public Expression {
public:
    static std::shared_ptr<UnaryOp> create(std::shared_ptr<Expression> expr, TokenRange tokens, std::shared_ptr<Node> parent);
    std::shared_ptr<Expression> expr;

    void visit(Visitor &visitor) override {
        visitor.visit_unary_op(*this);
    }

    std::shared_ptr<Node> get_left_child() override {
        return expr;
    }
    std::shared_ptr<Node> get_right_child() override {
        return expr;
    }
    void set_left_child(std::shared_ptr<Node> node) override {
        expr = std::dynamic_pointer_cast<Expression>(node);
    }
    void set_right_child(std::shared_ptr<Node> node) override {
        expr = std::dynamic_pointer_cast<Expression>(node);
    }

private:
    UnaryOp(std::shared_ptr<Expression> expr, TokenRange tokens, std::shared_ptr<Node> parent) :  Expression(tokens, parent, UNARY_OP_PRESEDENCE), expr(expr) {}
};

class Literal: public Expression{
public:
    const Token &token;
    Literal(const Token &token, TokenRange tokens, std::shared_ptr<Node> parent) :  Expression(tokens, parent, LITERAL_PRESEDENCDE), token(token) {}

    std::shared_ptr<Node> get_left_child() override {
        return NULL;
    }
    std::shared_ptr<Node> get_right_child() override {
        return NULL;
    }

};

class Number : public Literal {
public:
    Number(const Token &token, TokenRange tokens, std::shared_ptr<Node> parent) : Literal(token, tokens, parent) {
        number = std::stof(token.literal());
    }
    float number;

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
    Access(TokenRange tokens, std::shared_ptr<Node> parent) : Expression(tokens, parent, ACCESS_PRESEDENCDE) {}

};

class Identifier : public Access, public Literal {
public:
    Identifier(const Token &token, TokenRange tokens, std::shared_ptr<Node> parent) : Access(tokens, parent), Literal(token, tokens, parent) {}

    void visit(Visitor &visitor) override {
        visitor.visit_identifier(*this);
    }

    std::shared_ptr<Node> get_left_child() override {
        return NULL;
    }
    std::shared_ptr<Node> get_right_child() override {
        return NULL;
    }
};

class ClassAccess : public Access {
public:
    static std::shared_ptr<ClassAccess> create(std::shared_ptr<Access> left, std::shared_ptr<Access> right, TokenRange tokens, std::shared_ptr<Node> parent);
    std::shared_ptr<Access> left, right;

    void visit(Visitor &visitor) override {
        visitor.visit_class_access(*this);
    }

    std::shared_ptr<Node> get_left_child() override {
        return left;
    }
    std::shared_ptr<Node> get_right_child() override {
        return right;
    }
    void set_left_child(std::shared_ptr<Node> node) override {
        left = std::dynamic_pointer_cast<Access>(node);
    }
    void set_right_child(std::shared_ptr<Node> node) override {
        right = std::dynamic_pointer_cast<Access>(node);
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

    std::shared_ptr<Node> get_left_child() override {
        return left;
    }
    std::shared_ptr<Node> get_right_child() override {
        return index;
    }
    void set_left_child(std::shared_ptr<Node> node) override {
        left = std::dynamic_pointer_cast<Access>(node);
    }
    void set_right_child(std::shared_ptr<Node> node) override {
        index = std::dynamic_pointer_cast<Expression>(node);
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

    std::shared_ptr<Node> get_left_child() override {
        return name;
    }
    std::shared_ptr<Node> get_right_child() override {
        return NULL;
    }
    void set_left_child(std::shared_ptr<Node> node) override {
        name = std::dynamic_pointer_cast<Access>(node);
    }
private:
    FunctionCall(std::shared_ptr<Access> name, TokenRange tokens, std::shared_ptr<Node> parent, std::vector<std::shared_ptr<Expression>> parameters) : Access(tokens, parent), name(name), parameters(parameters) {}
};

}

#endif