#pragma once

#include "code_gen.h"

#include <utility>

namespace xawk {
class vm final {
  code_gen::code_block code_{};
  code_gen::const_pool consts_{};
  std::vector<code_gen::value> stack_{};

public:
  vm(std::pair<code_gen::code_block, code_gen::const_pool> data)
      : code_{data.first}, consts_{data.second} {}

  void operator()() {
    auto to_byte{[](code_gen::op_code o) constexpr noexcept -> uint8_t {
      return static_cast<uint8_t>(o);
    }};

    auto is_double{[](const code_gen::value &v) constexpr noexcept -> bool {
      return std::holds_alternative<double>(v);
    }};

    auto is_string{[](const code_gen::value &v) constexpr noexcept -> bool {
      return std::holds_alternative<std::string>(v);
    }};

    auto same_type{[&, this]() noexcept -> bool {
      return is_double(stack_.back()) &&
                 is_double(stack_[std::size(stack_) - 2]) ||
             is_string(stack_.back()) &&
                 is_string(stack_[std::size(stack_) - 2]);
    }};

    for (code_gen::code_block::size_type i{}; i != std::size(code_);) {
      switch (code_[i]) {
        using enum code_gen::op_code;
      default:
        std::cerr << "unknown instruction " << code_[i] << std::endl;
        exit(EXIT_FAILURE);
      case to_byte(const__):
        stack_.push_back(consts_[code_[i + 1]]);
        i += 2;
        break;
      case to_byte(neg__):
        if (!std::holds_alternative<double>(stack_.back())) {
          std::cerr << "invalid operand type for neg" << std::endl;
          exit(EXIT_FAILURE);
        }
        stack_.back() = -std::get<double>(stack_.back());
        ++i;
        break;
      case to_byte(add__):
        if (!same_type()) {
          std::cerr << "mismatch types for add" << std::endl;
          exit(EXIT_FAILURE);
        }

        if (is_double(stack_.back())) {
          stack_[std::size(stack_) - 2] =
              std::get<double>(stack_[std::size(stack_) - 2]) +
              std::get<double>(stack_.back());
        } else if (is_string(stack_.back())) {
          stack_[std::size(stack_) - 2] =
              std::get<std::string>(stack_[std::size(stack_) - 2]) +
              std::get<std::string>(stack_.back());
        }

        stack_.pop_back();
        ++i;
        break;
      case to_byte(sub__):
        if (!is_double(stack_.back()) ||
            !is_double(stack_[std::size(stack_) - 2])) {
          std::cerr << "mismatch types for sub" << std::endl;
          exit(EXIT_FAILURE);
        }

        stack_[std::size(stack_) - 2] =
            std::get<double>(stack_[std::size(stack_) - 2]) +
            std::get<double>(stack_.back());

        stack_.pop_back();
        ++i;
        break;
      case to_byte(mult__):
        if (!is_double(stack_.back()) ||
            !is_double(stack_[std::size(stack_) - 2])) {
          std::cerr << "mismatch types for mult" << std::endl;
          exit(EXIT_FAILURE);
        }

        stack_[std::size(stack_) - 2] =
            std::get<double>(stack_[std::size(stack_) - 2]) +
            std::get<double>(stack_.back());

        stack_.pop_back();
        ++i;
        break;
      case to_byte(div__):
        if (!is_double(stack_.back()) ||
            !is_double(stack_[std::size(stack_) - 2])) {
          std::cerr << "mismatch types for div" << std::endl;
          exit(EXIT_FAILURE);
        }

        stack_[std::size(stack_) - 2] =
            std::get<double>(stack_[std::size(stack_) - 2]) +
            std::get<double>(stack_.back());

        stack_.pop_back();
        ++i;
        break;
      case to_byte(print__):
        code_gen::operator<<(std::cout, stack_.back()) << std::endl;
        stack_.pop_back();
        ++i;
      }
    }
  }
};
} // namespace xawk