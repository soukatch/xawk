#pragma once

#include "token.h"

#include <fstream>
#include <string_view>
#include <vector>

class lexer final {
  std::ifstream file_{};
  std::string buf_{};
  std::string::size_type prev_{}, next_{};
  int line_{};

  char peek(std::string::size_type n = 0) const noexcept {
    return next_ + n < std::size(buf_) ? buf_[next_ + n] : '\0';
  }

  char prev() const noexcept { return buf_[prev_]; }

  char next() noexcept { return buf_[next_++]; }

  bool match(char c) noexcept { return peek() == c ? (next(), true) : false; }

  bool eol() const noexcept { return next_ == std::size(buf_); }

  bool eof() const noexcept { return file_.eof(); }

  bool line() {
    if (eof())
      return false;
    std::getline(file_, buf_);
    ++line_;
    prev_ = next_ = 0;
    return true;
  }

  token make_token(token_type type) const {
    return make_token(type, prev_, next_);
  }

  token make_token(token_type type, std::string::size_type first,
                   std::string::size_type last) const {
    return token{type, buf_.substr(first, last - first), line_};
  }

  void whitespace() {
    for (; next_ != std::size(buf_); ++next_) {
      switch (peek()) {
      default:
        return;
      case ' ':
      case '\t':
      case '\r':
        break;
      case '/':
        if (peek(1) == '/') {
          line();
          break;
        }
        return;
      }
    }
  }

  token identifier() {
    for (; std::isalnum(peek()); next())
      ;

    return make_token(token_type::iden__);
  }

  token keyword(std::string_view text, token_type type) {
    for (auto &&x : text)
      if (!match(x))
        return identifier();

    return !std::isalpha(peek()) ? make_token(type) : identifier();
  }

  token number() {
    for (; std::isdigit(peek()); next())
      ;

    if (peek() == '.' && std::isdigit(peek(1)))
      for (next(); std::isdigit(peek()); next())
        ;

    return make_token(token_type::number__);
  }

  token string() {
    for (; peek() != '"' && !eol(); next())
      ;

    if (eol())
      return token{token_type::error__, "unterminated string literal", line_};

    next();

    return make_token(token_type::string__, prev_ + 1, next_ - 1);
  }

public:
  lexer(std::string_view filename) : file_{filename} {}

  token operator()() {
    if (eol() && !line())
      return make_token(token_type::eof__);

    whitespace();

    if (eol())
      return make_token(token_type::eof__);

    prev_ = next_;
    switch (next()) {
      using enum token_type;
    default:
      return std::isdigit(prev()) ? number()
             : std::isalpha(prev())
                 ? identifier()
                 : token{error__, "unexpected character", line_};
    case '{':
      return make_token(l_brace__);
    case '}':
      return make_token(r_brace__);
    case '(':
      return make_token(l_paren__);
    case ')':
      return make_token(r_paren__);
    case '[':
      return make_token(l_square__);
    case ']':
      return make_token(r_square__);
    case ';':
      return make_token(semi__);
    case '$':
      return make_token(dollar__);
    case '.':
      return make_token(dot__);
    case '+':
      switch (peek()) {
      default:
        return make_token(plus__);
      case '+':
        return (next(), make_token(plusplus__));
      case '=':
        return (next(), make_token(plusequal__));
      }
    case '-':
      switch (peek()) {
      default:
        return make_token(minus__);
      case '-':
        return (next(), make_token(minusminus__));
      case '=':
        return (next(), make_token(minusequal__));
      case '>':
        return (next(), make_token(arrow__));
      }
    case '*':
      switch (peek()) {
      default:
        return make_token(star__);
      case '=':
        return (next(), make_token(starequal__));
      }
    case '/':
      switch (peek()) {
      default:
        return make_token(slash__);
      case '=':
        return (next(), make_token(slashequal__));
      }
    case '&':
      switch (peek()) {
      default:
        return make_token(amp__);
      case '&':
        return (next(), make_token(ampamp__));
      case '=':
        return (next(), make_token(ampequal__));
      }
    case '|':
      switch (peek()) {
      default:
        return make_token(pipe__);
      case '|':
        return (next(), make_token(pipepipe__));
      case '=':
        return (next(), make_token(pipeequal__));
      }
    case '!':
      switch (peek()) {
      default:
        return make_token(bang__);
      case '=':
        return (next(), make_token(bangequal__));
      }
    case '=':
      switch (peek()) {
      default:
        return make_token(equal__);
      case '=':
        return (next(), make_token(equalequal__));
      }
    case 'B':
      return keyword("EGIN", BEGIN__);
    case 'E':
      return keyword("ND", END__);
    case 'f':
      switch (peek()) {
      default:
        return identifier();
      case 'n':
        return keyword("n", fn__);
      case 'o':
        return keyword("or", for__);
      }
    case 'i':
      return keyword("f", if__);
    case 'e':
      return keyword("lse", else__);
    case 'r':
      return keyword("eturn", return__);
    case 's':
      return keyword("truct", struct__);
    case 'v':
      return keyword("ar", var__);
    }
  }
};