#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF       // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

// トークン型
Token *token;

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_DIV,
  ND_MUL, // *
  ND_NUM // 整数
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
};

Node *expr();
Node *mul();
Node *term();
Node *unary();
bool consume(char op);
void expect(char op);
int expect_number();
void gen(Node *node);
Token *tokenize(char *p);
bool at_eof();