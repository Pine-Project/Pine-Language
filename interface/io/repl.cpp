#include "repl.hpp"


void pine_interface::repl() {
    std::cout << "> ";
    for(std::string line; std::getline(std::cin, line);) {
        std::cout << '\n';

        pine_lang::interpret(line.c_str());

        std::cout << "> ";
    }
}



void pine_interface::run_file(const char *const filename) {
    // TODO: possibly consider lazily loading the file instead to save memory. This is especially important for large files being compiled with limited RAM.
    const auto file_str = pine_stl::read_file_into_string(filename);

    pine_vm::interpret_result_t result = pine_lang::interpret(file_str.c_str());

    if(result == pine_vm::interpret_result_t::COMPILE_ERROR) {
        throw std::runtime_error("run_file: compile error detected");
    }
    if(result == pine_vm::interpret_result_t::RUNTIME_ERROR) {
        throw std::runtime_error("run_file: runtime error detected");
    }
}



