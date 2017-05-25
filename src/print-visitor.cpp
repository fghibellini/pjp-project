
#include "print-visitor.h"

PrintVisitor::PrintVisitor(ostream &os) : os(os) {};

void PrintVisitor::visit(const ast::Program &p) {
    os << "program " << p.name << ";";
    // TODO functions and procedures
    p.main->accept(*this);
};
void PrintVisitor::visit(const ast::Scope &p) {
    for (auto &d : p.declarations)
        d->accept(*this);
    p.body->accept(*this);
};
void PrintVisitor::visit(const ast::IntExpr &e) { os << to_string(e.val); };
void PrintVisitor::visit(const ast::CallFactor &s) {
    os << s.fname << "(";
    for (auto e : s.expr)
    {
        e->accept(*this);
        os << ", ";
    }
    os << ")";
};
void PrintVisitor::visit(const ast::Block &s) {
    os << "begin" << endl;
    auto sts = s.statements;
    for (auto i = sts.begin(); i != sts.end(); i++)
    {
        (*i)->accept(*this);
        os << endl;
    }
    os << "end";
};
void PrintVisitor::visit(const ast::ConstDeclaration &s) { os << "const "; for (auto &d : s.decls) os << d.first << "=" << to_string(d.second) << ","; os << ";"; };
void PrintVisitor::visit(const ast::VarDeclaration &s) { os << "var "; for (auto &d : s.varnames) os << d << ","; os << ";"; };
void PrintVisitor::visit(const ast::IfStatement &s) {
    os << "if ";
    s.condition->accept(*this);
    os << " then ";
    s.trueBranch->accept(*this);
    if (s.elseBranch != nullptr) {
        os << " else ";
        s.elseBranch->accept(*this);
    }
    os << ";";
};
void PrintVisitor::visit(const ast::WhileStatement &s) {
    os << "while ";
    s.condition->accept(*this);
    os << " do ";
    s.body->accept(*this);
    os << ";";
};
void PrintVisitor::visit(const ast::EmptyStatement &s) { os << ";"; };
void PrintVisitor::visit(const ast::UnaryMinusExpression &e) { os << "-"; e.expr->accept(*this); };
void PrintVisitor::visit(const ast::BinaryOpExpression &e) { os << "("; e.left->accept(*this); os << ")" << e.op << "("; e.right->accept(*this); os << ")"; };

