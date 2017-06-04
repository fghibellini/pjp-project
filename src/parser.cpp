
#include "parser.h"
#include <iostream>

ParseError::ParseError(const std::string& what_arg, lex::Lexem pos)
: runtime_error(what_arg)
, msg(what_arg)
, position(pos)
, what_msg(("PARSING_ERROR [" + msg + "] at token [" + lex::lexem_to_string(position) + "]").c_str())
{
}

bool is_ident_like(lex::Lexem s)
{
    return is_ident(s)
      || is_special(s, "read")
      || is_special(s, "readln")
      || is_special(s, "write")
      || is_special(s, "writeln");
}

const char* ParseError::what() const noexcept
{
    return what_msg.c_str(); 
}

lex::Lexem Parser::current()
{
    return _current;
}

lex::Lexem Parser::next()
{
    _current = _lexem_reader.next();
    return _current;
}

Parser::Parser(lex::LexemReader &is) : _lexem_reader(is)
{
}

void Parser::consumeSpecial(string special_val)
{
    lex::Lexem s = current();
    if (lex::is_special(s, special_val)) {
        next();
    } else {
        throw ParseError("expected " + special_val + "!", s);
    }
}

string Parser::consume_ident()
{
    lex::Lexem s = current();
    if (lex::is_ident(s)) {
        string varname = s.ident;
        next();
        return varname;
    } else {
        throw ParseError("expected identifier!", s);
    }
}

int Parser::consume_int_literal()
{
    lex::Lexem s = current();
    if (lex::is_int_literal(s)) {
        int val = s.int_val;
        next();
        return val;
    } else {
        throw ParseError("expected integer literal!", s);
    }
}

ast::Expr * Parser::parseFactor()
{
    if (is_ident_like(current())) {
        string ident;
        if (is_ident(current())) {
            ident = consume_ident();
        } else {
            ident = special_string_val(current());
            next();
        }
        if (is_special(current(), "(")) {
            consumeSpecial("(");
            vector<ast::Expr *> args;
            while (true) {
                args.push_back(parseExpr());
                if (is_special(current(), ",")) {
                    consumeSpecial(",");
                } else {
                    consumeSpecial(")");
                    break;
                }
            }
            return new ast::CallFactor(ident, args);
        } else if (is_special(current(), "[")) {
            consumeSpecial("[");
            auto index = parseExpr();
            consumeSpecial("]");
            return new ast::IndexingFactor(ident, index);
        } else {
            return new ast::IdentExpr(ident);
        }
    } else if (is_int_literal(current())) {
        int val = consume_int_literal();
        return new ast::IntExpr(val);
    } else if (is_special(current(), "(")) {
        consumeSpecial("(");
        auto e = parseExpr();
        consumeSpecial(")");
        return e;
    } else {
        throw ParseError("invalid start of factor!", current());
    }
}

ast::Expr * Parser::parseTerm()
{
    auto left = parseFactor();
    while (true)
    {
        if (is_special(current(), "*")
         || is_special(current(), "/")
         || is_special(current(), "mod")
         || is_special(current(), "div")) {
            string op = special_string_val(current());
            next();
            auto right = parseFactor();
            left = new ast::BinaryOpExpression(op, left, right);
        } else {
            break;
        }
    }
    return left;
}

ast::Expr * Parser::parseExpr()
{
    ast::Expr * t0;
    if (is_special(current(), "-")) {
        consumeSpecial("-");
        auto t0_ = parseTerm();
        t0 = new ast::UnaryMinusExpression(t0_);
    } else {
        t0 = parseTerm();
    }
    auto left = t0;
    while (is_special(current(), "-")
        || is_special(current(), "+")
        || is_special(current(), "and")
        || is_special(current(), "or")
        || is_special(current(), "<")
        || is_special(current(), ">")
        || is_special(current(), ">=")
        || is_special(current(), "<=")
        || is_special(current(), "=")
        || is_special(current(), "<>")) {
        string op = special_string_val(current());
        next();
        auto right = parseTerm();
        left = new ast::BinaryOpExpression(op, left, right);
    }
    return left;
}

ast::Statement * Parser::parseStatement()
{
    lex::Lexem s = current();
    if (is_special(s, "exit")) {
        next();
        return new ast::CallFactor("exit", vector<ast::Expr *>());
    } else if (is_special(s, "break")) {
        next();
        return new ast::BreakStatement();
    } else if (is_special(s, "for")) {
        next();
        string iterator = consume_ident();
        consumeSpecial(":=");
        auto val0 = parseExpr();
        bool downto = is_special(current(), "downto");
        if (downto) {
            consumeSpecial("downto");
        } else {
            consumeSpecial("to");
        }
        auto val1 = parseExpr();
        consumeSpecial("do");
        auto body = parseStatement();
        return new ast::ForStatement(iterator, val0, downto, val1, body);
    } else if (is_special(s, "if")) {
        next();
        auto condition = parseExpr();
        consumeSpecial("then");
        auto trueBranch = parseStatement();
        ast::Statement * elseBranch = nullptr;
        if (is_special(current(), "else")) {
            consumeSpecial("else");
            elseBranch = parseStatement();
        }
        return new ast::IfStatement(condition, trueBranch, elseBranch);
    } else if (is_special(s, "while")) {
        next();
        auto condition = parseExpr();
        consumeSpecial("do");
        auto body = parseStatement();
        return new ast::WhileStatement(condition, body);
    } else if (is_special(s, "begin")) {
        return parseBlock();
    } else if (is_ident_like(s)) { // TODO identify callable identifiers
        auto left = parseExpr();
        if (is_special(current(), ":=")) {
            next();
            auto right = parseExpr();
            return new ast::AssignmentStatement(left, right);
        }
        return left;
        throw ParseError("Invalid start of statement!", s);
    } else {
        return new ast::EmptyStatement();
    }
}

