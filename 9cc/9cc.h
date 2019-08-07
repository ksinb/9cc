#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// vector.c
typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

// map.c
typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
bool map_exists(Map *map, char *key);

// lexer.c
// トークンの型
enum {
  TK_NUM,    // 数値
  TK_IDENT,  // 識別子
  TK_RETURN, // return
  TK_IF,     // if
  TK_ELSE,   // else
  TK_WHILE,  // while
  TK_FOR,    // for
  TK_ADD,    // +
  TK_SUB,    // -
  TK_DIV,    // /
  TK_MUL,    // *
  TK_AND,    // &
  TK_LPAR,   // (
  TK_RPAR,   // )
  TK_LBRACE, // {
  TK_RBRACE, // }    
  TK_EQ,     // =
  TK_EQ_EQ,  // ==
  TK_NOT_EQ, // !=
  TK_LT,     // <
  TK_LE,     // <=  
  TK_GT,     // >
  TK_GE,     // >=
  TK_COMMA,  // ,
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

Token *token; // 現在のトークンを表すグローバル変数
Token *tokenize(char *p);

// parser.c
// 抽象構文木のノードの型
typedef enum {
  ND_IDENT,     // 識別子
  ND_NUM,       // 数値
  ND_RETURN,    // return
  ND_IF,        // if
  ND_IF_ELSE,   // if else
  ND_WHILE,     // while
  ND_FOR,       // for
  ND_COMP_STMT, // Compound statement
  ND_FUNC_DEF,  // 関数定義
  ND_FUNC_CALL, // 関数呼び出し
  ND_ADD,       // +
  ND_SUB,       // -
  ND_DIV,       // /
  ND_MUL,       // *
  ND_ASSIGN,    // =
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_DEREF,     // *
  ND_ADDR       // &
} NodeType;

typedef struct Node Node;

struct Node {
  NodeType type;      // ノードの型
  Node *lhs;          // 左辺
  Node *rhs;          // 右辺
  Node *expr;
  
  char *name;         // typeがND_IDENT, ND_FUNC_DEFの場合
  int val;            // typeがND_NUMの場合のみ使う
  
  Node *cond;         // typeがND_IF|ND_IF_ELSE|ND_WHILE|ND_FORの場合のみ使う
  Node *then_stmt;    // typeがND_IF|ND_IF_ELSE|ND_WHILEの場合のみ使う
  Node *else_stmt;    // typeがND_IF_ELSEの場合のみ使う
  Node *init_cond;    // typeがND_FORの場合のみ
  Node *update_cond;  // typeがND_FORの場合のみ
  Vector *stmts;      // typeがND_COMPOUND_STATEMENTSの場合のみ
  Vector *params;     // typeがND_FUNC_DEF
  Node *body;         // typeがND_FUNC_DEF
  Vector *args;       // typeがND_FUNC_CALL
};

void error(char *fmt, ...);
void expect(char op);
int expect_number();
char *expect_ident();
bool at_eof();
bool consume(int op);

// EBNF
Node *code[100]; // グローバル変数
void *program();
Node *function();
Node *compound_stmt();
Node *stmt();
Node *expr();
Node *assign();
Node *equility();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

// codegen.c
void gen_func(Node *node);
void gen(Node *node);
