#pragma once

#include <iostream>

enum struct token_type {
  BEGIN__,
  END__,
  dollar__,
  semi__,
  l_paren__,
  r_paren__,
  l_brace__,
  r_brace__,
  l_square__,
  r_square__,
  comma__,
  dot__,
  plus__,
  minus__,
  star__,
  slash__,
  amp__,
  caret__,
  pipe__,
  bang__,
  equal__,
  tidle__,
  arrow__,
  plusplus__,
  minusminus__,
  ampamp__,
  pipepipe__,
  plusequal__,
  minusequal__,
  starequal__,
  slashequal__,
  ampequal__,
  caretequal__,
  pipeequal__,
  bangequal__,
  equalequal__,
  var__,
  fn__,
  for__,
  if__,
  else__,
  print__,
  return__,
  struct__,
  iden__,
  number__,
  string__,
  eof__,
  error__,
};

std::ostream &operator<<(std::ostream &os, token_type type) {
  switch (type) {
    using enum token_type;
  case BEGIN__:
    return os << "BEGIN__";
  case END__:
    return os << "END__";
  case dollar__:
    return os << "dollar__";
  case semi__:
    return os << "semi__";
  case l_paren__:
    return os << "l_paren__";
  case r_paren__:
    return os << "r_paren__";
  case l_brace__:
    return os << "l_brace__";
  case r_brace__:
    return os << "r_brace__";
  case l_square__:
    return os << "l_square__";
  case r_square__:
    return os << "r_square__";
  case comma__:
    return os << "comma__";
  case dot__:
    return os << "dot__";
  case plus__:
    return os << "plus__";
  case minus__:
    return os << "minus__";
  case star__:
    return os << "star__";
  case slash__:
    return os << "slash__";
  case amp__:
    return os << "amp__";
  case caret__:
    return os << "caret__";
  case pipe__:
    return os << "pipe__";
  case bang__:
    return os << "bang__";
  case equal__:
    return os << "equal__";
  case tidle__:
    return os << "tidle__";
  case arrow__:
    return os << "arrow__";
  case plusplus__:
    return os << "plusplus__";
  case minusminus__:
    return os << "minusminus__";
  case ampamp__:
    return os << "ampamp__";
  case pipepipe__:
    return os << "pipepipe__";
  case plusequal__:
    return os << "plusequal__";
  case minusequal__:
    return os << "minusequal__";
  case starequal__:
    return os << "starequal__";
  case slashequal__:
    return os << "slashequal__";
  case ampequal__:
    return os << "ampequal__";
  case caretequal__:
    return os << "caretequal__";
  case pipeequal__:
    return os << "pipeequal__";
  case bangequal__:
    return os << "bangequal__";
  case equalequal__:
    return os << "equalequal__";
  case var__:
    return os << "var__";
  case fn__:
    return os << "fn__";
  case for__:
    return os << "for__";
  case if__:
    return os << "if__";
  case else__:
    return os << "else__";
  case print__:
    return os << "print__";
  case return__:
    return os << "return__";
  case struct__:
    return os << "struct__";
  case iden__:
    return os << "iden__";
  case number__:
    return os << "number__";
  case string__:
    return os << "string__";
  case eof__:
    return os << "eof__";
  case error__:
    return os << "error__";
  }
}