#pragma once


#include <variant>
#include <cstdint>
#include <string>
#include <memory>
#include <iostream>
#include <cstdio>

#include <utils/visitor_utils.hpp>
#include <lang/frontend/lexer/lexer.hpp>



namespace pine_frontend {

struct binary;
struct unary;
struct literal;
struct grouping;

using expression = std::variant<binary, unary, literal, grouping>;
using value_types = std::variant<std::int64_t, double, bool, std::string>;

struct binary {
    std::unique_ptr<expression> left;
    token_t op;
    std::unique_ptr<expression> right;
};
struct unary {
    token_t op;
    std::unique_ptr<expression> right;
};
struct grouping {
    std::unique_ptr<expression> expr;
};
struct literal {
    value_types value;
};



struct ast {
expression root;
};



void print_token(const token_t &token);
void print_value(const value_types &value);
// does in-order traversal
void print_expr(const expression &ast);
void print_ast(const ast &ast);




}


