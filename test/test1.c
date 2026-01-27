
#include <stdio.h>

#include "../inc/lexer.h"

int main()
{
  int err = 0;

  char buf[] = "int a = 1;\n"
               "printf(\"what\");\n";

  token_tab_t token_tab;

  err = lexer_parse(buf, sizeof(buf), &token_tab);

  printf("MASHU\n");
}

