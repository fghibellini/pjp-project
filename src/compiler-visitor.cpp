
#include "compiler-visitor.h"

CompilerVisitor::CompilerVisitor()
  : ctx(new LLVMContext())
  , builder(new IRBuilder<>(*ctx))
{
    module = new Module("my cool jit", *ctx);
};

void CompilerVisitor::generate()
{
    module->print(errs(), nullptr);
};

void CompilerVisitor::visit(const ast::Program &p) {
};
void CompilerVisitor::visit(const ast::Args &as) {
};
void CompilerVisitor::visit(const ast::AssignmentStatement &as) {
};
void CompilerVisitor::visit(const ast::IdentExpr &ie) {
};
void CompilerVisitor::visit(const ast::FunctionDecl &fd) {
    /*
    vector<Type *> arg_types;
    for (auto arg: fd.args->args)
    {
        arg_types.push_back(to_llvm_type(arg->type));
    }

    FunctionType *fn_type = FunctionType::get(to_llvm_type(fd->ret_type), arg_types, false);

    fn = Function::Create(fn_type, Function::ExternalLinkage, fd.name, module);

    int i = 0;
    for (auto &arg : fn->args())
    {
        arg.setName(fd.args->args[i++].name);
    }
    */
};
void CompilerVisitor::visit(const ast::ProcedureDecl &fd) {
};
void CompilerVisitor::visit(const ast::Scope &p) {
    /*
    for (auto &d : p.declarations)
        d->accept(*this);
    p.body->accept(*this);
    */
};
void CompilerVisitor::visit(const ast::IntExpr &e)
{
    val = ConstantInt::get(*ctx, APInt(sizeof(e.val) * 8, e.val));
};
void CompilerVisitor::visit(const ast::IndexingFactor &ifac)
{
}
void CompilerVisitor::visit(const ast::CallFactor &s) {
    /*
    Function *callee = module->getFunction(fd.name);
    if (callee == nullptr) {
        throw exception("Invalid function!");
    }
    if (callee->arg_size() != fd.expr->size()) {
        throw exception("Invalid number of args!");
    }
    vector<Value *> arg_vals;
    for (auto arg : fd.expr) {
        (*arg)->accept(*this);
        if (val == nullptr) {
            throw exception("Couldn't compile argument!");
        }
        arg_vals.push_back(val);
    }
    val = builder.CreateCall(callee, arg_vals, "calltmp");
    */
};
void CompilerVisitor::visit(const ast::Block &s) {
};
void CompilerVisitor::visit(const ast::TypeSignature &s)
{
};
void CompilerVisitor::visit(const ast::ConstDeclaration &s) {};
void CompilerVisitor::visit(const ast::VarDeclaration &s)
{
};
void CompilerVisitor::visit(const ast::IfStatement &s) {
};
void CompilerVisitor::visit(const ast::ForStatement &s) {
};
void CompilerVisitor::visit(const ast::WhileStatement &s) {
};
void CompilerVisitor::visit(const ast::EmptyStatement &s) {};
void CompilerVisitor::visit(const ast::UnaryMinusExpression &e) {};
void CompilerVisitor::visit(const ast::BinaryOpExpression &e) {};

