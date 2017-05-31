#ifndef __COMPILER_VISITOR_H__
#define __COMPILER_VISITOR_H__

#include <ostream>

#include "ast.h"

//#include "llvm/ADT/APFloat.h"
//#include "llvm/ADT/APInt.h"
//#include "llvm/ADT/STLExtras.h"
//#include "llvm/IR/BasicBlock.h"
//#include "llvm/IR/Constants.h"
//#include "llvm/IR/DerivedTypes.h"
//#include "llvm/IR/Function.h"
//#include "llvm/IR/IRBuilder.h"
//#include "llvm/IR/LLVMContext.h"
//#include "llvm/IR/Module.h"
//#include "llvm/IR/Type.h"
//#include "llvm/IR/Verifier.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"


//#include "llvm/Analysis/Passes.h"
//#include "llvm/ExecutionEngine/GenericValue.h"
//#include "llvm/ExecutionEngine/Interpreter.h"
//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/IR/Constants.h"
//#include "llvm/IR/DerivedTypes.h"
//#include "llvm/IR/IRBuilder.h"
//#include "llvm/IR/Instructions.h"
//#include "llvm/IR/LLVMContext.h"
//#include "llvm/IR/Module.h"
//#include "llvm/IR/Verifier.h"
//#include "llvm/Support/ManagedStatic.h"
//#include "llvm/Support/TargetSelect.h"
//#include "llvm/Support/raw_ostream.h"
//
//#include "llvm/ADT/Triple.h"
//#include "llvm/Analysis/TargetLibraryInfo.h"
//#include "llvm/CodeGen/CommandFlags.h"
//#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
//#include "llvm/CodeGen/LinkAllCodegenComponents.h"
//#include "llvm/IR/DataLayout.h"
//#include "llvm/IR/IRPrintingPasses.h"
//#include "llvm/IR/LLVMContext.h"
//#include "llvm/IR/Module.h"
//#include "llvm/IRReader/IRReader.h"
//#include "llvm/MC/SubtargetFeature.h"
//#include "llvm/Pass.h"
//#include "llvm/IR/LegacyPassManager.h"
//#include "llvm/Support/CommandLine.h"
//#include "llvm/Support/Debug.h"
//#include "llvm/Support/FileSystem.h"
//#include "llvm/Support/FormattedStream.h"
//#include "llvm/Support/Host.h"
//#include "llvm/Support/ManagedStatic.h"
//#include "llvm/Support/PluginLoader.h"
//#include "llvm/Support/PrettyStackTrace.h"
//#include "llvm/Support/Signals.h"
//#include "llvm/Support/SourceMgr.h"
//#include "llvm/Support/TargetRegistry.h"
//#include "llvm/Support/TargetSelect.h"
//#include "llvm/Support/ToolOutputFile.h"
//#include "llvm/Target/TargetMachine.h"
//#include "llvm/Target/TargetSubtargetInfo.h"

using namespace llvm;
using namespace llvm::sys;

struct CompilerVisitor : public ast::Visitor {
private:
    Value *val; // visitor return value
    Function *fn; // visitor return value
    LLVMContext *ctx;
    Module *module;
    IRBuilder<> *builder;
public:
    CompilerVisitor();
    void generate();
    void generateObject(string outputPath);
    virtual void visit(const ast::Program &p);
    virtual void visit(const ast::Args &p);
    virtual void visit(const ast::FunctionDecl &p);
    virtual void visit(const ast::ProcedureDecl &p);
    virtual void visit(const ast::Scope &s);
    virtual void visit(const ast::IntExpr &e);
    virtual void visit(const ast::CallFactor &s);
    virtual void visit(const ast::IndexingFactor &s);
    virtual void visit(const ast::Block &s);
    virtual void visit(const ast::IdentExpr &s);
    virtual void visit(const ast::ConstDeclaration &s);
    virtual void visit(const ast::VarDeclaration &s);
    virtual void visit(const ast::AssignmentStatement &s);
    virtual void visit(const ast::IfStatement &s);
    virtual void visit(const ast::TypeSignature &s);
    virtual void visit(const ast::WhileStatement &s);
    virtual void visit(const ast::ForStatement &s);
    virtual void visit(const ast::EmptyStatement &s);
    virtual void visit(const ast::UnaryMinusExpression &s);
    virtual void visit(const ast::BinaryOpExpression &s);
};

#endif
