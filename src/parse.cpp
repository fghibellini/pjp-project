
#include <iostream>
#include <cstring>

#include "lex.h"
#include "parser.h"
#include "print-visitor.h"

using namespace std;

int main(int argc, const char *argv[])
{
    if (argc == 1) {
        lex::LexemReader lreader(cin);
        Parser parser(lreader);

        auto res = parser.parse();
        PrintVisitor coutAstPrinter(cout);
        res.accept(coutAstPrinter);
        
        return 0;
    } else if (argc == 2 && strcmp(argv[1], "--lex") == 0) {
        lex::LexemReader lreader(cin);

        lreader.read_all();

        return 0;
    } else {
        cerr << "Usage:" << endl;
        cerr << "\tparse prog1.mila" << endl;
        cerr << "\tparse --lex prog1.mila" << endl;
        return (argc == 2 && strcmp(argv[1], "-h") == 0) ? 0 : 1;
    }
}
