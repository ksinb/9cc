#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型
enum {
  TK_NUM,  // 数値
  TK_ADD,  // +
  TK_SUB,  // -
  TK_DIV,  // /
  TK_MUL,  // *
  TK_LPAR, // (
  TK_RPAR, // )
  TK_GT,   // >
  TK_LT,   // <
  TK_GE,   // >=
  TK_LE,   // <=  
  TK_EOF   // 入力の終わりを表すトークン
};

typedef struct Token Token;

struct Token {
  int type; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

// トークン型
Token *token;

// 抽象構文木のノードの型
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_DIV,
  ND_MUL, // *
  ND_NUM // 整数
} NodeType;

typedef struct Node Node;

struct Node {
  NodeType type; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
};

// lexer
Token *tokenize(char *p);

// parser
bool consume(int op);
void expect(char op);
int expect_number();
bool at_eof();

// EBNF
Node *expr();
Node *mul();
Node *term();
Node *unary();

// codegen
void gen(Node *node);
bool at_eof();