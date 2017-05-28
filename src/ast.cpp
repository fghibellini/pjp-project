
#include "ast.h"

namespace ast {

void IntExpr::accept(Visitor &v) const { v.visit(*this); }
void Program::accept(Visitor &v) const { v.visit(*this); }
void Block::accept(Visitor &v) const { v.visit(*this); }
void Scope::accept(Visitor &v) const { v.visit(*this); }
void FunctionDecl::accept(Visitor &v) const { v.visit(*this); }
void ProcedureDecl::accept(Visitor &v) const { v.visit(*this); }
void CallFactor::accept(Visitor &v) const { v.visit(*this); }
void AssignmentStatement::accept(Visitor &v) const { v.visit(*this); }
void ConstDeclaration::accept(Visitor &v) const { v.visit(*this); }
void VarDeclaration::accept(Visitor &v) const { v.visit(*this); }
void IfStatement::accept(Visitor &v) const { v.visit(*this); }
void WhileStatement::accept(Visitor &v) const { v.visit(*this); }
void EmptyStatement::accept(Visitor &v) const { v.visit(*this); }
void BinaryOpExpression::accept(Visitor &v) const { v.visit(*this); }
void UnaryMinusExpression::accept(Visitor &v) const { v.visit(*this); }
void IdentExpr::accept(Visitor &v) const { v.visit(*this); }
void TypeSignature::accept(Visitor &v) const { v.visit(*this); }
void Args::accept(Visitor &v) const { v.visit(*this); }

IntExpr::IntExpr(int val) : val(val) {}
Program::Program(string name, vector<FunctionDecl *> functions, vector<ProcedureDecl *> procedures, Scope *main) : name(name), functions(functions), procedures(procedures), main(main) {}
CallFactor::CallFactor(string fname, vector<Expr *> expr) : fname(fname), expr(expr) {}
AssignmentStatement::AssignmentStatement(string left, Expr *right) : left(left), right(right) {}
Block::Block(vector<Statement *> statements) : statements(statements) {}
Scope::Scope(vector<Declaration *> declarations, Block *body) : declarations(declarations), body(body) {}
FunctionDecl::FunctionDecl(string name, Args *args, TypeSignature *ret_type, Scope *scope) : name(name), args(args), ret_type(ret_type), scope(scope) {}
ProcedureDecl::ProcedureDecl(string name, Args *args, Scope *scope) : name(name), args(args), scope(scope) {}
ConstDeclaration::ConstDeclaration(map<string,int> decls) : decls(decls) {}
VarDeclaration::VarDeclaration(vector<string> varnames, TypeSignature *type) : varnames(varnames), type(type) {}
IfStatement::IfStatement(Expr *condition, Statement *trueBranch, Statement *elseBranch) : condition(condition), trueBranch(trueBranch), elseBranch(elseBranch) {}
WhileStatement::WhileStatement(Expr *condition, Statement *body) : condition(condition), body(body) {}
BinaryOpExpression::BinaryOpExpression(string op, Expr *left, Expr *right) : op(op), left(left), right(right) {}
UnaryMinusExpression::UnaryMinusExpression(Expr *expr) : expr(expr) {}
IdentExpr::IdentExpr(string ident) : ident(ident) {}
TypeSignature::TypeSignature(bool array, int min_index, int max_index) : array(array), min_index(min_index), max_index(max_index) {}
Arg::Arg(string name, TypeSignature *type) : name(name), type(type) {}
Args::Args(vector<Arg *> args) : args(args) {}

void Visitor::visit(const Program &p) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const Scope &p) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const FunctionDecl &p) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const ProcedureDecl &p) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const IntExpr &e) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const CallFactor &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
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
void Visitor::visit(const TypeSignature &s) { throw domain_error("Visitor doesn't support this AST node type!"); };
void Visitor::visit(const Args &s) { throw domain_error("Visitor doesn't support this AST node type!"); };

}
