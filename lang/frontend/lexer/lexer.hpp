#pragma once


#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <string_view>

#include <utils/types.hpp>
#include <utils/string_utils.hpp>



namespace pine_frontend {
enum class token_type_t : std::uint32_t {
    // single-char lexemes
    LEFT_PAREN = 0, RIGHT_PAREN = 1,
    LEFT_CURLY = 2, RIGHT_CURLY = 3,
    COMMA = 4, DOT= 5, SEMICOLON = 6, COLON = 7,
    PLUS = 8, MINUS = 9, STAR = 10, SLASH = 11,
    LOGICAL_NOT = 12,
    BITWISE_OR = 13, BITWISE_AND = 14, BITWISE_NOT = 15, BITWISE_XOR = 16,
    ASSIGNMENT,

    // TODO: consider adding a LOGICAL_XOR lexeme
    // two-character lexemes
    EQUAL = 17, NOT_EQUAL = 18, GREATER = 19, GREATER_EQUAL = 20, LESS = 21, LESS_EQUAL = 22,
    LOGICAL_OR = 23, LOGICAL_AND = 24,
    BITSHIFT_LEFT = 25, BITSHIFT_RIGHT = 26,

    // literals
    IDENTIFIER = 27, STRING = 28, INTEGER = 29, FLOAT = 30, LIST = 31,

    // keywords/reserved identifiers
    IF = 32, ELSE = 33, FUN = 34, TRUE = 35, FALSE = 36, STRUCT = 37,
    FOR = 38, WHILE = 39, DO_WHILE = 40, LOOP = 41,
    RETURN = 42, LET = 43, MUT = 44,


    // have to use `EOF_VAL` instead of `EOF` because `EOF` is a built-in macro in C/C++
    ERROR = 45, EOF_VAL = 46,
};


struct scanner {
    const char* start; // starting character of the current lexeme
    const char* current; // current character being scanned for the current lexeme
    pine_stl::line_number_t current_line;


    scanner() = delete;

    scanner(const char *const source) :
        start(source),
        current(source),
        current_line(1)
    {}


    bool at_end_of_str() {
        return (*current) == '\0';
    }
    // at_end_of_str() == at_end_of_str_n(0)
    bool at_end_of_str_n(std::size_t lookahead) {
        for(uint32_t i = 0; i <= lookahead; ++i) {
            if(current[i] == '\0') return true;
        }
        return false;
    }

    char advance_char() {
        return *(current++);
    }
    char peek_char() {
        return (*current);
    }
    // peek_char() == peek_char_n(0)
    char peek_char_n(std::size_t lookahead) {
        if(at_end_of_str_n(lookahead)) return '\0';

        return current[lookahead];
    }
    bool match_char(const char expected) {
        if(at_end_of_str()) return false;

        if(*current != expected) return false;

        ++current;
        return true;
    }
};

struct token_t {
    token_type_t type;
    const char* start; // start of string containing the token
    std::uint32_t length;
    pine_stl::line_number_t line;

    token_t() = delete;

    token_t(const scanner &scanner, const token_type_t type)
        : type(type),
        start(scanner.start),
        length(scanner.current - scanner.start),
        line(scanner.current_line)
    {}
};



token_type_t handle_number(scanner &scanner);
token_type_t handle_identifier(scanner &scanner);
token_type_t handle_string(scanner &scanner);
token_type_t handle_list(scanner &scanner);

// returns `true` is a comment is parsed (i.e the text is a comment), returns `false` otherwise
bool handle_comment(scanner &scanner);
void skip_whitespace(scanner &scanner);

// we pass around scanner object and have no global scanner object (as contrasted with VM).
//  This ensures that the scanner object is always initialized since we have no default constructor.
token_t scan_token(scanner &scanner);
// TODO: write automated unit tests for the below function or test it on some example programs
std::vector<token_t> scan_all_tokens(scanner &scanner);
}


