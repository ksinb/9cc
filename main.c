#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
      return 1;
  }

  // トークナイズしてパースする
  char *user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expr();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n"); 
  printf("main:\n");

  // 抽象構文木を下りながらコードを生成
  gen(node);

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数からの返り値とする
  printf("    pop rax\n");
  printf("    ret\n");
  
  // `+ <数>` あるいは `- <数>` というトークンの並びを消費しつつアセンブリを出力
  while (!at_eof()) {
    if (consume('+')) {
      printf("    add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("    sub rax, %d\n", expect_number());
  }

  printf("    ret\n");
  return 0;
}