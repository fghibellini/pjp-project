
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
    cerr << "\tmilac <input_file>" << endl;
    cerr << "\tmilac --lex <input_file>" << endl;
    cerr << "\tmilac --ast <input_file>" << endl;
    cerr << "\tmilac --ir <input_file>" << endl;
}

int main(int argc, const char *argv[])
{
    bool lex = false;
    bool ast = false;
    bool ir = false;
    string filename = "";
    string out = "";

    try {
        for (int i = 1; i < argc; i++)
        {
            string arg_val = string(argv[i]);
            if (arg_val == "--lex") {
                lex = true;
            } else if (arg_val == "--ast") {
                ast = true;
            } else if (arg_val == "--ir") {
                ir = true;
            } else if (arg_val == "-h" || arg_val == "--help") {
                print_help();
                return 0;
            } else if (arg_val == "-o") {
                i++;
                if (i >= argc) {
                    throw "Expected filename after -o.";
                }
                out = string(argv[i]);
                continue;
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

    if (((ast ? 1 : 0) + (lex ? 1 : 0) + (ir ? 1 : 0)) > 1) {
        cerr << "You cannot specify more than one of --lex, --ast, --ir" << endl;
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
    } else if (ir) {
        /*
        lex::LexemReader lreader(cin);
        Parser parser(lreader);

        auto res = parser.parse();
        */

        auto res = new ast::Program(
            "prog1",
            vector<ast::FunctionDecl *>(),
            vector<ast::ProcedureDecl *>(),
            new ast::Scope(
                vector<ast::Declaration *>(),
                new ast::Block(
                    vector<ast::Statement *>(1, new ast::IntExpr(42))
                )
            )
        );

        CompilerVisitor compilerVisitor;
        res->accept(compilerVisitor);
        compilerVisitor.dumpIR();
    } else {
        /*
        lex::LexemReader lreader(cin);
        Parser parser(lreader);

        auto res = parser.parse();
        */

        cout << "Output file: " << out << endl;
        auto res = new ast::Program(
            "prog1",
            vector<ast::FunctionDecl *>(),
            vector<ast::ProcedureDecl *>(),
            new ast::Scope(
                vector<ast::Declaration *>(),
                new ast::Block(
                    vector<ast::Statement *>(1, new ast::IntExpr(42))
                )
            )
        );

        CompilerVisitor compilerVisitor;
        res->accept(compilerVisitor);
        compilerVisitor.generateObject(out);
    }

    return 0;
}
