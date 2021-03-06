#include "9cc.h"

Vector *arguments();

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

Node *new_node(NodeType type) {
  Node *node = calloc(1, sizeof(Node));
  node->type = type;
  return node;
}

Node *new_binop(NodeType type, Node *lhs, Node *rhs) {
  Node *node = new_node(type);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_expr(NodeType type, Node *expr) {
  Node *node = new_node(type);
  node->expr = expr;
  return node;
}

Node *new_node_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

void *program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = function();
  }
  code[i] = NULL;
}

Node *function() {
  char *name = expect_ident();
  
  // Function
  if (consume(TK_LPAR)) {
    Node *node = new_node(ND_FUNC_DEF);
    node->name = name;
    
    Vector *params = new_vector();
    while (!consume(TK_RPAR)) {
      if (params->len > 0)  {
        expect(TK_COMMA);
      }
      vec_push(params, (void * )expect_ident());
    }
    node->params = params;
    
    expect(TK_LBRACE);
    node->body = compound_stmt();
    return node;
  }
}

Node *compound_stmt() {
  Node *node = new_node(ND_COMP_STMT);
  node->stmts = new_vector();

  while (!consume(TK_RBRACE)) {
    vec_push(node->stmts, stmt());
  }

  return node;
}

Node *stmt() {
  Node *node;

  // "{" stmt* "}"
  if (consume(TK_LBRACE)) {
    return compound_stmt();
  }

  // "if" "(" expr ")" stmt ("else" stmt)?
  if (consume(TK_IF)) {
    expect(TK_LPAR);
    Node *if_expr = expr();
    expect(TK_RPAR);
    Node *if_stmt = stmt();
    
    node = calloc(1, sizeof(Node));
    node->cond = if_expr;
    node->then_stmt = if_stmt;
    
    if (consume(TK_ELSE)) {
      node->type = ND_IF_ELSE;
      node->else_stmt = stmt();
    } else {
      node->type = ND_IF;
    }
    return node;
  }

  // "while" "(" expr ")" stmt
  if (consume(TK_WHILE)) {
    expect(TK_LPAR);
    Node *while_expr = expr();
    expect(TK_RPAR);
    Node *while_stmt = stmt();
    
    node = new_node(ND_WHILE);
    node->cond = while_expr;
    node->then_stmt = while_stmt;
    return node;
  }

  // "for" "(" expr? ";" expr? ";" expr? ")" stmt
  if (consume(TK_FOR)) {
    node = new_node(ND_FOR);
    
    expect(TK_LPAR);

    if (consume(TK_SEMC)) {
      node->init_cond = NULL;
    } else {
      node->init_cond = expr();
      expect(TK_SEMC);
    }
    
    if (consume(TK_SEMC)) {
      node->cond = NULL;
    } else {
      node->cond = expr();
      expect(TK_SEMC);
    }

    if (consume(TK_RPAR)) {
      node->update_cond = NULL;
    } else {
      node->update_cond = expr();
      expect(TK_RPAR);
    }
    
    node->then_stmt = stmt();
    return node;
  }
  
  // "return" expr ";"
  if (consume(TK_RETURN)) {
    node = new_expr(ND_RETURN, expr());    
  } else {
    node = expr();
  }
  
  expect(TK_SEMC);
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equility();
  if (consume(TK_EQ)) {
    node = new_binop(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *equility() {
  Node *node = relational();

  for (;;) {
    if (consume(TK_EQ_EQ)) {
      node = new_binop(ND_EQ, node, relational());
    } else if (consume(TK_NOT_EQ)) {
      node = new_binop(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume(TK_LT)) {
      return new_binop(ND_LT, node, add());
    } 
    if (consume(TK_GT)) {
      return new_binop(ND_LT, add(), node);
    }
    if (consume(TK_LE)) {
      return new_binop(ND_LE, node, add());
    }
    if (consume(TK_GE)) {
      return new_binop(ND_LE, add(), node);
    }
    return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume(TK_ADD)) {
      node = new_binop(ND_ADD, node, mul());
    } else if (consume(TK_SUB)) {
      node = new_binop(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();
  
  for (;;) {
    if (consume(TK_MUL)) {
      node = new_binop(ND_MUL, node, unary());
    } else if (consume(TK_DIV)) {
      node = new_binop(ND_DIV, node, unary());
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
    return new_binop(ND_SUB, new_node_num(0), term());
  }
  if (consume(TK_MUL)) {
    return new_expr(ND_DEREF, unary());
  }
  if (consume(TK_AND)) {
    return new_expr(ND_ADDR, unary());
  }
  return term();
}

Node *term() {
  // "(" expr ")"
  if (consume(TK_LPAR)) {
    Node *node = expr();
    expect(TK_RPAR);
    return node;
  }

  // num
  int number = expect_number();
  if (number != -1) {
    return new_node_num(number);
  }

  char *name = expect_ident();
  if (name) {
    if (!consume(TK_LPAR)) {
      // 変数
      Node *node = new_node(ND_IDENT);
      node->name = name;
      return node;
    }
    // 関数の呼び出し
    Vector *args = new_vector();
    while (!consume(TK_RPAR)) {
      if (args->len > 0) {
        expect(TK_COMMA);
      }
      vec_push(args, (void *)term());
    }
    Node *node = new_node(ND_FUNC_CALL);
    node->name = name;
    node->args = args;

    return node;
  }
}
