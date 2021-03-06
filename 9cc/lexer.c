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

// 英数字かアンダースコアかどうか判定
int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
          (c == '_');
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

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
    {
      cur = new_token(TK_RETURN, cur, p);
      p += 6;
      continue;
    }

    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2]))
    {
      cur = new_token(TK_IF, cur, p);
      p += 2;
      continue;
    }

    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4]))
    {
      cur = new_token(TK_ELSE, cur, p);
      p += 4;
      continue;
    }

    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5]))
    {
      cur = new_token(TK_WHILE, cur, p);
      p += 5;
      continue;
    }

    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3]))
    {
      cur = new_token(TK_FOR, cur, p);
      p += 3;
      continue;
    }

    if (strncmp(p, "==", 2) == 0)
    {
      cur = new_token(TK_EQ_EQ, cur, p);
      p += 2;
      continue;
    }

    if (strncmp(p, "!=", 2) == 0)
    {
      cur = new_token(TK_NOT_EQ, cur, p);
      p += 2;
      continue;
    }

    if (strncmp(p, ">=", 2) == 0)
    {
      cur = new_token(TK_GE, cur, p);
      p += 2;
      continue;
    }

    if (strncmp(p, "<=", 2) == 0)
    {
      cur = new_token(TK_LE, cur, p);
      p += 2;
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

    if (*p == '&')
    {
      cur = new_token(TK_AND, cur, p++);
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

    if (*p == '{')
    {
      cur = new_token(TK_LBRACE, cur, p++);
      continue;
    }

    if (*p == '}')
    {
      cur = new_token(TK_RBRACE, cur, p++);
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

    if (*p == ';')
    {
      cur = new_token(TK_SEMC, cur, p++);
      continue;
    }

    if (*p == ',')
    {
      cur = new_token(TK_COMMA, cur, p++);
      continue;
    }

    if (*p == '=')
    {
      cur = new_token(TK_EQ, cur, p++);
      continue;
    }

    if ('a' <= *p && *p <= 'z')
    {
      int j = 0;
      // 変数が何文字続くかカウント
      do {
        j++;
      } while (is_alnum(p[j]));
      cur = new_token(TK_IDENT, cur, p);
      cur->name = strndup(p, j);
      p += j;
      continue;
    }

    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    perror("トークナイズできません");
    exit(1);
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}
