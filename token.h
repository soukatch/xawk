#pragma once

#include "token_type.h"

#include <string>

struct token final {
  const token_type type_{};
  const std::string lexeme_{};
  const int line_{};
};

std::ostream &operator<<(std::ostream &os, const token &t) {
  return os << t.type_ << " " << t.lexeme_ << " " << t.line_;
}