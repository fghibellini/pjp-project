#ifndef __PARSER_H__
#define __PARSER_H__

#include "ast.h"
#include "lex.h"

using namespace std;


class Parser
{
private:
    // fields
    lex::LexemReader &_lexem_reader;
    lex::Lexem _current;

    // private methods
    lex::Lexem current();
    void next();

public:
    // constructor
    Parser(lex::LexemReader &is);

    // public methods
    ast::Program parse();
};

#endif
