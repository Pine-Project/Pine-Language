#include "ast.hpp"



void pine_frontend::print_token(const pine_frontend::token_t &token) {
    std::printf("[%.*s]", token.length, token.start);
}

void pine_frontend::print_value(const pine_frontend::value_types &value) {
    std::visit(overloaded{
        [](const auto &value) {
            std::cout << value;
        }
    }, value);
}

void pine_frontend::print_expr(const pine_frontend::expression &ast) {
    std::visit(overloaded{
        [](const pine_frontend::binary &arg) {
            std::cout << "(binary: ";
            print_expr(*arg.left);
            print_token(arg.op);
            print_expr(*arg.right);
            std::cout << ')';
        },
        [](const pine_frontend::unary &arg) {
            std::cout << "(unary: ";
            print_token(arg.op);
            print_expr(*arg.right);
            std::cout << ')';
        },
        [](const pine_frontend::grouping &arg) {
            std::cout << "(grouping: ";
            print_expr(*arg.expr);
            std::cout << ')';
        },
        [](const pine_frontend::literal &arg) {
            std::cout << "(literal: ";
            print_value(arg.value);
            std::cout << ')';
        }
    }, ast);
}

void pine_frontend::print_ast(const pine_frontend::ast &ast) {
    print_expr(ast.root);
}



