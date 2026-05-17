#pragma once
#include <string>
#include <vector>

enum class TkTp { NUM, ID, PLUS, MINUS, MUL, DIV, EQ, LT, ASSIGN, IF, ELSE, WHILE, PRINT, INPUT, END, LET, LBRACE, RBRACE };

struct Tk {
    TkTp t;
    std::string v;
};

class Lx {
    std::string s;
    size_t p;
public:
    Lx(const std::string& in);
    std::vector<Tk> tks();
};