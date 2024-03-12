// Copyright (C) 2024, leleneme on github.com
// This file is part of brainfck, which is free software:
// you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <optional>
#include <variant>

using u8 = uint8_t;
using usize = size_t;
using ssize = ssize_t;

enum class token_type {
    add = '+',
    sub = '-',
    shift_left = '<',
    shift_right = '>',
    write = '.',
    read = ',',
    jmp_z = '[',
    jmp_nz = ']',
};

struct token {
    token_type type;
    usize operand;
};

enum class lexer_error_type {
    unmatched_loop,
};

struct lexer_error {
    lexer_error_type type;
    usize position;
};

using lexer_result = std::variant<std::vector<token>, lexer_error>;

struct lexer {
    const std::string valid_ops = "+-<>,.[]";

    std::string source_;
    usize position_;

    lexer(std::string source)
        : source_(source)
        , position_(0) { }

    bool is_valid_op(char ch) const;
     std::optional<char> next();
     lexer_result get_tokens();
};
