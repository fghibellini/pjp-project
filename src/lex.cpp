
#include "lex.h"
#include <iostream>

namespace lex {

string lexem_to_string(Lexem l)
{
    string s = "";
    if (l.type == Lexem::EPSILON) {
        s = "<Epsilon>";
    } else if (l.type == Lexem::IDENT) {
        s = "<Ident:\"" + l.ident + "\">";
    } else if (l.type == Lexem::INT_LITERAL) {
        s = "<IntLiteral:" + to_string(l.int_val) + ">";
    } else if (l.type == Lexem::SPECIAL) {
        s = "<Special:" + l.special_val + ">";
    } else {
        s = "<Invalid>";
    }
    s += " [" + to_string(l.start.line) + "," + to_string(l.start.col) + "]-" +
         "[" + to_string(l.end.line) + "," + to_string(l.end.col) + "]";
    return s;
}

bool is_ident(Lexem l)
{
    return l.type == Lexem::IDENT;
}

bool is_int_literal(Lexem l)
{
    return l.type == Lexem::INT_LITERAL;
}

bool is_epsilon(Lexem l)
{
    return l.type == Lexem::EPSILON;
}

bool is_special(Lexem l, string val)
{
    return l.type == Lexem::SPECIAL && l.special_val == val;
}

Lexem lexem_epsilon(int line, int col)
{
    Lexem l;
    l.type = Lexem::EPSILON;
    l.start.line = line;
    l.start.col = col;
    l.end.line = line;
    l.end.col = col;
    return l;
}

Lexem lexem_special(int start_line, int start_col, string special_val)
{
    Lexem l;
    l.type = Lexem::SPECIAL;
    l.start.line = start_line;
    l.start.col = start_col;
    l.end.line = start_line;
    l.end.col = start_col + special_val.length() - 1;
    l.special_val = special_val;
    return l;
}

Lexem lexem_ident(int start_line, int start_col, string ident)
{
    Lexem l;
    l.type = Lexem::IDENT;
    l.start.line = start_line;
    l.start.col = start_col;
    l.end.line = start_line;
    l.end.col = start_col + ident.length() - 1;
    l.ident = ident;
    return l;
}

Lexem lexem_int_literal(int start_line, int start_col, int length, int int_val)
{
    Lexem l;
    l.type = Lexem::INT_LITERAL;
    l.start.line = start_line;
    l.start.col = start_col;
    l.end.line = start_line;
    l.end.col = start_col + length - 1;
    l.int_val = int_val;
    return l;
}


int LexemReader::cur_char()
{
    return _cur_char;
}

int LexemReader::next_char()
{
    int c = is.get();
    if (is.eof()) {
        c = LexemReader::EOP;
    }
    _cur_char = c;
    if (_cur_is_newline) {
        _cur_line += 1;
    }
    _cur_col = _cur_is_newline ? 0 : (_cur_col + 1);
    _cur_is_newline = _cur_char == '\n';
    return c;
}

void LexemReader::init()
{
    int c = is.get();
    if (is.eof()) {
        c = LexemReader::EOP;
    }
    _cur_char = c;
    _cur_line = 0;
    _cur_col = 0;
}

LexemReader::LexemReader(istream &is) : is(is), _cur_line(0), _cur_col(0), _cur_is_newline(false)
{
}

int intval_of_char(int c)
{
    return c - 48;
}

ParsingError::ParsingError(int line, int col, int c, string what) :
    runtime_error("ParseError: encountered '" + string(1,(char) c) + "' at [" + to_string(line) + "," + to_string(col) + "]" + (what == "" ? "" : " - " + what))
{
}

ParsingError LexemReader::parsing_error(string what)
{
    return ParsingError(_cur_line, _cur_col, _cur_char, what);
}

bool is_keyword(string x)
{
    return (x == "var"
       || x == "const"
       || x == "begin"
       || x == "end"
       || x == "if"
       || x == "then"
       || x == "else"
       || x == "while"
       || x == "do"
       || x == "for"
       || x == "to"
       || x == "downto"
       || x == "writeln"
       || x == "mod"
       || x == "div"
       || x == "function"
       || x == "procedure"
       || x == "program"
       || x == "integer"
       || x == "array"
       || x == "of"
       || x == "and"
       || x == "or"
       || x == "forward"
       || x == "exit"
       || x == "readln");
}

Lexem LexemReader::next()
{
    int c = cur_char();
    while (isspace(c)) {
        c = next_char();
    }
    int start_line = _cur_line;
    int start_col = _cur_col;
    if (c == LexemReader::EOP) {
        return lexem_epsilon(start_line, start_col);
    } else if (isdigit(c)) {
        int char_count = 0;
        int acc = 0;
        do  {
            char_count += 1;
            acc = acc * 10 + intval_of_char(c);
            c = next_char();
        } while (isdigit(c));
        if (isalpha(c)) {
            throw parsing_error("Alpha character directly follows integer literal!");
        }
        return lexem_int_literal(start_line, start_col, char_count, acc);
    } else if (isalpha(c)) {
        string acc = "";
        do  {
            acc = acc + string(1,c);
            c = next_char();
        } while (isalpha(c));
        if (isdigit(c)) {
            throw parsing_error("Digit directly follows identifier or keyword!");
        }
        if (is_keyword(acc)) {
            return lexem_special(start_line, start_col, acc);
        } else {
            return lexem_ident(start_line, start_col, acc);
        }
    } else if (c == '=' || c == '+' || c == '-' || c == '*' || c == ',' || c == ';' || c == '(' || c == ')') {
        next_char();
        return lexem_special(start_line, start_col, string(1,c));
    } else if (c == ':') {
        if (next_char() != '=') {
            throw parsing_error("Expected '='!");
        }
        next_char();
        return lexem_special(start_line, start_col, ":=");
    } else if (c == '<' || c == '>') {
        if (next_char() == '=') {
            next_char();
            return lexem_special(start_line, start_col, string(1, c) + "=");
        } else {
            return lexem_special(start_line, start_col, string(1, c));
        }
    } else {
        throw parsing_error("Expected start of lexem!");
    }
}

void LexemReader::read_all()
{
    init();
    while (true) {
        auto n = next();
        cout << lex::lexem_to_string(n) << endl;
        if (is_epsilon(n)) {
            break;
        }
    }
}

}
