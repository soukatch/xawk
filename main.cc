#include "parser.h"
#include "vm.h"

int main() { vm{parser{"main.xawk"}.parse()}(); }