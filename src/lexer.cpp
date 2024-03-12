// Copyright (C) 2024, leleneme on github.com
// This file is part of brainfck, which is free software:
// you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "lexer.hpp"
#include <stack>

bool lexer::is_valid_op(char ch) const { return valid_ops.find(ch) != std::string::npos; }

std::optional<char> lexer::next() {
    while (position_ <= source_.length()) {
        char current = source_[position_];
        position_++;
        if (is_valid_op(current)) {
            return current;
        }
    }

    return std::nullopt;
}

lexer_result lexer::get_tokens() {
    std::stack<usize> stack {};
    std::vector<token> tokens {};

    auto c = next();
    while (c != std::nullopt) {
        switch (c.value()) {
        case '.':
        case ',':
        case '<':
        case '>':
        case '-':
        case '+': {
            usize times = 1;
            auto nextc = next();
            while (nextc == c) {
                times++;
                nextc = next();
            }

            tokens.push_back({
                .type = static_cast<token_type>(c.value()),
                .operand = times,
            });

            c = nextc;
        } break;
        case '[': {
            usize fromaddr = tokens.size();
            tokens.push_back({
                .type = token_type::jmp_z,
                .operand = 0,
            });
            stack.push(fromaddr);

            c = next();
        } break;
        case ']': {
            if (stack.size() == 0) {
                return lexer_error {
                    .type = lexer_error_type::unmatched_loop,
                    .position = position_,
                };
            }

            usize destaddr = stack.top();
            stack.pop();

            tokens.push_back({
                .type = token_type::jmp_nz,
                .operand = destaddr,
            });
            tokens[destaddr].operand = tokens.size() - 1;

            c = next();
        } break;
        default:
            break;
        }
    }

    return tokens;
}