#include "vm.hpp"


// TODO: consider also allowing people to pass around a context pointer instead if needed (e.g. for embedding)
static pine_vm::VM vm;



static void load_and_store_binary_op(pine_vm::VM &vm_context, pine_bytecode::value_t (*const op)(pine_bytecode::value_t, pine_bytecode::value_t)) {
    vm_context.stack.push(pine_stl::binary_op(vm_context.stack.pop(), vm_context.stack.pop(), op));
}


static pine_vm::interpret_result_t run() {
    const auto read_byte = [] { return static_cast<std::uint64_t>(*vm.ip++); };

    for(;;) {

#ifdef DEBUG_TRACE_EXECUTION
        std::cout << "stack:          ";
        for(const auto val : vm.stack) {
            std::cout << "[ ";
            std::cout << val;
            std::cout << " ]";
        }
        std::cout << '\n';

        pine_bytecode::disassemble_instruction(*vm.chunk, static_cast<pine_stl::offset_t>(vm.ip - vm.chunk->code.data()));
#endif

        switch(uint8_t instruction = read_byte()) {
            case pine_bytecode::OP_NEGATE:
                vm.stack.top() = -vm.stack.top();
                break;

            case pine_bytecode::OP_ADD:
                load_and_store_binary_op(vm, pine_stl::add<pine_bytecode::value_t>);
                break;
            case pine_bytecode::OP_SUB:
                load_and_store_binary_op(vm, pine_stl::sub<pine_bytecode::value_t>);
                break;
            case pine_bytecode::OP_DIV:
                load_and_store_binary_op(vm, pine_stl::div<pine_bytecode::value_t>);
                break;
            case pine_bytecode::OP_MUL:
                load_and_store_binary_op(vm, pine_stl::mul<pine_bytecode::value_t>);
                break;

            case pine_bytecode::OP_RETURN:
                pine_bytecode::print_value(vm.stack.pop());
                std::cout << '\n';

                return pine_vm::interpret_result_t::OK;

            case pine_bytecode::OP_CONSTANT_8: {
                const pine_bytecode::value_t constant = vm.chunk->values[read_byte()];
                //pine_bytecode::print_value(constant);
                vm.stack.push(constant);
                //std::cout << '\n';
                break;
            }
            case pine_bytecode::OP_CONSTANT_16: {
                const pine_bytecode::value_t constant = vm.chunk->values[read_byte() | read_byte() << 8];
                //pine_bytecode::print_value(constant);
                vm.stack.push(constant);
                //std::cout << '\n';
                break;
            }
            case pine_bytecode::OP_CONSTANT_32: {
                const pine_bytecode::value_t constant = vm.chunk->values[read_byte() | read_byte() << 8 | read_byte() << 16 | read_byte() << 24];
                //pine_bytecode::print_value(constant);
                vm.stack.push(constant);
                //std::cout << '\n';
                break;
            }
            case pine_bytecode::OP_CONSTANT_64: {
                const pine_bytecode::value_t constant = vm.chunk->values[read_byte() | read_byte() << 8 | read_byte() << 16 | read_byte() << 24
                                                            | read_byte() << 32 | read_byte() << 40 | read_byte() << 48 | read_byte() << 56];
                //pine_bytecode::print_value(constant);
                vm.stack.push(constant);
                //std::cout << '\n';
                break;
            }
        }
    }
}

pine_vm::interpret_result_t pine_vm::interpret(pine_bytecode::chunk_t &chunk) {
    vm.chunk = &chunk;
    vm.ip = vm.chunk->code.data();
    return run();
}




pine_vm::interpret_result_t pine_lang::interpret(const char* file_contents) {
    pine_frontend::scanner scanner(file_contents);
    const auto lexemes = scan_all_tokens(scanner);

    std::cout << "lexemes.size(): " << lexemes.size() << '\n';

    for(const auto e : lexemes) {
        if(e.type != pine_frontend::token_type_t::ERROR) {
            std::printf("contents: [%.*s], line: [%d], type: [%d]\n", e.length, e.start, e.line, e.type);
        }
    }

    return pine_vm::interpret_result_t::OK;
}



