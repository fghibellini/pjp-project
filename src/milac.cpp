
#include <iostream>
#include <cstring>

#include "lex.h"
#include "parser.h"
#include "print-visitor.h"
#include "compiler-visitor.h"

using namespace std;

void print_help()
{
    cerr << "Usage:" << endl;
    cerr << "\tparse prog1.mila" << endl;
    cerr << "\tparse --lex prog1.mila" << endl;
}

int main(int argc, const char *argv[])
{
    bool lex = false;
    bool ast = false;
    string filename = "";

    try {
        for (int i = 1; i < argc; i++)
        {
            string arg_val = string(argv[i]);
            if (arg_val == "--lex") {
                lex = true;
            } else if (arg_val == "--ast") {
                ast = true;
            } else if (arg_val == "-h" || arg_val == "--help") {
                print_help();
                return 0;
            } else if (arg_val[0] == '-') {
                throw "Unknown flag: " + arg_val;
            } else {
                if (filename != "") {
                    throw "Multiple filenames!";
                }
                filename = arg_val;
            }
        }
    } catch (string desc) {
        cerr << "Error: " << desc << endl;
        print_help();
        return 1;
    }

    if (filename != "") {
        cerr << "Currently only stdin input is supported" << endl;
        return 2;
    }

    if (ast && lex) {
        cerr << "You cannot specify both --lex and --ast" << endl;
        return 1;
    }

    if (ast) {
        lex::LexemReader lreader(cin);
        Parser parser(lreader);

        auto res = parser.parse();

        PrintVisitor coutAstPrinter(cout);
        res->accept(coutAstPrinter);
    } else if (lex) {
        lex::LexemReader lreader(cin);

        lreader.read_all();
    } else {
        lex::LexemReader lreader(cin);
        Parser parser(lreader);

        auto res = parser.parse();

        CompilerVisitor compilerVisitor;
        res->accept(compilerVisitor);
        compilerVisitor.generate();
    }

    return 0;
}
