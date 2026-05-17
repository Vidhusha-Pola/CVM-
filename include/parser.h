#pragma once
#include <vector>
#include <memory>
#include <string>
#include "lexer.h"
#include "ast.h"

class Parser {
    std::vector<Tk> tokens;
    int current = 0;

    Tk peek();
    Tk previous();
    bool isAtEnd();
    bool check(TkTp type);
    Tk advance();
    bool match(std::vector<TkTp> types);
    Tk consume(TkTp type, const std::string& message);

    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> expression();

    std::unique_ptr<Stmt> letDeclaration();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> block(); 
    std::unique_ptr<Stmt> statement();

public:
    Parser(const std::vector<Tk>& tks);
    std::vector<std::unique_ptr<Stmt>> parse();
};