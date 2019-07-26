#include "9cc.h"

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
  if (token->type != type) {
    error("token error: '%d' expected, but got '%d'", type, token->type);
  }
  token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す
// それ以外の場合には-1を返す
int expect_number() {
  if (token->type != TK_NUM) {
    return -1;
  }
  int val = token->val;
  token = token->next;
  return val;
}

// 次のトークンが識別子の場合、トークンを一つ読み進めてその名前を返す
// それ以外の場合には0を返す
char *expect_ident() {
  if (token->type != TK_IDENT) {
    return 0;
  }
  char *name = token->name;
  token = token->next;
  return name;
}

bool at_eof() {
  return token->type == TK_EOF;
}

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

// 変数を名前で検索する
// 見つからなかった場合はNULLを返す
LVar *find_lvar(char *name) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == strlen(name) && !memcmp(name, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}
Node *new_node_ident(char *name) {
  Node *node = calloc(1, sizeof(Node));
  node->type = ND_LVAR;
  LVar *lvar = find_lvar(name);
  
  if (lvar) {
    node->offset = lvar->offset;
  } else {
    lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = name;
    lvar->len = strlen(name);
    lvar->offset = locals ? locals->offset + 8 : 0;
    node->offset = lvar->offset;
    locals = lvar;
  }
  return node;
}

void *program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = stmt();
  }
  code[i] = NULL;
}

Node *stmt() {
  Node *node = expr();
  expect(TK_SEMC);
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equility();
  if (consume(TK_EQ)) {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *equility() {
  Node *node = relational();

  for (;;) {
    if (consume(TK_EQ_EQ)) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume(TK_NOT_EQ)) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume(TK_LT)) {
      return new_node(ND_LT, node, add());
    } 
    if (consume(TK_GT)) {
      return new_node(ND_LT, add(), node);
    }
    if (consume(TK_LE)) {
      return new_node(ND_LE, node, add());
    }
    if (consume(TK_GE)) {
      return new_node(ND_LE, add(), node);
    }
    return node;
  }
}

Node *add() {
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

Node *unary() {
  if (consume(TK_ADD)) {
    return term();
  }
  if (consume(TK_SUB)) {
    return new_node(ND_SUB, new_node_num(0), term());
  }
  return term();
}

Node *term() {
  int number = expect_number();
  if (number != -1) {
    return new_node_num(number);
  }

  char *name = expect_ident();
  if (name) {
    return new_node_ident(name);
  }

  // "(" expr ")"のはず
  expect(TK_LPAR);
  Node *node = expr();
  expect(TK_RPAR);
  return node;
}
