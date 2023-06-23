#include "bytecode.hpp"


// TODO: consider removing this and just using the fully qualified names instead for disambiguity
using namespace pine_bytecode;

void pine_bytecode::print_value(const value_t value) {
    std::cout << value;
}


template<typename T>
static pine_stl::offset_t constant_instruction(const char *const name, const chunk_t &chunk, const pine_stl::offset_t offset) {
    const auto constant = *reinterpret_cast<const T*>(&chunk.code.at(offset + 1));
    // TODO: maybe change to have more characters of space in string formatting?????
    std::printf("%-16s %4d '", name, constant);
    print_value(chunk.values.at(constant));
    std::cout << "'\n";
    return offset + sizeof(T) + 1;
}


static pine_stl::offset_t constant_instruction_8(const char *const name, const chunk_t &chunk, const pine_stl::offset_t offset) {
    return constant_instruction<std::uint8_t>(name, chunk, offset);
}
static pine_stl::offset_t constant_instruction_16(const char *const name, const chunk_t &chunk, const pine_stl::offset_t offset) {
    return constant_instruction<std::uint16_t>(name, chunk, offset);
}
static pine_stl::offset_t constant_instruction_32(const char *const name, const chunk_t &chunk, const pine_stl::offset_t offset) {
    return constant_instruction<std::uint32_t>(name, chunk, offset);
}
static pine_stl::offset_t constant_instruction_64(const char *const name, const chunk_t &chunk, const pine_stl::offset_t offset) {
    return constant_instruction<std::uint64_t>(name, chunk, offset);
}


static pine_stl::offset_t simple_instruction(const char *const name, const pine_stl::offset_t offset) {
    std::cout << name << '\n';
    return offset + 1;
}



void pine_bytecode::disassemble_chunk(const chunk_t &chunk, const char *const name) {
    std::cout << "== " << name << " ==\n";

    for(pine_stl::offset_t offset = 0; offset < std::size(chunk.code);) {
        offset = disassemble_instruction(chunk, offset);
    }
}

pine_stl::offset_t pine_bytecode::disassemble_instruction(const chunk_t &chunk, const pine_stl::offset_t offset) {
    std::cout << std::setfill('0') << std::setw(4) << offset << ' ';


    const auto line_at_offset = get_line(chunk, offset);
    // TODO: change this as it is now completely wrong and should use `get_line()`
    if(offset > 0 && line_at_offset == get_line(chunk, offset - 1)) {
        std::printf("   | ");
    } else {
        std::printf("%4d ", line_at_offset);
    }



    const std::uint8_t instruction = chunk.code.at(offset);
    switch(instruction) {
        case OP_CONSTANT_8:
            return constant_instruction_8("OP_CONSTANT_8", chunk, offset);
        case OP_CONSTANT_16:
            return constant_instruction_16("OP_CONSTANT_16", chunk, offset);
        case OP_CONSTANT_32:
            return constant_instruction_32("OP_CONSTANT_32", chunk, offset);
        case OP_CONSTANT_64:
            return constant_instruction_64("OP_CONSTANT_64", chunk, offset);

        case OP_NEGATE:
            return simple_instruction("OP_NEGATE", offset);

        case OP_ADD:
            return simple_instruction("OP_ADD", offset);
        case OP_SUB:
            return simple_instruction("OP_SUB", offset);
        case OP_MUL:
            return simple_instruction("OP_MUL", offset);
        case OP_DIV:
            return simple_instruction("OP_DIV", offset);

        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);

        default:
            std::cout << "Unkown Opcode: " << instruction << '\n';
            return offset + 1;
    }
}


size_t pine_bytecode::add_constant(chunk_t &chunk, value_t value) {
    chunk.values.push_back(value);
    return std::size(chunk.values) - 1;
}

void pine_bytecode::write_constant(chunk_t &chunk, const value_t value, const pine_stl::line_number_t line) {
    const auto constant = add_constant(chunk, value);
    const unsigned char *const constant_as_bytes = reinterpret_cast<const unsigned char*>(&constant);
    if(constant <= 0xFF) {
        write_chunk(chunk, OP_CONSTANT_8, line);
        write_chunk(chunk, constant_as_bytes[0], line);
    } else if(constant <= 0xFFFF) {
        write_chunk(chunk, OP_CONSTANT_16, line);
        write_chunk(chunk, constant_as_bytes[0], line);
        write_chunk(chunk, constant_as_bytes[1], line);
    } else if(constant <= 0xFFFFFFFF) {
        write_chunk(chunk, OP_CONSTANT_32, line);
        write_chunk(chunk, constant_as_bytes[0], line);
        write_chunk(chunk, constant_as_bytes[1], line);
        write_chunk(chunk, constant_as_bytes[2], line);
        write_chunk(chunk, constant_as_bytes[3], line);
    } else {
        write_chunk(chunk, OP_CONSTANT_64, line);
        write_chunk(chunk, constant_as_bytes[0], line);
        write_chunk(chunk, constant_as_bytes[1], line);
        write_chunk(chunk, constant_as_bytes[2], line);
        write_chunk(chunk, constant_as_bytes[3], line);
        write_chunk(chunk, constant_as_bytes[4], line);
        write_chunk(chunk, constant_as_bytes[5], line);
        write_chunk(chunk, constant_as_bytes[6], line);
        write_chunk(chunk, constant_as_bytes[7], line);
    }
}

void pine_bytecode::write_chunk(chunk_t &chunk, const uint8_t byte, const pine_stl::line_number_t line) {
    chunk.code.push_back(byte);
    if(chunk.lines.empty() || chunk.lines.back().line != line) {
        assert(chunk.lines.empty() || chunk.lines.back().line < line);
        chunk.lines.push_back({line, 1});
    } else {
        ++chunk.lines.back().frequency;
    }
}

// TODO: consider writing unit tests for this function
pine_stl::line_number_t pine_bytecode::get_line(const chunk_t &chunk, const pine_stl::offset_t offset) {
    for(pine_stl::offset_t current_offset = 0, line_index = 0; line_index < chunk.lines.size(); ++line_index) {
        if(offset >= current_offset && offset < (current_offset + chunk.lines.at(line_index).frequency)) {
            return chunk.lines.at(line_index).line;
        }
        current_offset += chunk.lines.at(line_index).frequency;
    }
    return 0xFFFFFFFF;
}

