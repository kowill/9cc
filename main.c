#include <stdio.h>
#include "9cc.h"

int main(int argc, char **argv)
{
    if (argc ==2 && !strcmp(argv[1], "-test"))
    {
        runtest();
        return 0;
    }

    if (argc != 2)
    {
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    Token *tokens = tokenize(argv[1]);
    Node *code = program(tokens);

    // init part
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //prologue 変数領域
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", 8 * 26); //26個分

    for (int i = 0; code[i].type; i++)
    {
        gen(&code[i]);
        printf("  pop rax\n");
    }

    //epilogue
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret \n");
    return 0;
}