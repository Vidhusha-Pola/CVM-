#include "lexer.h"
#include <cctype>

Lx::Lx(const std::string& in) : s(in), p(0) {}

std::vector<Tk> Lx::tks() {
    std::vector<Tk> r;
    while (p < s.length()) {
        char c = s[p];
        if (std::isspace(c)) { p++; } 
        else if (std::isdigit(c)) {
            std::string n;
            while (p < s.length() && std::isdigit(s[p])) n += s[p++];
            r.push_back({TkTp::NUM, n});
        } 
        else if (std::isalpha(c)) {
            std::string id;
            while (p < s.length() && (std::isalnum(s[p]) || s[p] == '_')) id += s[p++];
            if (id == "print") r.push_back({TkTp::PRINT, id});
            else if (id == "let") r.push_back({TkTp::LET, id});
            else if (id == "if") r.push_back({TkTp::IF, id});
            else if (id == "else") r.push_back({TkTp::ELSE, id});
            else if (id == "while") r.push_back({TkTp::WHILE, id});
            else if (id == "input") r.push_back({TkTp::INPUT, id});
            else r.push_back({TkTp::ID, id});
        } 
        else {
            switch (c) {
                case '+': r.push_back({TkTp::PLUS, "+"}); p++; break;
                case '-': r.push_back({TkTp::MINUS, "-"}); p++; break;
                case '*': r.push_back({TkTp::MUL, "*"}); p++; break;
                case '/': r.push_back({TkTp::DIV, "/"}); p++; break;
                case '{': r.push_back({TkTp::LBRACE, "{"}); p++; break;
                case '}': r.push_back({TkTp::RBRACE, "}"}); p++; break;
                case '=':
                    if (p + 1 < s.length() && s[p+1] == '=') { r.push_back({TkTp::EQ, "=="}); p += 2; } 
                    else { r.push_back({TkTp::ASSIGN, "="}); p++; }
                    break;
                case '<': r.push_back({TkTp::LT, "<"}); p++; break;
                default: p++; break;
            }
        }
    }
    r.push_back({TkTp::END, ""});
    return r;
}