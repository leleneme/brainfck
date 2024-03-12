A simple [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) interpreter and
compiler that can output [QBE IL](https://c9x.me/compile/) and C. This
implementation uses 30000 cells with a 8-bit width.

### Building
This projects requires a C++ compiler with C++20 or later support. No external
dependencies are used directly by the executable.

##### With Meson:
```
$ meson setup build/ && ninja -j $(nproc) -C build/
$ ./build/brainfck
```

##### Without Meson:
```
$ c++ -Wall -pedantic --std=c++20 src/*.cpp -Iinc/ -o brainfck
$ ./brainfck
```

### Usage

```
$ brainfck <subcommand> <file>
```

This application has three subcommands: `run`, `c` and `qbe`. `run` interprets
the provided brainfuck program on the fly, while `c` and `qbe` output the
transpilled code to _stdout_.

Since the program outputs code to _stdout_, it can be piped into a C compiler to
create a final executable:
```
$ brainfck c sample.bf | cc -O2 -x c -o out -
```

or with QBE (assuming it can be located by the shell):
```
$ brainfck qbe sample.bf | qbe | cc -O2 -x assembler -o out -
```

The program can also read programs from _stdin_, using `-` as filename:
```
# Outputs 'A'
$ echo "----[---->+<]>++." | ./build/brainfck run - 
```

### License

This program is free software:
you can redistribute it and/or modify it under the terms of
the GNU General Public License as published by the Free Software Foundation.
You should have received a copy of the GNU General Public License
along with this program. If not, see https://www.gnu.org/licenses/.