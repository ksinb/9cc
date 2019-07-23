#include "9cc.h"

// 次のトークンが期待している記号のときは、トークンを一つ読み進めて真を返す
// それ以外の場合は偽を返す
bool consume(int type) {
  if (token->type != type) {
    return false;
  }

  token = token->next;
  return true;
}

Node *new_node(NodeType type, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->type = type;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->type = ND_NUM;
  node->val = val;
  return node;
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 次のトークンが期待している記号のときは、トークンを一つ読み進める
// それ以外の場合にはエラーを報告する
void expect(char type) {
  if (token->type != type)
  {
    error("expect: '%d'は%c'ではありません", token->type, type);
  }
  token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す
// それ以外の場合にはエラーを報告する
int expect_number() {
  if (token->type != TK_NUM)
  {
    error("数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->type == TK_EOF;
}

Node *expr() {
  Node *node = mul();

  for (;;) {
    if (consume(TK_ADD)) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume(TK_SUB)) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();
  
  for (;;) {
    if (consume(TK_MUL)) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume(TK_DIV)) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *term() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume(TK_LPAR)) {
    Node *node = expr();
    expect(TK_RPAR);
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

Node *unary() {
  if (consume(TK_ADD)) {
    return term();
  }
  if (consume(TK_SUB)) {
    return new_node(ND_SUB, new_node_num(0), term());
  }
  return term();
}
