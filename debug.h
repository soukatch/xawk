#include "code_gen.h"

namespace xawk {
void disassemble(const code_gen::code_block &cb) {
  auto disassemble_code{[&](code_gen::code_block::size_type i)
                            -> code_gen::code_block::size_type {
    switch (cb[i++]) {
    case code_gen::to_byte(code_gen::op_code::add__):
      std::cout << "add__ " << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::sub__):
      std::cout << "sub__ " << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::mult__):
      std::cout << "mult__ " << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::div__):
      std::cout << "div__ " << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::neg__):
      std::cout << "neg__ " << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::const__):
      std::cout << "const__ " << std::to_string(cb[i++]) << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::print__):
      std::cout << "print__ " << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::pop__):
      std::cout << "add__ " << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::def_global__):
      std::cout << "def_global__ " << std::to_string(cb[i++]) << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::load_global__):
      std::cout << "load_global__ " << std::to_string(cb[i++]) << std::endl;
      break;
    case code_gen::to_byte(code_gen::op_code::store_global__):
      std::cout << "store_global__ " << std::to_string(cb[i++]) << std::endl;
      break;
    }
    return i;
  }};

  for (code_gen::code_block::size_type i{}; i != std::size(cb);)
    i = disassemble_code(i);
}
} // namespace xawk