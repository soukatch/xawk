#pragma once

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace xawk {
namespace code_gen {
using value = std::variant<double, std::string>;
using code_block = std::vector<uint8_t>;
using fn_map = std::unordered_map<std::string, code_block>;
using var_map = std::unordered_map<std::string, value>;
using const_pool = std::vector<value>;

enum op_code {
  add__,
  sub__,
  mult__,
  div__,
  neg__,
  const__,
  print__,
  pop__,
  def_global__,
  load_global__,
  store_global__,
};

auto to_byte{[](code_gen::op_code o) constexpr noexcept -> uint8_t {
  return static_cast<uint8_t>(o);
}};

std::ostream &operator<<(std::ostream &os, const value &v) {
  if (std::holds_alternative<double>(v))
    return os << std::get<double>(v);
  else
    return os << std::get<std::string>(v);
}

void gen_code(code_block &cb, op_code op) {
  cb.push_back(static_cast<uint8_t>(op));
}

void gen_code2(code_block &cb, op_code op, uint8_t byte) {
  cb.push_back(static_cast<uint8_t>(op));
  cb.push_back(byte);
}

uint8_t add_const(const_pool &cp, value v) {
  if (std::size(cp) == std::numeric_limits<uint8_t>::max()) {
    std::cerr << "const pool size limit reached." << std::endl;
    exit(EXIT_FAILURE);
  }

  cp.push_back(v);
  return static_cast<uint8_t>(std::size(cp)) - 1;
}

} // namespace code_gen
} // namespace xawk