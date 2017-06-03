
#include "compiler-visitor.h"

CompilationError::CompilationError(const std::string& what_arg)
: runtime_error(what_arg)
, msg(what_arg)
, what_msg(("COMPILATION_ERROR [" + msg + "]").c_str())
{
}

const char* CompilationError::what() const noexcept
{
    return what_msg.c_str(); 
}

CompilerVisitor::CompilerVisitor()
  : ctx(new LLVMContext())
  , builder(new IRBuilder<>(*ctx))
{
    module = new Module("mila compiler", *ctx);

    INT_TYPE = llvm::IntegerType::get(*ctx, INT_BIT_SIZE);
    VOID_TYPE = llvm::Type::getVoidTy(*ctx);

    FunctionType *output_fn_type = FunctionType::get(VOID_TYPE, vector<Type *>(1, INT_TYPE), false);
    addFunctionBinding("write", Function::Create(output_fn_type, Function::ExternalLinkage, "write", module));
    addFunctionBinding("writeln", Function::Create(output_fn_type, Function::ExternalLinkage, "writeln", module));
};

void CompilerVisitor::addFunctionBinding(string name, Function *f)
{
    BindingValue binding = { BindingValue::FUNCTION, nullptr, f };
    currentScope.emplace(name, binding);
}

void CompilerVisitor::addValueBinding(string name, Value *v)
{
    BindingValue binding = { BindingValue::VALUE, v, nullptr };
    currentScope.emplace(name, binding);
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
    if (ref == currentScope.end() || ref->second.type != BindingValue::VALUE) {
        throw CompilationError("Referenced invalid variable" + ie.ident);
    }
    val = ref->second.v;
};
void CompilerVisitor::visit(const ast::FunctionDecl &fd) {
    vector<Type *> arg_types;
    for (auto arg: fd.args->args)
    {
        arg->type->accept(*this);
        arg_types.push_back(tp);
    }
    /*

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
    auto binding_value = currentScope.find(s.fname);
    if (binding_value == currentScope.end() || binding_value->second.type != BindingValue::FUNCTION) {
        throw CompilationError("Invalid function!");
    }
    Function *callee = binding_value->second.f;
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
    // TODO actual implementation
    tp = INT_TYPE;
};
void CompilerVisitor::visit(const ast::ConstDeclaration &s)
{
    for (auto d : s.decls)
    {
        auto v = ConstantInt::get(*ctx, APInt(sizeof(d.second) * 8, d.second));
        addValueBinding(d.first, v);
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
        throw CompilationError("Invalid operator " + op);
    }
};

