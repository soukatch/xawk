#include "lexer.h"

int main() {
  lexer l{"main.xawk"};
  for (;;) {
    token t = l();
    std::cout << t << std::endl;
    if (t.type_ == token_type::eof__)
      break;
  }
}