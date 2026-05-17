#include "compiler.h"
#include <iostream>
#include <stdexcept>

void Compiler::emitByte(uint8_t byte) { bytecode.push_back(byte); }
void Compiler::emitOp(Opcode op) { emitByte(static_cast<uint8_t>(op)); }
void Compiler::emitInt(int value) {
    emitByte((value >> 24) & 0xFF); emitByte((value >> 16) & 0xFF);
    emitByte((value >> 8) & 0xFF); emitByte(value & 0xFF);
}

int Compiler::emitJump(Opcode instruction) {
    emitOp(instruction);
    emitByte(0); emitByte(0); 
    return bytecode.size() - 2;
}

void Compiler::patchJump(int offset) {
    int target = bytecode.size();
    bytecode[offset] = (target >> 8) & 0xFF;
    bytecode[offset + 1] = target & 0xFF;
}

void Compiler::compileExpr(Expr* expr) {
    if (auto e = dynamic_cast<NumExpr*>(expr)) {
        emitOp(Opcode::CONST); emitInt(e->val);
    } 
    else if (dynamic_cast<InputExpr*>(expr)) {
        emitOp(Opcode::READ);
    }
    else if (auto e = dynamic_cast<VarExpr*>(expr)) {
        if (symbol_table.find(e->name) == symbol_table.end()) throw std::runtime_error("Undefined variable");
        emitOp(Opcode::GET_VAR); emitByte(symbol_table[e->name]);
    }
    else if (auto e = dynamic_cast<BinExpr*>(expr)) {
        compileExpr(e->left.get()); compileExpr(e->right.get());
        switch (e->op) {
            case TkTp::PLUS: emitOp(Opcode::ADD); break;
            case TkTp::MINUS: emitOp(Opcode::SUB); break;
            case TkTp::MUL: emitOp(Opcode::MUL); break;
            case TkTp::DIV: emitOp(Opcode::DIV); break;
            case TkTp::EQ: emitOp(Opcode::EQ); break;
            case TkTp::LT: emitOp(Opcode::LT); break;
            default: break;
        }
    }
}


void Compiler::compileStmt(Stmt* stmt) {
    if (auto s = dynamic_cast<LetStmt*>(stmt)) {
        compileExpr(s->initializer.get());
        if (symbol_table.find(s->name) == symbol_table.end()) symbol_table[s->name] = next_var_id++;
        emitOp(Opcode::SET_VAR); emitByte(symbol_table[s->name]);
    } 
    else if (auto s = dynamic_cast<AssignStmt*>(stmt)) {
        compileExpr(s->value.get());
        if (symbol_table.find(s->name) == symbol_table.end()) throw std::runtime_error("Undefined variable!");
        emitOp(Opcode::SET_VAR); emitByte(symbol_table[s->name]);
    }
    else if (auto s = dynamic_cast<PrintStmt*>(stmt)) {
        compileExpr(s->expr.get()); emitOp(Opcode::PRINT);
    }
    else if (auto s = dynamic_cast<BlockStmt*>(stmt)) {
        for (const auto& blockStmt : s->statements) compileStmt(blockStmt.get());
    }
    else if (auto s = dynamic_cast<IfStmt*>(stmt)) {
        compileExpr(s->condition.get());
        int jumpFalse = emitJump(Opcode::JMP_FALSE);
        compileStmt(s->thenBranch.get());
        
        if (s->elseBranch) {
            int jumpEnd = emitJump(Opcode::JMP);
            patchJump(jumpFalse);
            compileStmt(s->elseBranch.get());
            patchJump(jumpEnd);
        } else {
            patchJump(jumpFalse);
        }
    }
    else if (auto s = dynamic_cast<WhileStmt*>(stmt)) {
        int loopStart = bytecode.size();
        compileExpr(s->condition.get());
        int jumpExit = emitJump(Opcode::JMP_FALSE);
        compileStmt(s->body.get());
        
        emitOp(Opcode::JMP);
        emitByte((loopStart >> 8) & 0xFF); emitByte(loopStart & 0xFF);
        
        patchJump(jumpExit);
    }
}

std::vector<uint8_t> Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& ast) {
    for (const auto& stmt : ast) compileStmt(stmt.get());
    emitOp(Opcode::HALT);
    return bytecode;
}


void Compiler::disassemble() {
    std::cout << "\n--- COMPILED BYTECODE ---\n";
    for (size_t i = 0; i < bytecode.size(); ) {
        std::cout << i << "\t";
        Opcode op = static_cast<Opcode>(bytecode[i++]);
        
        switch (op) {
            case Opcode::CONST: std::cout << "CONST\n"; i += 4; break;
            case Opcode::ADD: std::cout << "ADD\n"; break;
            case Opcode::SUB: std::cout << "SUB\n"; break;
            case Opcode::MUL: std::cout << "MUL\n"; break;
            case Opcode::DIV: std::cout << "DIV\n"; break;
            case Opcode::EQ: std::cout << "EQ\n"; break;
            case Opcode::LT: std::cout << "LT\n"; break;
            case Opcode::SET_VAR: std::cout << "SET_VAR (id: " << (int)bytecode[i++] << ")\n"; break;
            case Opcode::GET_VAR: std::cout << "GET_VAR (id: " << (int)bytecode[i++] << ")\n"; break;
            case Opcode::PRINT: std::cout << "PRINT\n"; break;
            case Opcode::JMP_FALSE: std::cout << "JMP_FALSE\n"; i += 2; break;
            case Opcode::JMP: std::cout << "JMP\n"; i += 2; break;
            case Opcode::HALT: std::cout << "HALT\n"; break;
            case Opcode::READ: std::cout << "READ\n"; break;
            default: std::cout << "UNKNOWN\n"; break;
        }
    }
    std::cout << "-------------------------\n\n";
}