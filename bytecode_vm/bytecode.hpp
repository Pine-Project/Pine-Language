#pragma once


#include <cstddef>
#include <vector>
#include <string_view>
#include <iostream>
#include <iomanip>
#include <cassert>

#include <utils/types.hpp>



namespace pine_bytecode {

enum opcode_t : uint32_t { // cannot be an enum class unfortunately since we need weak typing conversion to integer for switch case
    OP_CONSTANT_8,
    OP_CONSTANT_16,
    OP_CONSTANT_32,
    OP_CONSTANT_64,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEGATE,
    OP_RETURN,
};



using value_t = double;
using value_array_t = std::vector<value_t>;


struct chunk_t {
    std::vector<unsigned char> code;
    value_array_t values;

    // debug data:
    std::vector<pine_stl::run_length_encoding_t> lines;
};


void print_value(value_t value);

void disassemble_chunk(const chunk_t &chunk, const char* name);
pine_stl::offset_t disassemble_instruction(const chunk_t &chunk, pine_stl::offset_t offset);

size_t add_constant(chunk_t &chunk, value_t value);
void write_constant(chunk_t &chunk, value_t value, pine_stl::line_number_t line);

void write_chunk(chunk_t &chunk, uint8_t byte, pine_stl::line_number_t line);

pine_stl::line_number_t get_line(const chunk_t &chunk, const pine_stl::offset_t offset);

}


