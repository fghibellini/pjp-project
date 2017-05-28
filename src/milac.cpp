
#include <iostream>
#include <cstring>

#include "lex.h"
#include "parser.h"
#include "print-visitor.h"

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
    string filename = "";

    try {
        for (int i = 1; i < argc; i++)
        {
            string arg_val = string(argv[i]);
            if (arg_val == "--lex") {
                lex = true;
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

    if (!lex) {
        lex::LexemReader lreader(cin);
        Parser parser(lreader);

        auto res = parser.parse();
        PrintVisitor coutAstPrinter(cout);
        res->accept(coutAstPrinter);
    } else {
        lex::LexemReader lreader(cin);

        lreader.read_all();
    }

    return 0;
}
