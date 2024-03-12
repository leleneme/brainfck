// Copyright (C) 2024, leleneme on github.com
// This file is part of brainfck, which is free software:
// you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "lexer.hpp"
#include "interpreter.hpp"
#include "compiler_c.hpp"
#include "compiler_qbe.hpp"

#include <unistd.h>
#include <cassert>

enum class program_mode {
    compile_c,
    compile_qbe,
    run,
};

// reads entire file to a std::string, if the file fails to open, std::nullopt
// is return
std::optional<std::string> read_entire_file(std::string path);
// converts index/location of character in a file to a (line, column) pair
std::pair<usize, usize> position_to_line_column(std::string source, usize position);

void usage(std::vector<std::string> args, int exit_code = 1, std::FILE* output = stderr) {
    fprintf(output, "Usage: %s <c|qbe|run> <file.bf>\n", args.at(0).c_str());
    exit(exit_code);
}

int main(const int argc, const char** argv) {
    std::vector<std::string> args(argv, argv + argc);

    if (args.size() <= 2) {
        usage(args);
    }

    program_mode mode;
    std::string command = args[1];

    if (command == "c") {
        mode = program_mode::compile_c;
    } else if (command == "qbe") {
        mode = program_mode::compile_qbe;
    } else if (command == "run") {
        mode = program_mode::run;
    } else {
        fprintf(stderr, "error: unknow command '%s'\n", command.c_str());
        usage(args);
    }

    std::string file_path = args[2];
    std::optional<std::string> source;

    if (file_path == std::string("-")) {
        constexpr usize buffer_size = 256;
        char tmp_buffer[buffer_size] = { 0 };

        std::string data {};
        while (read(STDIN_FILENO, tmp_buffer, buffer_size) > 0) {
            data.append(tmp_buffer);
        }

        file_path = "<stdin>"; // to make lexer errors more readable
        source = data;
    } else {
        source = read_entire_file(file_path);
    }

    if (!source) {
        fprintf(stderr, "error: %s: file not found\n", file_path.c_str());
        return 1;
    }

    auto lex = lexer(source.value());
    lexer_result result = lex.get_tokens();

    if (std::holds_alternative<lexer_error>(result)) {
        auto error = std::get<lexer_error>(result);
        auto lc = position_to_line_column(source.value(), error.position);
        auto base_filename = file_path.substr(file_path.find_last_of("/\\") + 1);

        switch (error.type) {
        case lexer_error_type::unmatched_loop: {
            fprintf(stderr, "error: %s:%zu:%zu: unmatched loop end (']')", base_filename.c_str(), lc.first, lc.second);
            return 1;
        }
        default:
            assert(false && "Unhandled lexer error");
        }
    }

    auto tokens = std::get<std::vector<token>>(result);

    switch (mode) {
    case program_mode::compile_qbe: {
        auto qbe = compiler_qbe(std::move(tokens));
        qbe.compile();
    } break;
    case program_mode::compile_c: {
        auto cc = compiler_c(std::move(tokens));
        cc.compile();
    } break;
    case program_mode::run: {
        auto interp = interpreter(std::move(tokens));
        auto error = interp.interpret();
        if (error) {
            switch (error.value()) {
            case interpreter_error::out_of_bounds:
                fprintf(stderr, "\nruntime error: out of bounds memory tape access\n");
                return 1;
            case interpreter_error::char_overflow:
                fprintf(stderr, "\nruntime error: head overflowed\n");
                return 1;
            }
        }
    } break;
    default:
        assert(false && "unreachable");
    }

    return 0;
}

std::optional<std::string> read_entire_file(std::string path) {
    FILE* fd = fopen(path.c_str(), "r");
    if (!fd)
        return {};

    fseek(fd, 0, SEEK_END);
    usize file_size = static_cast<usize>(ftell(fd));

    std::string contents;
    contents.resize(file_size);

    rewind(fd);
    fread(&contents[0], sizeof(char), file_size, fd);

    fclose(fd);
    return contents;
}

std::pair<usize, usize> position_to_line_column(std::string source, usize position) {
    usize i = 0;
    usize line_count = 1;
    usize col_count = 0;

    while (i < position && i < source.length()) {
        auto c = source[i];
        if (c == '\n') {
            line_count++;
            col_count = 0;
        }

        col_count++;
        i++;
    }

    return { line_count, col_count };
}
