#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
      return 1;
  }

  // トークナイズしてパースする
  char *user_input = argv[1];
  token = tokenize(user_input);
  program();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n"); 

  for (int i = 0; code[i]; i++) {
    gen_func(code[i]);
  }

  return 0;
}