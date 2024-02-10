#pragma once

#include "code.h"
#include "lexer.h"

#include <utility>
#include <vector>

class parser final {
  lexer lexer_;
  token next_{};

  code::code_block code_{};
  code::const_pool consts_{};

  token next() { return std::exchange(next_, lexer_()); }

  token peek() const { return next_; }

  static constexpr int lbp(token_type type) noexcept {
    switch (type) {
      using enum token_type;
    default:
      return 0;
    case plus__:
    case minus__:
      return 1;
    case star__:
    case slash__:
      return 2;
    }
  }

  void nud(token t) {
    switch (t.type_) {
      using enum token_type;
    default:
      std::cerr << "unkown nud " << t.type_ << std::endl;
      break;
    case number__:
      code::gen_const(code_, consts_, std::stod(t.lexeme_));
      break;
    case l_paren__:
      expr();
      if (next().type_ != r_paren__)
        std::cerr << "expected )" << std::endl;
      break;
    case minus__:
      expr(2);
      code::gen_op(code_, code::op::neg__);
      break;
    }
  }

  void led(token_type type) {
    switch (type) {
      using enum token_type;
    default:
      std::cerr << "unknown binary " << type << std::endl;
      break;
    case plus__:
      expr(lbp(plus__));
      code::gen_op(code_, code::op::add__);
      break;
    case minus__:
      expr(lbp(plus__));
      code::gen_op(code_, code::op::sub__);
      break;
    case star__:
      expr(lbp(star__));
      code::gen_op(code_, code::op::mult__);
      break;
    case slash__:
      expr(lbp(slash__));
      code::gen_op(code_, code::op::div__);
      break;
    }
  }

  void expr(int rbp = 0) {
    nud(next());

    for (; lbp(peek().type_) > rbp;) {
      led(next().type_);
    }
  }

public:
  parser(std::string_view filename) : lexer_{filename} {}

  std::pair<code::code_block, code::const_pool> parse() {
    next_ = lexer_();
    expr();
    return {code_, consts_};
  }
};