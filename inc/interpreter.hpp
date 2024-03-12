// Copyright (C) 2024, leleneme on github.com
// This file is part of brainfck, which is free software:
// you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "lexer.hpp"

enum class interpreter_error {
    out_of_bounds,
    char_overflow,
};

struct interpreter {
    u8 memory[30'000] = { 0 };
    usize pointer = 0;
    usize location = 0;
    std::vector<token> tokens;

    interpreter(std::vector<token> token_list)
        : tokens(token_list) { }

    std::optional<interpreter_error> interpret();
};
