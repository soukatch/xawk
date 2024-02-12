#pragma once

#include "code_gen.h"
#include "lexer.h"

#include <ranges>

namespace xawk {
class parser final {
  lexer lex_{""};
  token curr_{};

  code_gen::code_block cb_{};
  code_gen::const_pool cp_{};
  std::vector<std::pair<std::string, uint8_t>> locals_{};
  uint8_t scope_{};

  token peek() const noexcept { return curr_; }

  token next() noexcept { return std::exchange(curr_, lex_()); }

  bool check(token_type type) const noexcept { return peek().type_ == type; }

  bool match(token_type type) noexcept {
    if (check(type)) {
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

  void nud(token t, int rbp) {
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
      iden(t.lexeme_, rbp);
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

  void iden(std::string_view name, int rbp) {
    auto resolve{[&, this]() -> int {
      for (int i{}; auto &&[x, y] : std::ranges::views::reverse(locals_)) {
        if (x == name)
          return i;
        ++i;
      }
      return -1;
    }};

    auto check_decl{[&, this]() -> bool {
      for (int i{}; auto &&[x, y] : std::ranges::views::reverse(locals_)) {
        if (y < scope_)
          break;
        if (x == name)
          return false;
      }
      return true;
    }};

    switch (peek().type_) {
      using enum token_type;
    default:
      if (scope_ == 0) {
        code_gen::gen_code2(cb_, code_gen::op_code::load_global__,
                            code_gen::add_const(cp_, name.data()));
      } else {
        const auto off{resolve()};

        if (off == -1) {
          std::cerr << "undefined symbol: " << name << std::endl;
          exit(EXIT_FAILURE);
        }

        code_gen::gen_code2(cb_, code_gen::op_code::load__, off);
        break;
      }
      break;
    case equal__:
      if (rbp != 0) {
        std::cerr << "cannot assign to rvalue" << std::endl;
        exit(EXIT_FAILURE);
      }

      next();
      expr();

      if (scope_ == 0) {
        code_gen::gen_code2(cb_, code_gen::op_code::store_global__,
                            code_gen::add_const(cp_, name.data()));
      } else {
        const auto off{resolve()};
        if (off == -1) {
          std::cerr << "undefined symbol: " << name << std::endl;
          exit(EXIT_FAILURE);
        }

        code_gen::gen_code2(cb_, code_gen::op_code::store__, off);
      }
      break;
    case plusequal__:
      if (rbp != 0) {
        std::cerr << "cannot assign to rvalue" << std::endl;
        exit(EXIT_FAILURE);
      }

      next();

      if (scope_ == 0) {
        code_gen::gen_code2(cb_, code_gen::op_code::load_global__,
                            code_gen::add_const(cp_, name.data()));
        expr();
        code_gen::gen_code(cb_, code_gen::op_code::add__);
        code_gen::gen_code2(cb_, code_gen::op_code::store_global__,
                            code_gen::add_const(cp_, name.data()));
      } else {
        const auto off{resolve()};
        if (off == -1) {
          std::cerr << "undefined symbol: " << name << std::endl;
          exit(EXIT_FAILURE);
        }
        code_gen::gen_code2(cb_, code_gen::op_code::load__, off);
        expr();
        code_gen::gen_code(cb_, code_gen::op_code::add__);
        code_gen::gen_code2(cb_, code_gen::op_code::store__, off);
      }
      break;
    case minusequal__:
      if (rbp != 0) {
        std::cerr << "cannot assign to rvalue" << std::endl;
        exit(EXIT_FAILURE);
      }

      next();

      if (scope_ == 0) {
        code_gen::gen_code2(cb_, code_gen::op_code::load_global__,
                            code_gen::add_const(cp_, name.data()));
        expr();
        code_gen::gen_code(cb_, code_gen::op_code::sub__);
        code_gen::gen_code2(cb_, code_gen::op_code::store_global__,
                            code_gen::add_const(cp_, name.data()));
      } else {
        const auto off{resolve()};
        if (off == -1) {
          std::cerr << "undefined symbol: " << name << std::endl;
          exit(EXIT_FAILURE);
        }
        code_gen::gen_code2(cb_, code_gen::op_code::load__, off);
        expr();
        code_gen::gen_code(cb_, code_gen::op_code::sub__);
        code_gen::gen_code2(cb_, code_gen::op_code::store__, off);
      }
      break;
    case starequal__:
      if (rbp != 0) {
        std::cerr << "cannot assign to rvalue" << std::endl;
        exit(EXIT_FAILURE);
      }

      next();

      if (scope_ == 0) {
        code_gen::gen_code2(cb_, code_gen::op_code::load_global__,
                            code_gen::add_const(cp_, name.data()));
        expr();
        code_gen::gen_code(cb_, code_gen::op_code::mult__);
        code_gen::gen_code2(cb_, code_gen::op_code::store_global__,
                            code_gen::add_const(cp_, name.data()));
      } else {
        const auto off{resolve()};
        if (off == -1) {
          std::cerr << "undefined symbol: " << name << std::endl;
          exit(EXIT_FAILURE);
        }
        code_gen::gen_code2(cb_, code_gen::op_code::load__, off);
        expr();
        code_gen::gen_code(cb_, code_gen::op_code::mult__);
        code_gen::gen_code2(cb_, code_gen::op_code::store__, off);
      }
      break;
    case slashequal__:
      if (rbp != 0) {
        std::cerr << "cannot assign to rvalue" << std::endl;
        exit(EXIT_FAILURE);
      }

      next();

      if (scope_ == 0) {
        code_gen::gen_code2(cb_, code_gen::op_code::load_global__,
                            code_gen::add_const(cp_, name.data()));
        expr();
        code_gen::gen_code(cb_, code_gen::op_code::div__);
        code_gen::gen_code2(cb_, code_gen::op_code::store_global__,
                            code_gen::add_const(cp_, name.data()));
      } else {
        const auto off{resolve()};
        if (off == -1) {
          std::cerr << "undefined symbol: " << name << std::endl;
          exit(EXIT_FAILURE);
        }
        code_gen::gen_code2(cb_, code_gen::op_code::load__, off);
        expr();
        code_gen::gen_code(cb_, code_gen::op_code::div__);
        code_gen::gen_code2(cb_, code_gen::op_code::store__, off);
      }
      break;
    }
  }

  void expr(int rbp = 0) {
    for (nud(next(), rbp); lbp(peek().type_) > rbp;)
      led(next());
  }

  void stmt() {
    const auto type{peek().type_};
    switch (type) {
      using enum token_type;
    default:
      expr();
      expect(token_type::semi__);
      code_gen::gen_code(cb_, code_gen::op_code::pop__);
      break;
    case print__:
      next();
      expr();
      expect(token_type::semi__);
      code_gen::gen_code(cb_, code_gen::op_code::print__);
      break;
    case l_brace__:
      next();
      ++scope_;
      for (; !check(token_type::eof__) && !check(token_type::r_brace__);)
        decl();
      for (; !std::empty(locals_) && locals_.back().second == scope_;
           locals_.pop_back())
        code_gen::gen_code(cb_, code_gen::op_code::pop__);
      --scope_;
      expect(token_type::r_brace__);
      break;
    }
  }

  void var_decl() {
    expect(token_type::var__);
    const auto iden{expect(token_type::iden__).lexeme_};
    expect(token_type::equal__);
    expr();
    expect(token_type::semi__);
    if (scope_ != 0)
      locals_.emplace_back(iden, scope_);
    else
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