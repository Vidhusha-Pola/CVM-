#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "ast.h"
#include "opcodes.h"

class Compiler {
    std::vector<uint8_t> bytecode;
    std::unordered_map<std::string, uint8_t> symbol_table;
    uint8_t next_var_id = 0;

    void emitByte(uint8_t byte);
    void emitOp(Opcode op);
    void emitInt(int value);
    int emitJump(Opcode instruction); 
    void patchJump(int offset);       

    void compileExpr(Expr* expr);
    void compileStmt(Stmt* stmt);

public:
    std::vector<uint8_t> compile(const std::vector<std::unique_ptr<Stmt>>& ast);
    void disassemble(); 
};