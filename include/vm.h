#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>  
#include "opcodes.h"

class VM {
    std::vector<uint8_t> code;
    std::vector<int> stack;
    std::vector<int> globals;
    size_t ip = 0;

    void push(int val);
    int pop();
    int readInt();

public:
    VM(const std::vector<uint8_t>& bytecode);
    void run();
};