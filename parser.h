#pragma once

#include "lexer.h"

class parser final {
  lexer lexer_;
  token next_{};

  token next() { return std::exchange(next_, lexer_()); }

  token peek() const { return next_; }

public:
  parser(std::string_view filename) : lexer_{filename} {}

private:
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
    default:
      std::cerr << "unkown nud " << t.type_ << std::endl;
      break;
    case token_type::number__:
      std::cout << t.lexeme_ << std::endl;
      break;
    case token_type::l_paren__:
      expr();
      if (next().type_ != token_type::r_paren__)
        std::cerr << "expected )" << std::endl;
      break;
    case token_type::minus__:
      expr(1);
      std::cout << "neg" << std::endl;
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
      expr(1);
      std::cout << "plus" << std::endl;
      break;
    case minus__:
      expr(1);
      std::cout << "minus" << std::endl;
      break;
    case star__:
      expr(2);
      std::cout << "star" << std::endl;
      break;
    case slash__:
      expr(2);
      std::cout << "slash" << std::endl;
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
  void parse() {
    next_ = lexer_();
    expr();
  }
};