#ifndef __AST_H__
#define __AST_H__

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace ast {

struct Visitor;

struct Node
{
    virtual void accept(Visitor &v) const = 0;
};

struct Expr : public Node
{
};

struct UnaryMinusExpression : public Expr
{
    unique_ptr<Expr> expr;
public:
    UnaryMinusExpression(unique_ptr<Expr> &&expr);
    virtual void accept(Visitor &v) const;
};

struct BinaryOpExpression : public Expr
{
    string op;
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;
public:
    BinaryOpExpression(string &&op, unique_ptr<Expr> &&left, unique_ptr<Expr> &&right);
    virtual void accept(Visitor &v) const;
};

struct IntExpr : public Expr
{
    int val;
public:
    IntExpr(int val);
    virtual void accept(Visitor &v) const;
};

struct IdentExpr : public Expr
{
    string ident;
public:
    IdentExpr(string &&ident);
    virtual void accept(Visitor &v) const;
};

struct Statement : public Node
{
};

// deprecated
struct EmptyStatement : public Statement
{
public:
    virtual void accept(Visitor &v) const;
};

struct IfStatement : public Statement
{
    unique_ptr<Expr> condition;
    unique_ptr<Statement> trueBranch;
    unique_ptr<Statement> elseBranch;
public:
    IfStatement(unique_ptr<Expr> &&condition, unique_ptr<Statement> &&trueBranch, unique_ptr<Statement> &&elseBranch);
    virtual void accept(Visitor &v) const;
};

struct WhileStatement : public Statement
{
    unique_ptr<Expr> condition;
    unique_ptr<Statement> body;
public:
    WhileStatement(unique_ptr<Expr> &&condition, unique_ptr<Statement> &&body);
    virtual void accept(Visitor &v) const;
};

struct AssignmentStatement : public Statement
{
    string left;
    unique_ptr<Expr> right;
public:
    AssignmentStatement(string left, unique_ptr<Expr> &&right);
    virtual void accept(Visitor &v) const;
};

struct WriteStmt : public Statement
{
    unique_ptr<Expr> expr;
public:
    WriteStmt(unique_ptr<Expr> &&expr);
    virtual void accept(Visitor &v) const;
};

struct Declaration : public Node
{
};

struct ConstDeclaration : public Declaration
{
    map<string,int> decls;
public:
    ConstDeclaration(map<string,int> &&decls);
    virtual void accept(Visitor &v) const;
};

struct VarDeclaration : public Declaration
{
    vector<string> varnames;
public:
    VarDeclaration(vector<string> &&varnames);
    virtual void accept(Visitor &v) const;
};

struct Block : public Statement
{
    vector<unique_ptr<Statement>> statements;
public:
    Block(vector<unique_ptr<Statement>> statements);
    virtual void accept(Visitor &v) const;
};

struct Program : public Node
{
    vector<unique_ptr<Declaration>> declarations;    
    Block body;
public:
    Program(vector<unique_ptr<Declaration>> &&declarations, Block &&body);
    virtual void accept(Visitor &v) const;
};

struct Visitor {
    virtual void visit(const Program &p);
    virtual void visit(const IntExpr &e);
    virtual void visit(const WriteStmt &s);
    virtual void visit(const Block &s);
    virtual void visit(const AssignmentStatement &s);
    virtual void visit(const ConstDeclaration &s);
    virtual void visit(const VarDeclaration &s);
    virtual void visit(const IfStatement &s);
    virtual void visit(const WhileStatement &s);
    virtual void visit(const EmptyStatement &s);
    virtual void visit(const BinaryOpExpression &s);
    virtual void visit(const UnaryMinusExpression &e);
    virtual void visit(const IdentExpr &s);
};

}

#endif
