#pragma once



#include <iostream>
#include <cstdio>


#include "bytecode.hpp"
#include <stack/stack.hpp>
#include <utils/binary_op.hpp>
#include <utils/types.hpp>
#include <lang/frontend/lexer/lexer.hpp>


#define DEBUG_TRACE_EXECUTION




namespace pine_vm {

enum class interpret_result_t : std::uint8_t {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR,
};

struct VM {
    pine_bytecode::chunk_t* chunk; // non-owning pointer to the chunk currently being executed
    unsigned char* ip;
    pine_stl::stack<pine_bytecode::value_t> stack; // TODO: consider making the stack fixed size instead of dynamic on the heap?
};


interpret_result_t interpret(pine_bytecode::chunk_t &chunk);

}


namespace pine_lang {
pine_vm::interpret_result_t interpret(const char* file_contents);
}

