// Copyright (C) 2024, leleneme on github.com
// This file is part of brainfck, which is free software:
// you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "interpreter.hpp"

// TOOD(lem): Add optional runtime check for (over|under)flows
std::optional<interpreter_error> interpreter::interpret() {
    while (location < tokens.size()) {
        auto token = tokens[location];
        switch (token.type) {
        case token_type::add:
            memory[pointer] += token.operand;
            location++;
            break;
        case token_type::sub:
            memory[pointer] -= token.operand;
            location++;
            break;
        case token_type::shift_left:
            if (static_cast<ssize>(pointer - token.operand) < 0)
                return interpreter_error::out_of_bounds;

            pointer -= token.operand;
            location++;
            break;
        case token_type::shift_right:
            if (pointer + token.operand > sizeof(memory) + 1)
                return interpreter_error::out_of_bounds;

            pointer += token.operand;
            location++;
            break;
        case token_type::write:
            for (usize i = 0; i < token.operand; i++) {
                putchar(static_cast<char>(memory[pointer]));
            }
            location++;
            break;
        case token_type::read:
            for (usize i = 0; i < token.operand; i++) {
                memory[pointer] = static_cast<char>(getchar());
            }
            location++;
            break;
        case token_type::jmp_z: { // [
            if (memory[pointer] == 0) {
                location = token.operand;
            } else {
                location++;
            }
        } break;
        case token_type::jmp_nz: {
            if (memory[pointer] != 0) {
                location = token.operand;
            } else {
                location++;
            }
        } break;
        default: {
        }
        }
    }

    return {};
}