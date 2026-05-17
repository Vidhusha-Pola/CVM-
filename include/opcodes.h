#pragma once
#include <cstdint>

enum class Opcode : uint8_t {
    CONST, ADD, SUB, MUL, DIV, EQ, LT,
    SET_VAR, GET_VAR, PRINT, INPUT, JMP_FALSE, JMP, HALT,
    READ
};