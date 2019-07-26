#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型
enum {
  TK_NUM,    // 数値
  TK_IDENT,  // 識別子
  TK_ADD,    // +
  TK_SUB,    // -
  TK_DIV,    // /
  TK_MUL,    // *
  TK_LPAR,   // (
  TK_RPAR,   // )
  TK_EQ,     // =
  TK_EQ_EQ,  // ==
  TK_NOT_EQ, // !=
  TK_LT,     // <
  TK_LE,     // <=  
  TK_GT,     // >
  TK_GE,     // >=
  TK_SEMC,   // ; 
  TK_EOF     // 入力の終わりを表すトークン
};

typedef struct Token Token;

struct Token {
  int type;       // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // typeがTK_NUMの場合、その数値
  char *name;     // typeがTK_INDENTの場合、その名前
  char *str;      // トークン文字列
};

// 抽象構文木のノードの型
typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_DIV,    // /
  ND_MUL,    // *
  ND_ASSIGN, // =
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=  
  ND_LVAR,   // ローカル変数
  ND_NUM     // 整数
} NodeType;

typedef struct Node Node;

struct Node {
  NodeType type; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
  int offset;    // kindがND_LVARの場合のみ使う
};

typedef struct LVar LVar;

//ローカル変数の型
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

LVar *locals;

// lexer
Token *token; // 現在のトークンを表すグローバル変数
Token *tokenize(char *p);

// parser
void error(char *fmt, ...);
void expect(char op);
int expect_number();
char *expect_ident();
bool at_eof();
bool consume(int op);

// EBNF
Node *code[100]; // グローバル変数
void *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equility();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

// codegen
void gen(Node *node);
