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
#define STMT_PRESEDENCE 4
#define FUNCTION_CLASS_PRESEDENCE 5
#define FILE_PRESEDENCE 6

namespace ast {

class BinaryOp;
class UnaryOp;
class Number;
class String;
class Identifier;
class ClassAccess;
class IndexAccess;
class FunctionCall;
class Return;
class Assign;
class Block;
class FunctionDefinition;
class File;

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
    virtual void visit_return_stmt(Return &) {}
    virtual void visit_assign_stmt(Assign &) {}
    virtual void visit_block_stmt(Block &) {}
    virtual void visit_function_definition(FunctionDefinition &) {}
    virtual void visit_file(File &) {}
};

class Node{
public:
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
    Literal(const Token &token, TokenRange tokens, std::shared_ptr<Node> parent, unsigned int presedence) :  Expression(tokens, parent, presedence), token(token) {}

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

class Statement : public Node {
public:
    Statement(TokenRange tokens, std::shared_ptr<Node> parent, unsigned int presedence) : Node(tokens, parent, presedence) {}
};

class Return : public Statement {
public:
    static std::shared_ptr<Return> create(std::shared_ptr<Expression> expr, TokenRange tokens, std::shared_ptr<Node> parent);

    std::shared_ptr<Expression> expr;

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
    void visit(Visitor &visitor) override {
        visitor.visit_return_stmt(*this);
    }
private:
    Return(std::shared_ptr<Expression> expr, std::shared_ptr<Node> parent, TokenRange tokens) : Statement(tokens, parent, STMT_PRESEDENCE), expr(expr) {}
};

class Assign: public Statement {
public:
    static std::shared_ptr<Assign> create(std::shared_ptr<Access> location, std::shared_ptr<Expression> expr, std::shared_ptr<Node> parent, TokenRange tokens);

    std::shared_ptr<Access> location;
    std::shared_ptr<Expression> expr;
    

    std::shared_ptr<Node> get_left_child() override {
        return location;
    }
    std::shared_ptr<Node> get_right_child() override {
        return expr;
    }
    void set_left_child(std::shared_ptr<Node> node) override {
        location = std::dynamic_pointer_cast<Access>(node);
    }
    void set_right_child(std::shared_ptr<Node> node) override {
        expr = std::dynamic_pointer_cast<Expression>(node);
    }
    void visit(Visitor &visitor) override {
        visitor.visit_assign_stmt(*this);
    }
private:
    Assign(std::shared_ptr<Access> location, std::shared_ptr<Expression> expr, std::shared_ptr<Node> parent, TokenRange tokens) : Statement(tokens, parent, STMT_PRESEDENCE), location(location), expr(expr) {}
};

class Block: public Statement {
public:
    std::vector<std::shared_ptr<Statement>> statements;

    static std::shared_ptr<Block> create(std::vector<std::shared_ptr<Statement>> statements, TokenRange tokens, std::shared_ptr<Node> parent);
    static std::shared_ptr<Block> create(TokenRange tokens, std::shared_ptr<Node> parent) {
        return create(std::vector<std::shared_ptr<Statement>>{}, tokens, parent);
    }

    std::shared_ptr<Node> get_left_child() override {
        return statements.front();
    }
    std::shared_ptr<Node> get_right_child() override {
        return statements.back();
    }

    void set_right_child(std::shared_ptr<Node> node) override {
        statements.push_back(std::dynamic_pointer_cast<Statement>(node));
    }
    void visit(Visitor &visitor) override {
        visitor.visit_block_stmt(*this);
    }
private:
    Block(std::vector<std::shared_ptr<Statement>> statements, TokenRange tokens, std::shared_ptr<Node> parent) : Statement(tokens, parent, STMT_PRESEDENCE), statements(statements) {}
};

class FunctionDefinition: public Statement {
public:
    std::shared_ptr<Block> block;
    std::shared_ptr<Identifier> name;
    std::vector<std::shared_ptr<Identifier>> parameters;

    static std::shared_ptr<FunctionDefinition> create(std::shared_ptr<Block> block, std::vector<std::shared_ptr<Identifier>> parameters, std::shared_ptr<Identifier> name, TokenRange tokens, std::shared_ptr<Node> parent);

    std::shared_ptr<Node> get_left_child() override {
        return block;
    }
    std::shared_ptr<Node> get_right_child() override {
        return block;
    }
    void set_left_child(std::shared_ptr<Node> node) override {
        block = std::dynamic_pointer_cast<Block>(node);
    }
    void set_right_child(std::shared_ptr<Node> node) override {
        block = std::dynamic_pointer_cast<Block>(node);
    }
    void visit(Visitor &visitor) override {
        visitor.visit_function_definition(*this);
    }
private:

    FunctionDefinition(std::shared_ptr<Block> block, std::vector<std::shared_ptr<Identifier>> parameters, std::shared_ptr<Identifier> name, TokenRange tokens, std::shared_ptr<Node> parent) : Statement(tokens, parent, FUNCTION_CLASS_PRESEDENCE), block(block), name(name), parameters(parameters) {}
};

class File: public Node {
public:
    static std::shared_ptr<File> create(std::string filename, TokenRange tokens);

    std::string filename;
    std::vector<std::shared_ptr<FunctionDefinition>> functions;
    std::shared_ptr<Block> code;

    void visit(Visitor &visitor) override {
        visitor.visit_file(*this);
    };
    virtual std::shared_ptr<Node> get_left_child() {
        std::cout << "Error: File does not have a clear left child\n";
        return NULL;
    }
    virtual std::shared_ptr<Node> get_right_child() {
        std::cout << "Error: File does not have a clear right child\n";
        return NULL;
    }
private:
    File(std::string filename, TokenRange tokens) : Node(tokens, NULL, FILE_PRESEDENCE), filename(filename) {}
};

}

#endif