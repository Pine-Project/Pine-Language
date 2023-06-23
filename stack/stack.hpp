#pragma once


#include <cstddef>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <functional>


namespace pine_stl {
template<typename T, std::size_t N = 256>
class stack {
    static_assert(N > 0);

    T stack_buffer[N];
    std::size_t num_of_elements;
    std::vector<T> stack_heap_overflow;

public:
    stack() : num_of_elements(0) {}

    std::size_t size() const {
        return num_of_elements;
    }
    bool is_empty() const {
        return size() == 0;
    }

    T& top() {
        if(num_of_elements == 0) {
            throw std::out_of_range("cannot get top: stack is empty");
        }

        if(num_of_elements <= N) {
            return stack_buffer[num_of_elements - 1];
        }
        return stack_heap_overflow.at(num_of_elements - N - 1);
    }
    const T& top() const {
        if(num_of_elements == 0) {
            throw std::out_of_range("cannot get top: stack is empty");
        }

        if(num_of_elements <= N) {
            return stack_buffer[num_of_elements - 1];
        }
        return stack_heap_overflow.at(num_of_elements - N - 1);
    }

    const T& operator[](const std::size_t index) const {
        if(index >= num_of_elements) {
            throw std::out_of_range("operator[]: index out of range");
        }

        if(index < N) {
            return stack_buffer[index];
        }
        return stack_heap_overflow.at(index - N);
    }

    void push(const T& value) {
        if(num_of_elements < N) {
            stack_buffer[num_of_elements++] = value;
        } else {
            stack_heap_overflow.push_back(value);
        }
    }

    T pop() {
        if(num_of_elements == 0) {
            throw std::out_of_range("cannot pop element: stack is empty");
        }

        if(num_of_elements <= N) {
            return stack_buffer[--num_of_elements];
        }
        const auto value = stack_heap_overflow.back();
        stack_heap_overflow.pop_back();
        return value;
    }



    class const_iterator {
        std::reference_wrapper<const pine_stl::stack<T>> stack;

        std::size_t current_index;

    public:
        const_iterator(const pine_stl::stack<T>& stack) :
            stack(stack),
            current_index(0)
        {}

        const_iterator(const pine_stl::stack<T>& stack, const std::size_t index) :
            stack(stack),
            current_index(index)
        {}


        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;



        const_iterator& operator=(const const_iterator &rhs) = default;
        const_iterator& operator=(const_iterator &&rhs) = default;
        const_iterator(const const_iterator &rhs) = default;
        const_iterator(const_iterator &&rhs) = default;
        ~const_iterator() = default;



        reference operator*() const {
            return stack.get()[current_index];
        }
        pointer operator->() const {
            return &stack.stack.get()[current_index];
        }

        // prefix
        const_iterator& operator++() {
            ++current_index;
            return *this;
        }
        // postfix
        const_iterator operator++(int) {
            const auto tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const const_iterator &lhs, const_iterator &rhs) {
            return &lhs.stack.get() == &rhs.stack.get() && lhs.current_index == rhs.current_index;
        }
        friend bool operator!=(const const_iterator &lhs, const_iterator &rhs) {
            return !(lhs == rhs);
        }
    };




    const_iterator cbegin() const {
        return const_iterator(*this);
    }
    const_iterator cend() const {
        return const_iterator(*this, num_of_elements);
    }
    const_iterator begin() const {
        return cbegin();
    }
    const_iterator end() const {
        return cend();
    }
};


// return type for the four below functions:
//  `typename pine_stl::stack<T>::const_iterator`

template<typename T, std::size_t N>
auto cbegin(const stack<T, N> &stack) -> decltype(stack.cbegin()) {
    return stack.cbegin();
}

template<typename T, std::size_t N>
auto cend(const stack<T, N> &stack) -> decltype(stack.cend()) {
    return stack.cend();
}

template<typename T, std::size_t N>
auto begin(const stack<T, N> &stack) -> decltype(stack.begin()) {
    return stack.begin();
}

template<typename T, std::size_t N>
auto end(const stack<T, N> &stack) -> decltype(stack.end()) {
    return stack.end();
}
}

