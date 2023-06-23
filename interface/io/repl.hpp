#pragma once


#include <string>
#include <string_view>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <streambuf>

#include <utils/string_utils.hpp>
#include <bytecode_vm/vm.hpp>


namespace pine_interface {
void repl();
void run_file(const char* filename);
}


