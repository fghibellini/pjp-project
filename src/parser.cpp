
#include "parser.h"
#include <iostream>

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
        throw runtime_error("expected " + special_val + "!"); // TODO include location
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
        throw runtime_error("expected identifier!"); // TODO include location
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
        throw runtime_error("expected integer literal!"); // TODO include location
    }
}

unique_ptr<ast::Expr> Parser::parseFactor()
{
    if (is_ident(current())) {
        string ident = consume_ident();
        return make_unique<ast::IdentExpr>(move(ident));
    } else if (is_int_literal(current())) {
        int val = consume_int_literal();
        return make_unique<ast::IntExpr>(val);
    } else if (is_special(current(), "(")) {
        consumeSpecial("(");
        auto e = parseExpr();
        consumeSpecial(")");
        return e;
    } else {
        throw runtime_error("invalid start of factor!"); // TODO include location
    }
}

unique_ptr<ast::Expr> Parser::parseTerm()
{
    auto left = parseFactor();
    while (true)
    {
        if (is_special(current(), "*")) {
            next();
            auto right = parseFactor();
            left = make_unique<ast::BinaryOpExpression>("*", move(left), move(right));
        } else if (is_special(current(), "/")) {
            next();
            auto right = parseFactor();
            left = make_unique<ast::BinaryOpExpression>("/", move(left), move(right));
        } else {
            break;
        }
    }
    return left;
}

unique_ptr<ast::Expr> Parser::parseExpr()
{
    unique_ptr<ast::Expr> t0;
    if (is_special(current(), "-")) {
        consumeSpecial("-");
        auto t0_ = parseTerm();
        t0 = make_unique<ast::UnaryMinusExpression>(move(t0_));
    } else {
        t0 = parseTerm();
    }
    auto left = move(t0);
    while (true) {
        if (is_special(current(), "-")) {
            consumeSpecial("-");
            auto right = parseTerm();
            left = make_unique<ast::BinaryOpExpression>("-", move(left), move(right));
        } else if (is_special(current(), "+")) {
            consumeSpecial("+");
            auto right = parseTerm();
            left = make_unique<ast::BinaryOpExpression>("+", move(left), move(right));
        } else {
            break;
        }
    }
    return left;
}

unique_ptr<ast::Statement> Parser::parseStatement()
{
    lex::Lexem s = current();
    if (is_ident(s)) {
        string ident = consume_ident();
        if (is_special(current(), ":=")) {
            next();
            auto expr = parseExpr();
            return make_unique<ast::AssignmentStatement>(ident, move(expr));
        //}
        //else if (is_special(current(), "(")) {
        //    next();
        //    auto arg = parseExpr();
        //    consumeSpecial(")");
        //    return make_unique<ast::CallStatement>(ident, move(expr));
        //} else if (is_special(current(), "[")) {
        //    next();
        //    auto index = parseExpr();
        //    consumeSpecial("]");
        //    return make_unique<ast::ArrayAccess>(ident, move(index));
        } else {
            throw runtime_error("Invalid symbol after identifier in statement expected one of { [, (, := }!");
        }
    } else if (is_special(s, "write")) {
        next();
        auto expr = parseExpr();
        return make_unique<ast::WriteStmt>(move(expr));
    } else if (is_special(s, "if")) {
        next();
        auto condition = parseExpr();
        consumeSpecial("then");
        auto trueBranch = parseStatement();
        unique_ptr<ast::Statement> elseBranch = nullptr;
        if (is_special(current(), "else")) {
            consumeSpecial("else");
            elseBranch = parseStatement();
        }
        return make_unique<ast::IfStatement>(move(condition), move(trueBranch), move(elseBranch));
    } else if (is_special(s, "while")) {
        next();
        auto condition = parseExpr();
        consumeSpecial("do");
        auto body = parseStatement();
        return make_unique<ast::WhileStatement>(move(condition), move(body));
    } else if (is_special(s, "begin")) {
        return parseBlock();
    } else {
        throw runtime_error("Invalid start of statement!");
        //return make_unique<ast::EmptyStatement>();
    }
}

unique_ptr<ast::Block> Parser::parseBlock()
{
    consumeSpecial("begin");
    vector<unique_ptr<ast::Statement>> statements;
    statements.push_back(parseStatement());
    while (is_special(current(), ";")) {
        statements.push_back(parseStatement());
    }
    consumeSpecial("end");
    return make_unique<ast::Block>(move(statements));
}

ast::Program Parser::parse()
{
    _lexem_reader.init();
    next();

    vector<unique_ptr<ast::Declaration>> declarations;
    while (true) {
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
            declarations.push_back(make_unique<ast::ConstDeclaration>(move(bindings)));
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
            declarations.push_back(make_unique<ast::VarDeclaration>(move(varnames)));
        } else {
            break;
        }
    }

    auto body = parseBlock();

    if (!lex::is_epsilon(current())) {
        throw runtime_error("Expected end of file!");
    }

    return ast::Program(move(declarations), move(*body.release()));
}
