#pragma once


#include <bytecode_vm/bytecode.hpp>


namespace pine_stl {

inline pine_bytecode::value_t binary_op(const pine_bytecode::value_t a, const pine_bytecode::value_t b, pine_bytecode::value_t (*const op)(pine_bytecode::value_t, pine_bytecode::value_t)) {
    return op(a, b);
}


template<typename T>
inline T div(T a, T b) {
    return a / b;
}

template<typename T>
inline T mul(T a, T b) {
    return a * b;
}

template<typename T>
inline T sub(T a, T b) {
    return a - b;
}

template<typename T>
inline T add(T a, T b) {
    return a + b;
}

}

