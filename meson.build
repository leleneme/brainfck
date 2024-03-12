project('brainfck', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

executable(
  'brainfck',
  'src/interpreter.cpp',
  'src/lexer.cpp',
  'src/compiler_qbe.cpp',
  'src/compiler_c.cpp',
  'src/main.cpp',
  native : true,
  include_directories : include_directories('inc/')
)
