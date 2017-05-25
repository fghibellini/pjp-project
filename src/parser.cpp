
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
            cout << "processing call!" << endl;
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
        if (is_special(current(), "*")) {
            next();
            auto right = parseFactor();
            left = new ast::BinaryOpExpression("*", left, right);
        } else if (is_special(current(), "/")) {
            next();
            auto right = parseFactor();
            left = new ast::BinaryOpExpression("/", left, right);
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
    while (true) {
        if (is_special(current(), "-")) {
            consumeSpecial("-");
            auto right = parseTerm();
            left = new ast::BinaryOpExpression("-", left, right);
        } else if (is_special(current(), "+")) {
            consumeSpecial("+");
            auto right = parseTerm();
            left = new ast::BinaryOpExpression("+", left, right);
        } else {
            break;
        }
    }
    return left;
}

ast::Statement * Parser::parseStatement()
{
    lex::Lexem s = current();
    if (is_ident_like(s)) {
        return parseExpr();
        string ident = consume_ident();
        if (is_special(current(), ":=")) {
            next();
            auto expr = parseExpr();
            return new ast::AssignmentStatement(ident, expr);
        //}
        //else if (is_special(current(), "(")) {
        //    next();
        //    auto arg = parseExpr();
        //    consumeSpecial(")");
        //    return new ast::CallStatement(ident, expr);
        //} else if (is_special(current(), "[")) {
        //    next();
        //    auto index = parseExpr();
        //    consumeSpecial("]");
        //    return new ast::ArrayAccess(ident, index);
        } else {
            throw ParseError("Invalid symbol after identifier in statement expected one of { [, (, := }!", current());
        }
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
    } else {
        throw ParseError("Invalid start of statement!", s);
        //return new ast::EmptyStatement();
    }
}

ast::Block * Parser::parseBlock()
{
    consumeSpecial("begin");
    vector<ast::Statement *> statements;
    while (true) {
        statements.push_back(parseStatement());
        if (is_special(current(), ";")) {
            consumeSpecial(";");
        } else {
            consumeSpecial("end");
            break;
        }
    }
    return new ast::Block(statements);
}

ast::Scope * Parser::parseScope()
{
    vector<ast::Declaration *> declarations;
    while (true)
    {
        lex::Lexem s = current();
        if (lex::is_special(s, "const")) {
            next();
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
            consumeSpecial(";");
            declarations.push_back(new ast::ConstDeclaration(bindings));
        } else if (lex::is_special(s, "var")) {
            next();
            vector<string> varnames;
            varnames.push_back(consume_ident());
            while (is_special(current(), ","))
            {
                next();
                varnames.push_back(consume_ident());
            }
            consumeSpecial(";");
            declarations.push_back(new ast::VarDeclaration(varnames));
        } else {
            break;
        }
    }

    auto body = parseBlock();

    cout << "parsed block!" << endl;

    return new ast::Scope(declarations, body);
}

ast::Program *Parser::parse()
{
    _lexem_reader.init();
    next();

    cout << "lexer initialized!" << endl;

    consumeSpecial("program");
    string name = consume_ident();
    consumeSpecial(";");

    lex::Lexem s = current();

    cout << "gonna parse function decls" << endl;
    vector<ast::FunctionDecl *> functions;
    vector<ast::ProcedureDecl *> procedures;
    while (false)
    {
        if (lex::is_special(s, "function")) {
            // TODO
        } else if (lex::is_special(s, "procedure")) {
            // TODO
        }
        s = current();
    }

    if (!(lex::is_special(s, "begin") || lex::is_special(s, "var") || lex::is_special(s, "const"))) {
        throw ParseError("Expected function, procedure or scope.", s);
    } else {
        cout << "gonna parse scope of program: " << name << endl;
        ast::Scope * main = parseScope();

        if (!lex::is_epsilon(current())) {
            throw ParseError("Expected end of file!", current());
        }

        cout << "parsed program!" << endl;
        return new ast::Program(name, functions, procedures, main);
    }
}
