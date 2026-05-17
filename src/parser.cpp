#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Tk>& tks) : tokens(tks) {}

Tk Parser::peek() { return tokens[current]; }
Tk Parser::previous() { return tokens[current - 1]; }
bool Parser::isAtEnd() { return peek().t == TkTp::END; }
bool Parser::check(TkTp type) { if (isAtEnd()) return false; return peek().t == type; }
Tk Parser::advance() { if (!isAtEnd()) current++; return previous(); }

bool Parser::match(std::vector<TkTp> types) {
    for (TkTp type : types) {
        if (check(type)) { advance(); return true; }
    }
    return false;
}

Tk Parser::consume(TkTp type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}


std::unique_ptr<Expr> Parser::primary() {
    if (match({TkTp::NUM})) return std::make_unique<NumExpr>(std::stoi(previous().v));
    if (match({TkTp::ID})) return std::make_unique<VarExpr>(previous().v);
    if (match({TkTp::INPUT})) return std::make_unique<InputExpr>();
    throw std::runtime_error("Expect expression.");
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = primary();
    while (match({TkTp::MUL, TkTp::DIV})) {
        TkTp op = previous().t;
        auto right = primary();
        expr = std::make_unique<BinExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match({TkTp::PLUS, TkTp::MINUS})) {
        TkTp op = previous().t;
        auto right = factor();
        expr = std::make_unique<BinExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match({TkTp::LT})) {
        TkTp op = previous().t;
        auto right = term();
        expr = std::make_unique<BinExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    while (match({TkTp::EQ})) {
        TkTp op = previous().t;
        auto right = comparison();
        expr = std::make_unique<BinExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::expression() { return equality(); }



std::unique_ptr<Stmt> Parser::letDeclaration() {
    Tk name = consume(TkTp::ID, "Expect variable name.");
    consume(TkTp::ASSIGN, "Expect '=' after variable name.");
    auto initializer = expression();
    return std::make_unique<LetStmt>(name.v, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::printStatement() {
    auto value = expression();
    return std::make_unique<PrintStmt>(std::move(value));
}


std::unique_ptr<Stmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> stmts;
    while (!check(TkTp::RBRACE) && !isAtEnd()) {
        if (match({TkTp::LET})) stmts.push_back(letDeclaration());
        else stmts.push_back(statement());
    }
    consume(TkTp::RBRACE, "Expect '}' after block.");
    return std::make_unique<BlockStmt>(std::move(stmts));
}


std::unique_ptr<Stmt> Parser::statement() {
    if (match({TkTp::PRINT})) return printStatement();
    if (check(TkTp::ID)) {
        Tk name = advance(); 
        consume(TkTp::ASSIGN, "Expect '=' after variable name.");
        auto value = expression();
        return std::make_unique<AssignStmt>(name.v, std::move(value));
    }
    if (match({TkTp::IF})) {
        auto condition = expression();
        consume(TkTp::LBRACE, "Expect '{' before if body.");
        auto thenBranch = block();
        std::unique_ptr<Stmt> elseBranch = nullptr;
        if (match({TkTp::ELSE})) {
            consume(TkTp::LBRACE, "Expect '{' before else body.");
            elseBranch = block();
        }
        return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
    }

    if (match({TkTp::WHILE})) {
        auto condition = expression();
        consume(TkTp::LBRACE, "Expect '{' before while body.");
        auto body = block();
        return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    }
    
    throw std::runtime_error("Expect statement.");
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        if (match({TkTp::LET})) statements.push_back(letDeclaration());
        else statements.push_back(statement());
    }
    return statements;
}

