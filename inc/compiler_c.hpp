// Copyright (C) 2024, leleneme on github.com
// This file is part of brainfck, which is free software:
// you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "lexer.hpp"

struct compiler_c {
    std::vector<token> tokens;
    usize location = 0;

    compiler_c(std::vector<token> token_list)
        : tokens(token_list) { }

    void compile();
};
