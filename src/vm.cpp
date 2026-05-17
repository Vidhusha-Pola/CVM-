#include "vm.h"
#include <iostream>
#include <stdexcept>

VM::VM(const std::vector<uint8_t>& bytecode) : code(bytecode) {
    globals.resize(256, 0); 
}

void VM::push(int val) { stack.push_back(val); }
int VM::pop() { 
    if (stack.empty()) throw std::runtime_error("Stack underflow!");
    int val = stack.back(); stack.pop_back(); return val; 
}

int VM::readInt() {
    int val = (code[ip] << 24) | (code[ip+1] << 16) | (code[ip+2] << 8) | code[ip+3];
    ip += 4; return val;
}

void VM::run() {
    while (ip < code.size()) {
        Opcode instruction = static_cast<Opcode>(code[ip++]);
        
        switch (instruction) {
            case Opcode::CONST: push(readInt()); break;
            case Opcode::ADD: { int b = pop(); int a = pop(); push(a + b); break; }
            case Opcode::SUB: { int b = pop(); int a = pop(); push(a - b); break; }
            case Opcode::MUL: { int b = pop(); int a = pop(); push(a * b); break; }
            case Opcode::DIV: { int b = pop(); int a = pop(); push(a / b); break; }
            case Opcode::EQ:  { int b = pop(); int a = pop(); push(a == b ? 1 : 0); break; }
            case Opcode::LT:  { int b = pop(); int a = pop(); push(a < b ? 1 : 0); break; }
            
            case Opcode::SET_VAR: globals[code[ip++]] = pop(); break;
            case Opcode::GET_VAR: push(globals[code[ip++]]); break;
            
            case Opcode::PRINT: std::cout << pop() << "\n"; break;
            case Opcode::READ: {
                int userInput;
                std::cout << "> "; 
                std::cin >> userInput;
                push(userInput);
                break;
            }
            
            case Opcode::JMP_FALSE: {
                int target = (code[ip] << 8) | code[ip+1]; 
                ip += 2;
                if (pop() == 0) ip = target; 
                break;
            }
            case Opcode::JMP: {
                int target = (code[ip] << 8) | code[ip+1]; 
                ip = target; 
                break;
            }
            
            case Opcode::HALT: return;
            default: throw std::runtime_error("Unknown Opcode!");
        }
    }
}