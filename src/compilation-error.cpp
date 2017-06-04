
#include "compilation-error.h"

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

