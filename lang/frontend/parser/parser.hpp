#pragma once


#include <functional>
#include <vector>

#include <lang/frontend/lexer/lexer.hpp>


namespace pine_frontend {
struct expression {
    
};


struct parser {
    std::reference_wrapper<std::vector<token_t>> tokens;
    std::uint64_t current_index = 0;

    parser() = delete;

    parser(std::vector<token_t> &tokens) : tokens(tokens) {}


};
}

