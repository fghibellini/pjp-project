
#include "lexical-scope.h"

BindingValue::BindingValue(Value *v) : type(BindingType::CONSTANT), v(v) {};
BindingValue::BindingValue(AllocaInst *a) : type(BindingType::VARIABLE), a(a) {};
BindingValue::BindingValue(Function *f) : type(BindingType::FUNCTION), f(f) {};
BindingType BindingValue::getBindingType() { return type; };
Value *BindingValue::getValue() { return v; }
AllocaInst *BindingValue::getVariable() { return a; }
Function *BindingValue::getFunction() { return f; };

bool MaybeBindingValue::isPresent() { return present; }
BindingValue MaybeBindingValue::get() { return v; }

LexicalScope::LexicalScope() : parent(nullptr) {};

LexicalScope::LexicalScope(LexicalScope *parent) :parent(parent) {};

void LexicalScope::addFunctionBinding(string name, Function *f)
{
    BindingValue binding(f);
    bindings.emplace(name, binding);
}

void LexicalScope::addValueBinding(string name, Value *v)
{
    BindingValue binding(v);
    bindings.emplace(name, binding);
}

void LexicalScope::addVariableBinding(string name, AllocaInst *a)
{
    BindingValue binding(a);
    bindings.emplace(name, binding);
}

MaybeBindingValue LexicalScope::getBinding(string name)
{
    LexicalScope *current = this;
    while (current != nullptr)
    {
        auto ref = current->bindings.find(name);
        if (ref != current->bindings.end()) {
            MaybeBindingValue ret = { true, ref->second };
            return ret;
        } else {
            current = current->parent;
        }
    }
    MaybeBindingValue ret { false, BindingValue((Value *) nullptr) };
    return ret;
}

Function *LexicalScope::getFunction(string name)
{
    auto binding = getBinding(name);
    if (!binding.isPresent()) {
        return nullptr;
    }
    auto val = binding.get();
    if (val.getBindingType() != BindingType::FUNCTION) {
        throw CompilationError("Expected function for: " + name);
    }
    return val.getFunction();
}

Value *LexicalScope::getValue(string name)
{
    auto binding = getBinding(name);
    if (!binding.isPresent()) {
        return nullptr;
    }
    auto val = binding.get();
    if (val.getBindingType() != BindingType::CONSTANT) {
        throw CompilationError("Expected value!");
    }
    return val.getValue();
}

AllocaInst *LexicalScope::getVariable(string name)
{
    auto binding = getBinding(name);
    if (!binding.isPresent()) {
        return nullptr;
    }
    auto val = binding.get();
    if (val.getBindingType() != BindingType::VARIABLE) {
        throw CompilationError("Expected variable!");
    }
    return val.getVariable();
}

