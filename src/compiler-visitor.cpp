
#include "compiler-visitor.h"

CompilerVisitor::CompilerVisitor()
  : ctx(new LLVMContext())
  , builder(new IRBuilder<>(*ctx))
  , currentScope(new LexicalScope())
{
    module = new Module("mila compiler", *ctx);

    INT_TYPE = llvm::IntegerType::get(*ctx, INT_BIT_SIZE);
    VOID_TYPE = llvm::Type::getVoidTy(*ctx);
    INT_ZERO = toMilaInt(0);

    FunctionType *output_fn_type = FunctionType::get(VOID_TYPE, vector<Type *>(1, INT_TYPE), false);
    functions.emplace("write", Function::Create(output_fn_type, Function::ExternalLinkage, "write", module));
    functions.emplace("writeln", Function::Create(output_fn_type, Function::ExternalLinkage, "writeln", module));
    FunctionType *input_fn_type = FunctionType::get(INT_TYPE, vector<Type *>(), false);
    functions.emplace("read", Function::Create(input_fn_type, Function::ExternalLinkage, "read", module));
};

Value *CompilerVisitor::toMilaInt(int val)
{
    return ConstantInt::get(*ctx, APInt(sizeof(int) * 8, val));
}

int CompilerVisitor::parseIntLiteral(ast::Expr *e)
{
    int r = 1;
    if (dynamic_cast<ast::UnaryMinusExpression *>(e)) {
        r = -1;
        e = dynamic_cast<ast::UnaryMinusExpression *>(e)->expr;
    }
    if (!dynamic_cast<ast::IntExpr *>(e)) {
        throw CompilationError("Invalid numeric literal!");
    }
    r *= dynamic_cast<ast::IntExpr *>(e)->val;
    return r;
}

void CompilerVisitor::dumpIR()
{
    module->print(errs(), nullptr);
}

void CompilerVisitor::generateObject(string outputPath)
{
	// Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto TargetTriple = "x86_64-unknown-linux-gnu"; //getDefaultTargetTriple(); can't get this function to link for some weird reason
    module->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
      errs() << Error;
      throw CompilationError("Unknown target");
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine =
        Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module->setDataLayout(TheTargetMachine->createDataLayout());

    auto Filename = outputPath;
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::F_None);

    if (EC) {
      CompilationError("Could not open file: " + EC.message());
    }

    legacy::PassManager pass;
    auto FileType = TargetMachine::CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
      throw CompilationError("TheTargetMachine can't emit a file of this type");
    }

    pass.run(*module);
    dest.flush();

    cout << "Wrote " << Filename << "\n";
};


void CompilerVisitor::visit(const ast::Program &p) {

    //create main method
    auto mainType = FunctionType::get(INT_TYPE, vector<llvm::Type *>(), false);
    auto main = Function::Create(mainType, Function::ExternalLinkage, "main", module);

    for (auto &f : p.functions)
    {
        declareFunction(*f);
    }

    for (auto f : p.functions)
    {
        f->accept(*this);
    }

    //body of main
    auto mainBlock = BasicBlock::Create(*ctx, "main_entry", main);
    builder->SetInsertPoint(mainBlock);
	
	fn = main;
	p.main->accept(*this);

    //return 0
    builder->CreateRet(INT_ZERO);

    verifyFunction(*main);

};

void CompilerVisitor::visit(const ast::Args &as) {
};

void CompilerVisitor::visit(const ast::AssignmentStatement &as) {
    if (dynamic_cast<ast::IdentExpr *>(as.left) == nullptr) {
        throw CompilationError("Only assignment to identifiers are supported!");
    }
    string ident = dynamic_cast<ast::IdentExpr *>(as.left)->ident;
    auto a = currentScope->getVariable(ident);
    if (a == nullptr) {
        throw CompilationError("Trying to assign to undeclared variable: " + ident);
    }
    as.right->accept(*this);
    auto right_val = val;
    val = builder->CreateStore(right_val, a);
};

