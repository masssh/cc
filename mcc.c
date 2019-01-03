#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型
enum {
  TOKEN_NUM = 256, // 整数トークン
  TOKEN_EOF,       // ファイル終端トークン
};
typedef struct {
  int token_type; // トークンの型
  int value;      // トークンの値
  char *input;    // トークンの文字列
} Token;

// トークナイズ結果を格納する配列
Token tokens[100];

// トークナイザ
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    // 空白文字は無視
    if (isspace(*p)) {
      p++;
    }

    // 演算子
    if (*p == '+' || *p == '-') {
      tokens[i].token_type = *p;
      tokens[i].input = p;
      p++;
      i++;
      continue;
    }

    // 整数
    if (isdigit(*p)) {
      tokens[i].token_type = TOKEN_NUM;
      tokens[i].input = p;
      tokens[i].value = strtol(p, &p, 10);
      i++;
      continue;
    }

    fprintf(stderr, "トークナイズできません: %s\n", p);
    exit(1);
  }

  tokens[i].token_type = TOKEN_EOF;
  tokens[i].input = p;
}

// エラー関数
void error(int i) {
  fprintf(stderr, "予期せぬトークンです: %s\n", tokens[i].input);
  exit(1);
}

int main(int argc, char **argv) {
  // 入力チェック
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // トークナイズ
  tokenize(argv[1]);

  // アセンブリ出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  if (tokens[0].token_type != TOKEN_NUM) {
    error(0);
  }

  printf("  mov rax, %d\n", tokens[0].value);

  int i = 1;
  while (tokens[i].token_type != TOKEN_EOF) {
    // 加算
    if (tokens[i].token_type == '+') {
      i++;
      if (tokens[i].token_type != TOKEN_NUM) {
        error(i);
      }
      printf("  add rax, %d\n", tokens[i].value);
      i++;
      continue;
    }

    // 減算
    if (tokens[i].token_type == '-') {
      i++;
      if (tokens[i].token_type != TOKEN_NUM) {
        error(i);
      }
      printf("  sub rax, %d\n", tokens[i].value);
      i++;
      continue;
    }

    error(i);
    return 1;
  }

  printf("  ret\n");
  return 0;
}