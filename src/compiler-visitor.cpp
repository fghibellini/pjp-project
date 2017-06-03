
#include "compiler-visitor.h"

CompilerVisitor::CompilerVisitor()
  : ctx(new LLVMContext())
  , builder(new IRBuilder<>(*ctx))
{
    module = new Module("mila compiler", *ctx);

    INT_TYPE = llvm::IntegerType::get(*ctx, INT_BIT_SIZE);
    VOID_TYPE = llvm::Type::getVoidTy(*ctx);

    FunctionType *output_fn_type = FunctionType::get(VOID_TYPE, vector<Type *>(1, INT_TYPE), false);
    auto write = Function::Create(output_fn_type, Function::ExternalLinkage, "write", module);
    writeln = Function::Create(output_fn_type, Function::ExternalLinkage, "writeln", module);
};

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
      throw 1;
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
      errs() << "Could not open file: " << EC.message();
      throw 1;
    }

    legacy::PassManager pass;
    auto FileType = TargetMachine::CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
      errs() << "TheTargetMachine can't emit a file of this type";
      throw 1;
    }

    pass.run(*module);
    dest.flush();

    cout << "Wrote " << Filename << "\n";
};


void CompilerVisitor::visit(const ast::Program &p) {

    //create main method
    auto mainType = FunctionType::get(INT_TYPE, vector<llvm::Type *>(), false);
    auto main = Function::Create(mainType, Function::ExternalLinkage, "main", module);

    //body of main
    //auto mainBlock = BasicBlock::Create(*ctx, "entry", main);
    //builder->SetInsertPoint(mainBlock);
	
	fn = main;
	p.main->accept(*this);

    //return 0
    auto zero = llvm::ConstantInt::get(INT_TYPE, 0);
    builder->CreateRet(zero);

    verifyFunction(*main);

};
void CompilerVisitor::visit(const ast::Args &as) {
};
void CompilerVisitor::visit(const ast::AssignmentStatement &as) {
};
void CompilerVisitor::visit(const ast::IdentExpr &ie) {
    auto ref = currentScope.find(ie.ident);
    if (ref == currentScope.end()) {
        throw string("Referenced invalid variable" + ie.ident);
    }
    val = ref->second;
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
    for (auto &d : p.declarations)
    {
        d->accept(*this);
    }
    p.body->accept(*this);
};
void CompilerVisitor::visit(const ast::IntExpr &e)
{
    val = ConstantInt::get(*ctx, APInt(sizeof(e.val) * 8, e.val));
};
void CompilerVisitor::visit(const ast::IndexingFactor &ifac)
{
}
void CompilerVisitor::visit(const ast::CallFactor &s) {
    Function *callee = module->getFunction(s.fname);
    if (callee == nullptr) {
        throw "Invalid function!";
    }
    if (callee->arg_size() != s.expr.size()) {
        throw "Invalid number of args!";
    }
    vector<Value *> arg_vals;
    for (auto arg : s.expr)
    {
        arg->accept(*this);
        if (val == nullptr) {
            throw "Couldn't compile argument!";
        }
        arg_vals.push_back(val);
    }

	val = builder->CreateCall(callee, arg_vals, "calltmp");
};
void CompilerVisitor::visit(const ast::Block &s)
{
    auto b = BasicBlock::Create(*ctx, "b1", fn);
    builder->SetInsertPoint(b);


	for (auto stmt : s.statements)
	{
		stmt->accept(*this);
	}
};
void CompilerVisitor::visit(const ast::TypeSignature &s)
{
};
void CompilerVisitor::visit(const ast::ConstDeclaration &s)
{
    for (auto d : s.decls)
    {
        auto v = ConstantInt::get(*ctx, APInt(sizeof(d.second) * 8, d.second));
        currentScope.emplace(d.first, v);
    }
};
void CompilerVisitor::visit(const ast::VarDeclaration &s)
{
    cout << "Visiting declaration" << endl;
    for (auto name : s.varnames)
    {
        cout << "Declaration: " << name << endl;
        builder->CreateAlloca(INT_TYPE, nullptr, name);
    }
};
void CompilerVisitor::visit(const ast::IfStatement &s) {
};
void CompilerVisitor::visit(const ast::ForStatement &s) {
};
void CompilerVisitor::visit(const ast::WhileStatement &s) {
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
    } else {
        throw "Invalid operator " + op;
    }
};