void CompilerVisitor::visit(const ast::IdentExpr &ie) {
    auto res = currentScope->getBinding(ie.ident);
    if (!res.isPresent() || res.get().getBindingType() == BindingType::FUNCTION) {
        throw CompilationError("Referenced invalid variable: " + ie.ident);
    }
    if (res.get().getBindingType() == BindingType::CONSTANT) {
        val = res.get().getValue();
    } else if (res.get().getBindingType() == BindingType::VARIABLE) {
        val = builder->CreateLoad(res.get().getVariable(), ie.ident);
    } else {
        throw CompilationError("Invalid binding value type!");
    }
};

Function *CompilerVisitor::getFunction(string name)
{
    auto iter = functions.find(name);
    if (iter == functions.end()) {
        return nullptr;
    } else {
        return iter->second;
    }
}

void CompilerVisitor::declareFunction(const ast::FunctionDecl &fd)
{
    vector<Type *> arg_types;
    for (auto arg: fd.args->args)
    {
        arg->type->accept(*this);
        arg_types.push_back(tp);
    }
    fd.ret_type->accept(*this);
    FunctionType *fn_type = FunctionType::get(tp, arg_types, false);

    fn = Function::Create(fn_type, Function::ExternalLinkage, fd.name, module);
    functions.emplace(fd.name, fn);
}
    
void CompilerVisitor::visit(const ast::FunctionDecl &fd)
{
    fn = getFunction(fd.name);

    auto fn_block = BasicBlock::Create(*ctx, fd.name + "_entry", fn);
    builder->SetInsertPoint(fn_block);

    // new scope
    auto parent_scope = currentScope;
    currentScope = new LexicalScope(parent_scope);
    // return value variable
    auto ret_alloc = builder->CreateAlloca(INT_TYPE, 0, fd.name);
    currentScope->addVariableBinding(fd.name, ret_alloc);
    // argument variables
    int i = 0;
    for (auto &formal : fn->args())
    {
        auto actual = fd.args->args[i];
        auto arg_alloc = builder->CreateAlloca(INT_TYPE, 0, actual->name);
        currentScope->addVariableBinding(actual->name, arg_alloc);
        builder->CreateStore(&formal, arg_alloc);
        formal.setName(fd.args->args[i]->name);
        i += 1;
    }

    fd.scope->accept(*this);

    val = builder->CreateLoad(ret_alloc, fd.name);
    builder->CreateRet(val);

    delete currentScope;
    currentScope = parent_scope;

    verifyFunction(*fn);
};
void CompilerVisitor::visit(const ast::ProcedureDecl &fd) {
};
void CompilerVisitor::visit(const ast::Scope &p) {
    auto parent_scope = currentScope;
    currentScope = new LexicalScope(parent_scope);

    auto old_block = builder->GetInsertBlock();
    auto b = BasicBlock::Create(*ctx, "scope", old_block->getParent());
    builder->CreateBr(b);
    builder->SetInsertPoint(b);

    for (auto &d : p.declarations)
    {
        d->accept(*this);
    }
    p.body->accept(*this);

    delete currentScope;
    currentScope = parent_scope;
};
void CompilerVisitor::visit(const ast::IntExpr &e)
{
    val = toMilaInt(e.val);
};
void CompilerVisitor::visit(const ast::IndexingFactor &ifac)
{
}
void CompilerVisitor::visit(const ast::CallFactor &s) {
    Function *callee = getFunction(s.fname);
    if (callee == nullptr) {
        throw CompilationError("Trying to call undefined function \"" + s.fname + "\"");
    }
    if (s.fname == "read" || s.fname == "readln") {
        if (s.expr.size() != 1) {
            throw CompilationError("Invalid number of args!");
        }
        auto arg = s.expr[0];
        if (!dynamic_cast<ast::IdentExpr *>(arg)) {
            throw CompilationError("Can read only into variables.");
        }
        string ident = dynamic_cast<ast::IdentExpr *>(arg)->ident;
        auto alloc = currentScope->getVariable(ident);
        val = builder->CreateCall(callee, vector<Value *>(), "calltmp");
        val = builder->CreateStore(val, alloc, "calltmp");
    } else {
        if (callee->arg_size() != s.expr.size()) {
            throw CompilationError("Invalid number of args!");
        }
        vector<Value *> arg_vals;
        for (auto arg : s.expr)
        {
            arg->accept(*this);
            if (val == nullptr) {
                throw CompilationError("Couldn't compile argument!");
            }
            arg_vals.push_back(val);
        }

        val = builder->CreateCall(callee, arg_vals, "calltmp");
    }
};
void CompilerVisitor::visit(const ast::Block &s)
{

	for (auto stmt : s.statements)
	{
		stmt->accept(*this);
	}
};
void CompilerVisitor::visit(const ast::TypeSignature &s)
{
    // TODO actual implementation
    tp = INT_TYPE;
};
void CompilerVisitor::visit(const ast::ConstDeclaration &s)
{
    for (auto d : s.decls)
    {
        auto v = toMilaInt(d.second);
        currentScope->addValueBinding(d.first, v);
    }
};
void CompilerVisitor::visit(const ast::VarDeclaration &s)
{
    for (auto name : s.varnames)
    {
        auto a = builder->CreateAlloca(INT_TYPE, 0, name);
        currentScope->addVariableBinding(name, a);
    }
};

