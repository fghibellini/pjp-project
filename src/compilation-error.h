#ifndef __COMPILATION_ERROR_H__
#define __COMPILATION_ERROR_H__

#include <stdexcept>

using namespace std;

class CompilationError : public runtime_error
{
    string msg;
    string what_msg;
public:
    CompilationError(const std::string& what_arg);
    const char* what() const noexcept;
};

#endif
