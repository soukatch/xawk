#pragma once

#include "code_gen.h"
#include "lexer.h"

namespace xawk {
class parser final {
  lexer lex_{""};
  token curr_{};

  code_gen::code_block cb_{};
  code_gen::const_pool cp_{};

  token peek() const noexcept { return curr_; }

  token next() noexcept { return std::exchange(curr_, lex_()); }

  bool match(token_type type) noexcept {
    if (peek().type_ == type) {
      next();
      return true;
    }
    return false;
  }

  token expect(token_type type) noexcept {
    if (peek().type_ != type) {
      std::cerr << "expected: " << type << ", got: " << peek().type_
                << std::endl;
      exit(EXIT_FAILURE);
    }

    return next();
  }

  static constexpr int lbp(token_type type) {
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
      std::cerr << "unknown nud: " << t.type_ << std::endl;
      exit(EXIT_FAILURE);
    case number__:
      code_gen::gen_code2(cb_, code_gen::op_code::const__,
                          code_gen::add_const(cp_, std::stod(t.lexeme_)));
      break;
    case string__:
      code_gen::gen_code2(cb_, code_gen::op_code::const__,
                          code_gen::add_const(cp_, t.lexeme_));
      break;
    case minus__:
      expr(2);
      code_gen::gen_code(cb_, code_gen::neg__);
      break;
    case l_paren__:
      expr();
      expect(r_paren__);
    case iden__:
      iden(t.lexeme_);
    }
  }

  void led(token t) {
    switch (t.type_) {
      using enum token_type;
    default:
      std::cerr << "unknown led: " << t.type_ << std::endl;
      exit(EXIT_FAILURE);
    case plus__:
      expr(lbp(plus__));
      code_gen::gen_code(cb_, code_gen::op_code::add__);
      break;
    case minus__:
      expr(lbp(minus__));
      code_gen::gen_code(cb_, code_gen::op_code::sub__);
      break;
    case star__:
      expr(lbp(star__));
      code_gen::gen_code(cb_, code_gen::op_code::mult__);
      break;
    case slash__:
      expr(lbp(slash__));
      code_gen::gen_code(cb_, code_gen::op_code::div__);
    }
  }

  void iden(std::string_view name) {
    if (match(token_type::equal__)) {
      expr();
      code_gen::gen_code2(cb_, code_gen::op_code::store_global__,
                          code_gen::add_const(cp_, name.data()));
    } else {
      code_gen::gen_code2(cb_, code_gen::op_code::load_global__,
                          code_gen::add_const(cp_, name.data()));
    }
  }

  void expr(int rbp = 0) {
    for (nud(next()); lbp(peek().type_) > rbp;)
      led(next());
  }

  void stmt() {
    const auto print_stmt{match(token_type::print__)};
    expr();
    expect(token_type::semi__);
    code_gen::gen_code(cb_, print_stmt ? code_gen::op_code::print__
                                       : code_gen::op_code::pop__);
  }

  void var_decl() {
    expect(token_type::var__);
    const auto iden{expect(token_type::iden__).lexeme_};
    expect(token_type::equal__);
    expr();
    expect(token_type::semi__);
    code_gen::gen_code2(cb_, code_gen::op_code::def_global__,
                        code_gen::add_const(cp_, iden));
  }

  void decl() {
    switch (peek().type_) {
      using enum token_type;
    default:
      stmt();
      break;
    case var__:
      var_decl();
      break;
    }
  }

public:
  parser(std::string_view filename) : lex_{filename} {}

  std::pair<code_gen::code_block, code_gen::const_pool> operator()() {
    next();

    for (; !match(token_type::eof__);)
      decl();

    return {cb_, cp_};
  }
};
} // namespace xawk