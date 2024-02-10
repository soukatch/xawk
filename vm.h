#pragma once

#include "code.h"

#include <utility>

class vm final {
  code::code_block code_{};
  code::const_pool consts_{};
  std::vector<double> stack_{};

public:
  vm(std::pair<code::code_block, code::const_pool> data)
      : code_{data.first}, consts_{data.second} {}

  void operator()() {
    auto to_byte{[](code::op o) constexpr noexcept -> uint8_t {
      return static_cast<uint8_t>(o);
    }};

    for (code::code_block::size_type i{}; i != std::size(code_);) {
      switch (code_[i]) {
        using enum code::op;
      default:
        std::cerr << "unknown instruction " << code_[i] << std::endl;
        exit(EXIT_FAILURE);
      case to_byte(const__):
        stack_.push_back(consts_[code_[i + 1]]);
        i += 2;
        break;
      case to_byte(neg__):
        stack_.back() = -stack_.back();
        ++i;
        break;
      case to_byte(add__):
        stack_[std::size(stack_) - 2] += stack_.back();
        stack_.pop_back();
        ++i;
        break;
      case to_byte(sub__):
        stack_[std::size(stack_) - 2] -= stack_.back();
        stack_.pop_back();
        ++i;
        break;
      case to_byte(mult__):
        stack_[std::size(stack_) - 2] *= stack_.back();
        stack_.pop_back();
        ++i;
        break;
      case to_byte(div__):
        stack_[std::size(stack_) - 2] /= stack_.back();
        stack_.pop_back();
        ++i;
        break;
      }
    }

    std::cout << (std::empty(stack_) ? 0 : stack_.front()) << std::endl;
  }
};