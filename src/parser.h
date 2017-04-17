#ifndef __PARSER_H__
#define __PARSER_H__

#include <memory>

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
    lex::Lexem next();
    void consumeSpecial(string special_val);
    string consume_ident();
    int consume_int_literal();
    unique_ptr<ast::Block> parseBlock();
    unique_ptr<ast::Expr> parseFactor();
    unique_ptr<ast::Expr> parseTerm();
    unique_ptr<ast::Expr> parseExpr();
    unique_ptr<ast::Statement> parseStatement();

public:
    // constructor
    Parser(lex::LexemReader &is);

    // public methods
    ast::Program parse();
};

#endif