void CompilerVisitor::visit(const ast::IfStatement &s) {
    Function *parent_fn = builder->GetInsertBlock()->getParent();

    BasicBlock *then_bb = BasicBlock::Create(*ctx, "then", parent_fn);
    BasicBlock *else_bb = BasicBlock::Create(*ctx, "else");
    BasicBlock *merge_bb = BasicBlock::Create(*ctx, "ifcont");
    parent_fn->getBasicBlockList().push_back(else_bb);
    parent_fn->getBasicBlockList().push_back(merge_bb);

    s.condition->accept(*this);
    Value *cond_res = val;
    auto cond_v = builder->CreateICmpNE(cond_res, INT_ZERO, "ifcond");
    builder->CreateCondBr(cond_v, then_bb, else_bb);
    
    builder->SetInsertPoint(then_bb);
    s.trueBranch->accept(*this);
    builder->CreateBr(merge_bb);

    builder->SetInsertPoint(else_bb);
    s.elseBranch->accept(*this);
    builder->CreateBr(merge_bb);

    builder->SetInsertPoint(merge_bb);
};

void CompilerVisitor::visit(const ast::ForStatement &s)
{
    auto parent_scope = currentScope;
    currentScope = new LexicalScope(parent_scope);

    Function *parent_fn = builder->GetInsertBlock()->getParent();

    BasicBlock *condition_bb = BasicBlock::Create(*ctx, "forcond", parent_fn);
    BasicBlock *body_bb = BasicBlock::Create(*ctx, "forbody");
    BasicBlock *cont_bb = BasicBlock::Create(*ctx, "forcont");
    parent_fn->getBasicBlockList().push_back(body_bb);
    parent_fn->getBasicBlockList().push_back(cont_bb);

    if (currentScope->getBinding(s.iterator).isPresent()) {
        throw CompilationError("Iterator variable " + s.iterator + " already used in this scope");
    }
    auto iterator_alloc = builder->CreateAlloca(INT_TYPE, 0, s.iterator);
    currentScope->addVariableBinding(s.iterator, iterator_alloc);

    s.val0->accept(*this);
    Value *val0 = val;
    s.val1->accept(*this);
    Value *val1 = val;

    builder->CreateStore(val0, iterator_alloc);
    builder->CreateBr(condition_bb);

    builder->SetInsertPoint(condition_bb);
    Value *cond_iterator_val = builder->CreateLoad(iterator_alloc);
    auto cond_v = s.downto ?
        builder->CreateICmpUGT(cond_iterator_val, val1, "forcond"):
        builder->CreateICmpULT(cond_iterator_val, val1, "forcond");
    builder->CreateCondBr(cond_v, body_bb, cont_bb);
    
    builder->SetInsertPoint(body_bb);
    break_blocks.push(cont_bb);
    s.body->accept(*this);
    break_blocks.pop();
    Value *cur_iterator_val = builder->CreateLoad(iterator_alloc);
    Value *next_iterator_val = s.downto ?
        builder->CreateSub(cur_iterator_val, toMilaInt(1)) :
        builder->CreateAdd(cur_iterator_val, toMilaInt(1)) ;
    builder->CreateStore(next_iterator_val, iterator_alloc);
    builder->CreateBr(condition_bb);

    builder->SetInsertPoint(cont_bb);

    delete currentScope;
    currentScope = parent_scope;
};

