#include "9cc.h"

// 新しいトークンを作成してcurにつなげる
Token *new_token(int type, Token *cur, char *str)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->type = type;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p)
{
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p)
  {
    // 空白文字をスキップ
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (strncmp(p, ">=", 2) == 0)
    {
      cur = new_token(TK_GE, cur, p++);
      continue;
    }

    if (strncmp(p, "<=", 2) == 0)
    {
      cur = new_token(TK_LE, cur, p++);
      continue;
    }

    if (*p == '+')
    {
      cur = new_token(TK_ADD, cur, p++);
      continue;
    }

    if (*p == '-')
    {
      cur = new_token(TK_SUB, cur, p++);
      continue;
    }

    if (*p == '*')
    {
      cur = new_token(TK_MUL, cur, p++);
      continue;
    }

    if (*p == '/')
    {
      cur = new_token(TK_DIV, cur, p++);
      continue;
    }

    if (*p == '(')
    {
      cur = new_token(TK_LPAR, cur, p++);
      continue;
    }

    if (*p == ')')
    {
      cur = new_token(TK_RPAR, cur, p++);
      continue;
    }

    if (*p == '>')
    {
      cur = new_token(TK_GT, cur, p++);
      continue;
    }

    if (*p == '<')
    {
      cur = new_token(TK_LT, cur, p++);
      continue;
    }

    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    perror("トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}
