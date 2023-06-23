#pragma once


#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <iostream>


namespace pine_stl {
// we cannot use std::string_view for this because std::ifstream only accepts std::string and const char* (null terminated) in its constructor for the filename
std::string read_file_into_string(const char* filename);

inline bool is_digit(const char c) {
    return c >= '0' && c <= '9';
}

// underscores (_) are considered alphabetical because identifiers are allowed to start with them
inline bool is_alpha(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// underscores (_) are considered alphabetical because identifiers are allowed to start with them
inline bool is_alpha_numeric(const char c) {
    return is_alpha(c) || is_digit(c);
}
}

