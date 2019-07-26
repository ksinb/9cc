#include "9cc.h"

int label_count = 0;

void gen_lval(Node *node) {
  if (node->type != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }

  printf("    mov rax, rbp\n");
  printf("    sub rax, %d\n", node->offset);
  printf("    push rax\n");
}

void gen(Node *node) {
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

  switch (node->type) {
    case ND_NUM:
      printf("    push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("    pop rax\n");
      printf("    mov rax, [rax]\n");
      printf("    push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("    pop rdi\n");
      printf("    pop rax\n");
      printf("    mov [rax], rdi\n");
      printf("    push rdi\n");
      return;
  }

  gen(node->lhs);
  gen(node->rhs);
  
  printf("    pop rdi\n");
  printf("    pop rax\n");

  switch (node->type) {
    case ND_ADD:
      printf("    add rax, rdi\n");
      break;
    case ND_SUB:
      printf("    sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("    imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("    cqo\n");
      printf("    idiv rdi\n");
      break;
    case ND_EQ: 
      printf("    cmp rax, rdi\n");
      printf("    sete al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_NE: 
      printf("    cmp rax, rdi\n");
      printf("    setne al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_LE: 
      printf("    cmp rax, rdi\n");
      printf("    setle al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_LT: 
      printf("    cmp rax, rdi\n");
      printf("    setl al\n");
      printf("    movzb rax, al\n");
      break;
  }

  printf("    push rax\n");
}  