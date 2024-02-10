#pragma once

#include "token_type.h"

#include <string>

struct token final {
  token_type type_{};
  std::string lexeme_{};
  int line_{};
};

std::ostream &operator<<(std::ostream &os, const token &t) {
  return os << t.type_ << " " << t.lexeme_ << " " << t.line_;
}