#!/bin/bash

g++ main.cpp bytecode_vm/bytecode.cpp bytecode_vm/vm.cpp interface/io/repl.cpp lang/frontend/lexer/lexer.cpp utils/string_utils.cpp lang/frontend/parser/parser.cpp lang/frontend/parser/ast.cpp -I. -o test -fsanitize=address -static-libasan -g
