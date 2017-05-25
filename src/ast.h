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

struct Statement : public Node
{
};

struct Expr : public Statement
{
};

struct UnaryMinusExpression : public Expr
{
    Expr *expr;
public:
    UnaryMinusExpression(Expr *expr);
    virtual void accept(Visitor &v) const;
};

struct BinaryOpExpression : public Expr
{
    string op;
    Expr *left;
    Expr *right;
public:
    BinaryOpExpression(string op, Expr *left, Expr *right);
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
    IdentExpr(string ident);
    virtual void accept(Visitor &v) const;
};

// deprecated
struct EmptyStatement : public Statement
{
public:
    virtual void accept(Visitor &v) const;
};

struct IfStatement : public Statement
{
    Expr *condition;
    Statement *trueBranch;
    Statement *elseBranch;
public:
    IfStatement(Expr *condition, Statement *trueBranch, Statement *elseBranch);
    virtual void accept(Visitor &v) const;
};

struct WhileStatement : public Statement
{
    Expr *condition;
    Statement *body;
public:
    WhileStatement(Expr * condition, Statement * body);
    virtual void accept(Visitor &v) const;
};

struct AssignmentStatement : public Statement
{
    string left;
    Expr *right;
public:
    AssignmentStatement(string left, Expr * right);
    virtual void accept(Visitor &v) const;
};

struct CallFactor : public Expr
{
    string fname;
    vector<Expr *> expr;
public:
    CallFactor(string fname, vector<Expr *> expr);
    virtual void accept(Visitor &v) const;
};

struct Declaration : public Node
{
};

struct ConstDeclaration : public Declaration
{
    map<string,int> decls;
public:
    ConstDeclaration(map<string,int> decls);
    virtual void accept(Visitor &v) const;
};

struct VarDeclaration : public Declaration
{
    vector<string> varnames;
public:
    VarDeclaration(vector<string> varnames);
    virtual void accept(Visitor &v) const;
};

struct Block : public Statement
{
    vector<Statement *> statements;
public:
    Block(vector<Statement *> statements);
    virtual void accept(Visitor &v) const;
};

struct Scope : public Node
{
    vector<Declaration *> declarations;    
    Block *body;
public:
    Scope(vector<Declaration *> declarations, Block *body);
    virtual void accept(Visitor &v) const;
};

struct FunctionDecl : public Node
{
    vector<Declaration *> declarations;    
    Block *body;
public:
    FunctionDecl(vector<Declaration *> declarations, Block *body);
    virtual void accept(Visitor &v) const;
};

struct ProcedureDecl : public Node
{
    vector<Declaration *> declarations;    
    Block *body;
public:
    ProcedureDecl(vector<Declaration *> declarations, Block *body);
    virtual void accept(Visitor &v) const;
};

struct Program : public Node
{
    string name;
    vector<ast::FunctionDecl *> functions;
    vector<ast::ProcedureDecl *> procedures;
    ast::Scope *main;
public:
    Program(string name, vector<FunctionDecl *> functions, vector<ProcedureDecl *> procedures, Scope *main);
    virtual void accept(Visitor &v) const;
};

struct Visitor {
    virtual void visit(const Program &p);
    virtual void visit(const IntExpr &e);
    virtual void visit(const CallFactor &s);
    virtual void visit(const FunctionDecl &s);
    virtual void visit(const ProcedureDecl &s);
    virtual void visit(const Scope &s);
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
