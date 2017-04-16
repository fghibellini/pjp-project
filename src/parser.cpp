
#include "parser.h"
#include <iostream>

lex::Lexem Parser::current()
{
    return _current;
}

void Parser::next()
{
    _current = _lexem_reader.next();
}

Parser::Parser(lex::LexemReader &is) : _lexem_reader(is)
{
}

ast::Program Parser::parse()
{
    _lexem_reader.init();
    //do {
    //    next();
    //    cout << lex::lexem_to_string(_current) << endl;
    //} while (!lex::is_epsilon(_current));
    return ast::Program();
}
