#ifndef __PRINT_VISITOR_H__
#define __PRINT_VISITOR_H__

#include <ostream>

#include "ast.h"

struct PrintVisitor : public ast::Visitor {
private:
    ostream &os;
public:
    PrintVisitor(ostream &os);
    virtual void visit(const ast::Program &p);
    virtual void visit(const ast::IntExpr &e);
    virtual void visit(const ast::WriteStmt &s);
    virtual void visit(const ast::Block &s);
    virtual void visit(const ast::ConstDeclaration &s);
    virtual void visit(const ast::VarDeclaration &s);
    virtual void visit(const ast::IfStatement &s);
    virtual void visit(const ast::WhileStatement &s);
    virtual void visit(const ast::EmptyStatement &s);
    virtual void visit(const ast::UnaryMinusExpression &s);
    virtual void visit(const ast::BinaryOpExpression &s);
};

#endif