ast::Block * Parser::parseBlock()
{
    consumeSpecial("begin");
    vector<ast::Statement *> statements;
    while (!is_special(current(), "end")) {
        statements.push_back(parseStatement());
        consumeSpecial(";");
    }
    consumeSpecial("end");
    return new ast::Block(statements);
}

ast::TypeSignature *Parser::parseTypeSignature()
{
    int array = false;
    int i1 = 0;
    int i2 = 0;
    if (is_special(current(), "array")) {
        array = true;
        consumeSpecial("array");
        consumeSpecial("[");
        bool neg1 = false;
        if (is_special(current(), "-")) {
            neg1 = true;
            next();
        }
        int n1 = consume_int_literal();
        consumeSpecial("..");
        bool neg2 = false;
        if (is_special(current(), "-")) {
            neg2 = true;
            next();
        }
        int n2 = consume_int_literal();
        consumeSpecial("]");
        consumeSpecial("of");
        i1 = neg1 ? -n1 : n1;
        i2 = neg2 ? -n2 : n2;
    }
    consumeSpecial("integer");
    return new ast::TypeSignature(array, i1, i2);
}

ast::Scope * Parser::parseScope()
{
    vector<ast::Declaration *> declarations;
    while (true)
    {
        lex::Lexem s = current();
        if (lex::is_special(s, "const")) {
            next();
            do {
                map<string,int> bindings;
                string name0 = consume_ident();
                consumeSpecial("=");
                int val0 = consume_int_literal();
                bindings.emplace(name0,val0);
                while (is_special(current(), ","))
                {
                    next();
                    string name = consume_ident();
                    consumeSpecial("=");
                    int val = consume_int_literal();
                    bindings.emplace(name,val);
                }
                consumeSpecial(":");
                auto type = parseTypeSignature();
                consumeSpecial(";");
                declarations.push_back(new ast::ConstDeclaration(bindings));
            } while (is_ident(current()));
        } else if (lex::is_special(s, "var")) {
            next();
            do {
                vector<string> varnames;
                varnames.push_back(consume_ident());
                while (is_special(current(), ","))
                {
                    next();
                    varnames.push_back(consume_ident());
                }
                consumeSpecial(":");
                auto type = parseTypeSignature();
                consumeSpecial(";");
                declarations.push_back(new ast::VarDeclaration(varnames, type));
            } while (is_ident(current()));
        } else {
            break;
        }
    }

    auto body = parseBlock();

    return new ast::Scope(declarations, body);
}

ast::Args *Parser::parseArgs()
{
    vector<ast::Arg *> args;
    consumeSpecial("(");
    while (!is_special(current(), ")"))
    {
        string name = consume_ident();
        consumeSpecial(":");
        auto type = parseTypeSignature();
        args.push_back(new ast::Arg(name, type));
        if (is_special(current(), ";")) {
            consumeSpecial(";");
        } else {
            break;
        }
    }
    consumeSpecial(")");
    return new ast::Args(args);
}

ast::FunctionDecl *Parser::parseFunctionDecl()
{
    consumeSpecial("function");
    string name = consume_ident();
    auto args = parseArgs();
    consumeSpecial(":");
    auto ret_type = parseTypeSignature();
    consumeSpecial(";");
    auto scope = parseScope();
    return new ast::FunctionDecl(name, args, ret_type, scope);
}

ast::ProcedureDecl *Parser::parseProcedureDecl()
{
    consumeSpecial("procedure");
    string name = consume_ident();
    auto args = parseArgs();
    consumeSpecial(";");
    auto scope = parseScope();
    return new ast::ProcedureDecl(name, args, scope);
}

ast::Program *Parser::parse()
{
    _lexem_reader.init();
    next();

    consumeSpecial("program");
    string name = consume_ident();
    consumeSpecial(";");

    lex::Lexem s = current();

    vector<ast::FunctionDecl *> functions;
    vector<ast::ProcedureDecl *> procedures;
    while (true)
    {
        if (lex::is_special(s, "function")) {
            functions.push_back(parseFunctionDecl());
        } else if (lex::is_special(s, "procedure")) {
            procedures.push_back(parseProcedureDecl());
        } else {
            break;
        }
        consumeSpecial(";");
        s = current();
    }

    ast::Scope * main = parseScope();

    consumeSpecial(".");
    if (!lex::is_epsilon(current())) {
        throw ParseError("Expected end of file!", current());
    }

    return new ast::Program(name, functions, procedures, main);
}
