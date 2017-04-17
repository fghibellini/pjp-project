
#include "ast.h"

namespace ast {

void IntExpr::accept(Visitor &v) const { v.visit(*this); }
void Program::accept(Visitor &v) const { v.visit(*this); }
void Block::accept(Visitor &v) const { v.visit(*this); }
void WriteStmt::accept(Visitor &v) const { v.visit(*this); }
void AssignmentStatement::accept(Visitor &v) const { v.visit(*this); }
void ConstDeclaration::accept(Visitor &v) const { v.visit(*this); }
void VarDeclaration::accept(Visitor &v) const { v.visit(*this); }
void IfStatement::accept(Visitor &v) const { v.visit(*this); }
void WhileStatement::accept(Visitor &v) const { v.visit(*this); }
void EmptyStatement::accept(Visitor &v) const { v.visit(*this); }
void BinaryOpExpression::accept(Visitor &v) const { v.visit(*this); }
void UnaryMinusExpression::accept(Visitor &v) const { v.visit(*this); }
void IdentExpr::accept(Visitor &v) const { v.visit(*this); }

IntExpr::IntExpr(int val) : val(val) {}
Program::Program(vector<unique_ptr<Declaration>> &&declarations, Block &&body) : declarations(move(declarations)), body(move(body)) {}
WriteStmt::WriteStmt(unique_ptr<Expr> &&expr) : expr(move(expr)) {}
AssignmentStatement::AssignmentStatement(string left, unique_ptr<Expr> &&right) : left(left), right(move(right)) {}
Block::Block(vector<unique_ptr<Statement>> statements) : statements(move(statements)) {}
ConstDeclaration::ConstDeclaration(map<string,int> &&decls) : decls(decls) {}
VarDeclaration::VarDeclaration(vector<string> &&varnames) : varnames(varnames) {}
IfStatement::IfStatement(unique_ptr<Expr> &&condition, unique_ptr<Statement> &&trueBranch, unique_ptr<Statement> &&elseBranch) : condition(move(condition)), trueBranch(move(trueBranch)), elseBranch(move(elseBranch)) {}
WhileStatement::WhileStatement(unique_ptr<Expr> &&condition, unique_ptr<Statement> &&body) : condition(move(condition)), body(move(body)) {}
BinaryOpExpression::BinaryOpExpression(string &&op, unique_ptr<Expr> &&left, unique_ptr<Expr> &&right) : op(move(op)), left(move(left)), right(move(right)) {}
UnaryMinusExpression::UnaryMinusExpression(unique_ptr<Expr> &&expr) : expr(move(expr)) {}
IdentExpr::IdentExpr(string &&ident) : ident(move(ident)) {}

void Visitor::visit(const Program &p) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const IntExpr &e) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const WriteStmt &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const Block &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const AssignmentStatement &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const ConstDeclaration &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const VarDeclaration &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const IfStatement &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const WhileStatement &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const EmptyStatement &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const BinaryOpExpression &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const UnaryMinusExpression &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const IdentExpr &s) { throw domain_error("Visitor doesn't support this AST node type!"); };

}
