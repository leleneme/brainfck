// Copyright (C) 2024, leleneme on github.com
// This file is part of brainfck, which is free software:
// you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "compiler_qbe.hpp"
#include <format>

namespace {

template <class... Args>
inline void print(std::format_string<Args...> fmt, Args&&... args) {
    std::string s = std::vformat(fmt.get(), std::make_format_args(args...));
    printf("\t%s\n", s.c_str());
}

}

void compiler_qbe::compile() {
    printf("data $memory = align 1 { b 0, z 29999 }\n");
    printf("data $head   = align 8 { l 0, }\n");
    printf("export function w $main() {\n");
    printf("@start\n");

    usize n = 1;

    while (location < tokens.size()) {
        auto token = tokens[location];
        switch (token.type) {
        case token_type::add:
            print("%.{} =l loadl $head", n);
            print("%.{} =l add $memory, %.{}", n + 1, n);
            print("%.{} =w loadsb %.{}", n + 2, n + 1);
            print("%.{} =w extsb %.{}", n + 3, n + 2);
            print("%.{} =w add %.{}, {}", n + 4, n + 3, token.operand);
            print("storeb %.{}, %.{}", n + 4, n + 1);
            n += 5;
            break;
        case token_type::sub:
            print("%.{} =l loadl $head", n);
            print("%.{} =l add $memory, %.{}", n + 1, n);
            print("%.{} =w loadsb %.{}", n + 2, n + 1);
            print("%.{} =w extsb %.{}", n + 3, n + 2);
            print("%.{} =w sub %.{}, {}", n + 4, n + 3, token.operand);
            print("storeb %.{}, %.{}", n + 4, n + 1);
            n += 5;
            break;
        case token_type::shift_left:
            print("%.{} =l loadl $head", n);
            print("%.{} =l extsw {}", n + 1, token.operand);
            print("%.{} =l sub %.{}, %.{}", n + 2, n, n + 1);
            print("storel %.{}, $head", n + 2);
            n += 3;
            break;
        case token_type::shift_right:
            print("%.{} =l loadl $head", n);
            print("%.{} =l extsw {}", n + 1, token.operand);
            print("%.{} =l add %.{}, %.{}", n + 2, n, n + 1);
            print("storel %.{}, $head", n + 2);
            n += 3;
            break;
        case token_type::write:
            for (usize i = 0; i < token.operand; i++) {
                print("%.{} =l loadl $head", n);
                print("%.{} =l add $memory, %.{}", n + 1, n);
                print("%.{} =w loadsb %.{}", n + 2, n + 1);
                print("%.{} =w extsb %.{}", n + 3, n + 2);
                print("%.{} =w call $putchar(w %.{})", n + 4, n + 3);
                n += 5;
            }
            break;
        case token_type::read:
            for (usize i = 0; i < token.operand; i++) {
                print("%.{} =w call $getchar()", n);
                print("%.{} =l loadl $head", n + 1, n);
                print("%.{} =l add $memory, %.{}", n + 2, n + 1);
                print("storeb %.{}, %.{}", n, n + 2);
                n += 3;
            }
            break;
        case token_type::jmp_z:
            printf("@loop_cond.%zu\n", location);
            print("%.{} =l loadl $head", n);
            print("%.{} =l add $memory, %.{}", n + 1, n);
            print("%.{} =w loadsb %.{}", n + 2, n + 1);
            print("%.{} =w extsb %.{}", n + 3, n + 2);
            print("jnz %.{}, @loop_body.{}, @loop_join.{}", n + 3, location, location);
            printf("@loop_body.%zu\n", location);
            n += 4;
            break;
        case token_type::jmp_nz:
            print("jmp @loop_cond.{}", token.operand);
            printf("@loop_join.%zu\n", token.operand);
            break;
        default:
            break;
        }
        location++;
    }

    print("ret 0");
    printf("}\n");
}