void CompilerVisitor::visit(const ast::WhileStatement &s) {
    Function *parent_fn = builder->GetInsertBlock()->getParent();

    BasicBlock *condition_bb = BasicBlock::Create(*ctx, "whilecond", parent_fn);
    BasicBlock *body_bb = BasicBlock::Create(*ctx, "whilebody");
    BasicBlock *cont_bb = BasicBlock::Create(*ctx, "whilecont");
    parent_fn->getBasicBlockList().push_back(body_bb);
    parent_fn->getBasicBlockList().push_back(cont_bb);

    builder->CreateBr(condition_bb);
    builder->SetInsertPoint(condition_bb);
    s.condition->accept(*this);
    auto cond_res = val;
    auto cond_v = builder->CreateICmpNE(cond_res, INT_ZERO, "whilecond");
    builder->CreateCondBr(cond_v, body_bb, cont_bb);
    
    builder->SetInsertPoint(body_bb);
    break_blocks.push(cont_bb);
    s.body->accept(*this);
    break_blocks.pop();
    builder->CreateBr(condition_bb);

    builder->SetInsertPoint(cont_bb);
};

void CompilerVisitor::visit(const ast::EmptyStatement &s) {};

void CompilerVisitor::visit(const ast::UnaryMinusExpression &e)
{
    e.expr->accept(*this);
    val = builder->CreateNeg(val, "neg_res");
};

void CompilerVisitor::visit(const ast::BinaryOpExpression &e)
{
    auto op = e.op;
    e.left->accept(*this);
    Value *left = val;
    e.right->accept(*this);
    Value *right = val;
    if (op == "+") {
        val = builder->CreateAdd(left, right, "add_res");
    } else if (op == "-") {
        val = builder->CreateSub(left, right, "sub_res");
    } else if (op == "*") {
        val = builder->CreateMul(left, right, "mul_res");
    } else if (op == "div") {
        val = builder->CreateSDiv(left, right, "div_res");
    } else if (op == ">") {
        val = builder->CreateICmpSGT(left, right, "cmpgt_res");
        val = builder->CreateBitCast(val, INT_TYPE, "cmpgt_res8b");
    } else if (op == "<") {
        val = builder->CreateICmpSLT(left, right, "cmplt_res");
        val = builder->CreateBitCast(val, INT_TYPE, "cmplt_res8b");
    } else if (op == ">=") {
        val = builder->CreateICmpSGE(left, right, "cmpge_res");
        val = builder->CreateBitCast(val, INT_TYPE, "cmpge_res8b");
    } else if (op == "<=") {
        val = builder->CreateICmpSLE(left, right, "cmple_res");
        val = builder->CreateBitCast(val, INT_TYPE, "cmple_res8b");
    } else if (op == "=") {
        val = builder->CreateICmpEQ(left, right, "cmpeq_res");
        val = builder->CreateBitCast(val, INT_TYPE, "cmpeq_res8b");
    } else {
        throw CompilationError("Invalid operator " + op);
    }
};

void CompilerVisitor::visit(const ast::BreakStatement &stmt)
{
    if (break_blocks.empty()) {
        throw CompilationError("Trying to call break outside a for/while");
    }
    builder->CreateBr(break_blocks.top());

    // br must be last in block so we create a dummy one
    // for what follows after the break call
    auto brBlock = BasicBlock::Create(*ctx, "break_dummy", builder->GetInsertBlock()->getParent());
    builder->SetInsertPoint(brBlock);
};

