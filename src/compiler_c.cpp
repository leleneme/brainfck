// Copyright (C) 2024, leleneme on github.com
// This file is part of brainfck, which is free software:
// you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "compiler_c.hpp"

#include <format>

// If this is set to 1 (true), all C code will be "pretty printed", which means
// it will be correctly formated with identation. If 0 (false), all C code will
// be printed without any identation in a single line.
#define C_BACKEND_PPRINT 1

static usize ident = 0;

namespace {

template <class... Args>
inline void print(std::format_string<Args...> fmt, Args&&... args) {
    std::string s = std::vformat(fmt.get(), std::make_format_args(args...));

#if C_BACKEND_PPRINT
    for (usize i = 0; i < ident; i++) {
        putchar('\t');
    }
    printf("%s\n", s.c_str());
#else
    printf("%s", s.c_str());
#endif
}

}

void compiler_c::compile() {
    // Using puts for headers to assure that it is not printed in the same line
    // when C_BACKEND_PPRINT is 0
    puts("#include <stdio.h>");
    puts("#include <stddef.h>");

    print("char mem[30000] = {{0}};");
    print("size_t head = 0;");
    print("int main(void) {{");

    ident++;
    while (location < tokens.size()) {
        auto token = tokens[location];
        switch (token.type) {
        case token_type::add:
            print("mem[head]+={};", token.operand);
            break;
        case token_type::sub:
            print("mem[head]-={};", token.operand);
            break;
        case token_type::shift_left:
            print("head-={};", token.operand);
            break;
        case token_type::shift_right:
            print("head+={};", token.operand);
            break;
        case token_type::write:
            for (usize i = 0; i < token.operand; i++) {
                print("putchar(mem[head]);");
            }
            break;
        case token_type::read:
            for (usize i = 0; i < token.operand; i++) {
                print("mem[head] = getchar();");
            }
            break;
        case token_type::jmp_z:
            print("while(mem[head]){{");
            ident++;
            break;
        case token_type::jmp_nz:
            ident--;
            print("}}");
        default:
            break;
        }

        location++;
    }

    print("return 0;");
    ident--;
    print("}}");
}
