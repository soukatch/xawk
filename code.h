#pragma once

#include <iostream>
#include <limits>
#include <vector>

namespace code {
enum struct op : uint8_t { const__, neg__, add__, sub__, mult__, div__ };

using code_block = std::vector<uint8_t>;
using const_pool = std::vector<double>;

void gen_op(code_block &cb, op o) { cb.push_back(static_cast<uint8_t>(o)); }

void gen_const(code_block &cb, const_pool &cp, double val) {
  if (std::size(cp) >= std::numeric_limits<uint8_t>::max()) {
    std::cerr << "const pool size exceeded" << std::endl;
    exit(EXIT_FAILURE);
  }
  cp.push_back(val);
  gen_op(cb, op::const__);
  cb.push_back(static_cast<uint8_t>(std::size(cp)) - 1);
}
} // namespace code