#pragma once


#include <cstddef>
#include <cstdint>


namespace pine_stl {
using offset_t = std::size_t;

using line_number_t = std::uint32_t;

struct run_length_encoding_t {
    line_number_t line;
    // the counter of how many chunks can be on the same line is a std::uint32_t.
    //  This is excessive, but using std::uint16_t or smaller would take up an equivalent amount of space because of alignment.
    std::uint32_t frequency;
};
}
