#include "9cc.h"

int label_count = 0;
Map *vars;
int stack_size;

const char *registers[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_lval(Node *node) {
  if (node->type != ND_IDENT) {
    error("代入の左辺値が変数ではありません");
  }

  if (!map_exists(vars, node->name)) {
    stack_size += 8;
    map_put(vars, node->name, (void *)(intptr_t)stack_size);
  }

  int offset = (intptr_t)map_get(vars, node->name);
  printf("    mov rax, rbp\n");
  printf("    sub rax, %d\n", offset);
  printf("    push rax\n");
}

void gen_args(Vector *nodes) {
  if (nodes->len == 0) {
    return;
  }

  for (int i = 0; i < nodes->len; i++) {
    printf("    mov [rbp-%d], %s\n", (i+1) * 8, registers[i]);
  }
}

void gen(Node *node) {
  if (node->type == ND_COMP_STMT) {
    Vector *statements = node->stmts;
    for (int i = 0; i < statements->len; i++) {
      Node *node = (Node *)statements->data[i];
      gen(node);
      printf("    pop rax\n");
    }
    return;
  }

  if (node->type == ND_RETURN) {
    gen(node->lhs);
    printf("    pop rax\n");
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return;
  }

  if (node->type == ND_IF) {
    int _label_count = label_count;
    label_count++;

    gen(node->cond);
    printf("    pop rax\n");
    printf("    cmp rax, 0\n");
    printf("    je  .Lend%d\n", _label_count);
    gen(node->then_stmt);
    printf(".Lend%d:\n", _label_count);
    return;
  }
  
  if (node->type == ND_IF_ELSE) {
    int _label_count = label_count;
    label_count++;

    gen(node->cond);
    printf("    pop rax\n");
    printf("    cmp rax, 0\n");
    printf("    je  .Lelse%d\n", _label_count);
    gen(node->then_stmt);
    printf("    jmp .Lend%d\n", _label_count);
    printf(".Lelse%d:\n", _label_count);
    gen(node->else_stmt);
    printf(".Lend%d:\n", _label_count);
    return;
  }

  if (node->type == ND_WHILE) {
    int _label_count = label_count;
    label_count++;

    printf(".Lbegin%d:\n", _label_count);
    gen(node->cond);
    printf("    pop rax\n");
    printf("    cmp rax, 0\n");
    printf("    je  .Lend%d\n", _label_count);
    gen(node->then_stmt);
    printf("    jmp .Lbegin%d\n", _label_count);
    printf(".Lend%d:\n", _label_count);
    return;
  }

  if (node->type == ND_FOR) {
    int _label_count = label_count;
    label_count++;

    if (node->init_cond) {
      gen(node->init_cond);
    }
    printf(".Lbegin%d:\n", _label_count);
    if (node->cond) {
      gen(node->cond);
      printf("    pop rax\n");
      printf("    cmp rax, 0\n");
      printf("    je  .Lend%d\n", _label_count);
    }
    gen(node->then_stmt);
    if (node->update_cond) {
      gen(node->update_cond);
    }
    printf("    jmp .Lbegin%d\n", _label_count);
    if (node->cond) {
      printf(".Lend%d:\n", _label_count);
    }
    return;
  }

  if (node->type == ND_FUNC_CALL) {
    for (int i = 0; i < node->args->len; i++) {
      gen((Node *)node->args->data[i]);
      printf("    pop %s\n", registers[i]);
    }
    printf("    call %s\n", node->name);
    printf("    push rax\n");
    return;
  }

  switch (node->type) {
    case ND_NUM:
      printf("    push %d\n", node->val);
      return;
    case ND_IDENT:
      gen_lval(node);
      printf("    pop rax\n");
      printf("    mov rax, [rax]\n");
      printf("    push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("    pop r10\n");
      printf("    pop rax\n");
      printf("    mov [rax], r10\n");
      printf("    push r10\n");
      return;
  }

  gen(node->lhs);
  gen(node->rhs);
  
  printf("    pop r10\n");
  printf("    pop rax\n");

  switch (node->type) {
    case ND_ADD:
      printf("    add rax, r10\n");
      break;
    case ND_SUB:
      printf("    sub rax, r10\n");
      break;
    case ND_MUL:
      printf("    imul rax, r10\n");
      break;
    case ND_DIV:
      printf("    cqo\n");
      printf("    idiv r10\n");
      break;
    case ND_EQ: 
      printf("    cmp rax, r10\n");
      printf("    sete al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_NE: 
      printf("    cmp rax, r10\n");
      printf("    setne al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_LE: 
      printf("    cmp rax, r10\n");
      printf("    setle al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_LT: 
      printf("    cmp rax, r10\n");
      printf("    setl al\n");
      printf("    movzb rax, al\n");
      break;
  }

  printf("    push rax\n");
}  

void gen_func(Node *node) {
  stack_size = 0;
  vars = new_map();
  
  printf("%s:\n", node->name);

  // プロローグ
  // 変数26個分の領域を確保する
  printf("    push rbp\n");
  printf("    mov rbp, rsp\n");
  printf("    sub rsp, 208\n");

  gen_args(node->params);

  for (int i = 0; i < node->body->stmts->len; i++) {
    gen((Node *)node->body->stmts->data[i]);
  }
}