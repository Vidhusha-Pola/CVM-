#pragma once
#include <string>
#include <vector>
#include <memory>
#include "lexer.h"

struct Expr { virtual ~Expr() = default; };

struct NumExpr : Expr { int val; NumExpr(int v) : val(v) {} };
struct InputExpr : Expr { InputExpr() {} };
struct VarExpr : Expr { std::string name; VarExpr(const std::string& n) : name(n) {} };
struct BinExpr : Expr {
    std::unique_ptr<Expr> left; TkTp op; std::unique_ptr<Expr> right;
    BinExpr(std::unique_ptr<Expr> l, TkTp o, std::unique_ptr<Expr> r) : left(std::move(l)), op(o), right(std::move(r)) {}
};

struct Stmt { virtual ~Stmt() = default; };

struct LetStmt : Stmt {
    std::string name; std::unique_ptr<Expr> initializer;
    LetStmt(const std::string& n, std::unique_ptr<Expr> init) : name(n), initializer(std::move(init)) {}
};
struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(const std::string& n, std::unique_ptr<Expr> v) : name(n), value(std::move(v)) {}
};
struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expr;
    PrintStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};
struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts) : statements(std::move(stmts)) {}
};
struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition; std::unique_ptr<Stmt> thenBranch; std::unique_ptr<Stmt> elseBranch;
    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> thenB, std::unique_ptr<Stmt> elseB)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
};
struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition; std::unique_ptr<Stmt> body;
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> b) : condition(std::move(cond)), body(std::move(b)) {}
};