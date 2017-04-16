#ifndef __LEX_H__
#define __LEX_H__

#include <istream>
#include <cctype>
#include <stdexcept>
#include <string>

using namespace std;

namespace lex {

struct Lexem {
    enum LexemType {
        EPSILON,
        IDENT,
        INT_LITERAL,
        SPECIAL
    } type;

    struct Loc {
        int line;
        int col;
    } start, end;

    /* for type = IDENT */
    string ident;

    /* for type = INT_LITERAL */
    int int_val;

    /* for type = SPECIAL */
    string special_val;
};

string lexem_to_string(Lexem l);

bool is_epsilon(Lexem l);

class ParsingError : public runtime_error
{
public:
    ParsingError(int line, int col, int c, string what);
};

class LexemReader
{
private:
    // fields
    istream &is;
    int _cur_char;
    int _cur_line;
    int _cur_col;
    bool _cur_is_newline;

    // private methods
    int cur_char();
    int next_char();
    ParsingError parsing_error(string what);
public:
    static const int EOP = -1;

    // constructor
    LexemReader(istream &is);
    
    // public methods
    void init();
    Lexem next();
    void read_all();
};

}

#endif
