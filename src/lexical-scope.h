#ifndef __LEXICAL_SCOPE_H__
#define __LEXICAL_SCOPE_H__

#include "compilation-error.h"

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

using namespace std;
using namespace llvm;

enum BindingType {
    CONSTANT,
    VARIABLE,
    FUNCTION
};

class BindingValue {
    BindingType type;
    Value *v;
    AllocaInst *a;
    Function *f;
public:
    BindingValue(Value *v);
    BindingValue(AllocaInst *a);
    BindingValue(Function *f);
    BindingType getBindingType();
    Value *getValue();
    AllocaInst *getVariable();
    Function *getFunction();
};

struct MaybeBindingValue {
    bool present;
    BindingValue v;
public:
    bool isPresent();
    BindingValue get();
};

class LexicalScope {
    LexicalScope *parent;
    map<string, BindingValue> bindings;
public:
    LexicalScope();
    LexicalScope(LexicalScope *parent);
    void addFunctionBinding(string name, Function *f);
    void addValueBinding(string name, Value *v);
    void addVariableBinding(string name, AllocaInst *a);
    MaybeBindingValue getBinding(string name);
    Function *getFunction(string name);
    Value *getValue(string name);
    AllocaInst *getVariable(string name);
};

#endif
