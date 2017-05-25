#ifndef __PARSER_H__
#define __PARSER_H__

#include <memory>

#include "ast.h"
#include "lex.h"

using namespace std;

class ParseError : public runtime_error
{
    string msg;
    lex::Lexem position;
    string what_msg;
public:
    ParseError(const std::string& what_arg, lex::Lexem pos);
    const char* what() const noexcept;
};

class Parser
{
private:
    // fields
    lex::LexemReader &_lexem_reader;
    lex::Lexem _current;

    // private methods
    lex::Lexem current();
    lex::Lexem next();
    void consumeSpecial(string special_val);
    string consume_ident();
    int consume_int_literal();
    ast::Block *parseBlock();
    ast::Expr *parseFactor();
    ast::Expr *parseTerm();
    ast::Expr *parseExpr();
    ast::Statement *parseStatement();
    ast::Scope *parseScope();

public:
    // constructor
    Parser(lex::LexemReader &is);

    // public methods
    ast::Program *parse();
};

#endif
