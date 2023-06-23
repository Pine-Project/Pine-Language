#include "bytecode_vm/bytecode.hpp"
#include "bytecode_vm/vm.hpp"
#include "interface/io/repl.hpp"



#if 0
int main() {
    pine_bytecode::chunk_t chunk;


#if 0
    const auto constant = add_constant(chunk, 1.2);
    write_chunk(chunk, pine_bytecode::OP_CONSTANT_8, 123);
    write_chunk(chunk, constant, 123);

    write_chunk(chunk, pine_bytecode::OP_NEGATE, 123);
#endif


    auto constant = add_constant(chunk, 1.2);
    write_chunk(chunk, pine_bytecode::OP_CONSTANT_8, 123);
    write_chunk(chunk, constant, 123);

    constant = add_constant(chunk, 3.4);
    write_chunk(chunk, pine_bytecode::OP_CONSTANT_8, 123);
    write_chunk(chunk, constant, 123);

    write_chunk(chunk, pine_bytecode::OP_ADD, 123);

    constant = add_constant(chunk, 5.6);
    write_chunk(chunk, pine_bytecode::OP_CONSTANT_8, 123);
    write_chunk(chunk, constant, 123);

    write_chunk(chunk, pine_bytecode::OP_DIV, 123);

    write_chunk(chunk, pine_bytecode::OP_NEGATE, 123);

    write_chunk(chunk, pine_bytecode::OP_RETURN, 123);


    //disassemble_chunk(chunk, "test chunk");

    interpret(chunk);
}
#endif




// TODO: split pine-language and pine-vm into two different git repositories


int main(int argc, char **argv) {
    if(argc == 1) {
        pine_interface::repl(); // no filename was passed to interpreter
    } else if(argc == 2) {
        pine_interface::run_file(argv[1]); // load the file using the filename passed to interpreter
    } else {
        throw std::runtime_error("main: Too many arguments passed to interpreter");
    }

    return 0;
}



