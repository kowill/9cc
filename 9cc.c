#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    TK_NUM = 256,
    TK_EOF,
};

typedef struct
{
    int type;
    int val;
    char *input;
} Token;

enum
{
    ND_NUM = 256,
};

typedef struct Node
{
    int type;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

Node *new_node(int op, Node *lhs, Node *rhs)
{
    Node *node = malloc(sizeof(Node));
    node->type = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = malloc(sizeof(Node));
    node->type = ND_NUM;
    node->val = val;
    return node;
}

Token *tokenize(char *p)
{
    int i = 0;
    Token *tokens = malloc(sizeof(Token) * 100);

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        Token *t = &tokens[i];

        if (*p == '+' || *p == '-')
        {
            t->type = *p;
            t->input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p))
        {
            t->type = TK_NUM;
            t->input = p;
            t->val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "can not tokenize: %s\n", p);
        exit(1);
    }

    tokens[i].type = TK_EOF;
    tokens[i].input = p;

    return tokens;
}

int pos = 0;

Node *term(Token *tokens)
{
    if (tokens[pos].type == TK_NUM)
        return new_node_num(tokens[pos++].val);
    fprintf(stderr, "不正なトークンです: %s\n", tokens[pos].input);
    exit(1);
}

Node *expr(Token *tokens)
{
    Node *lhs = term(tokens);
    if (tokens[pos].type == TK_EOF)
        return lhs;
    if (tokens[pos].type == '+')
    {
        pos++;
        return new_node('+', lhs, expr(tokens));
    }
    if (tokens[pos].type == '-')
    {
        pos++;
        return new_node('-', lhs, expr(tokens));
    }
    fprintf(stderr, "不正なトークンです: %s\n", tokens[pos].input);
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    Token *tokens = tokenize(argv[1]);

    // init part
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    if (tokens[0].type != TK_NUM)
    {
        fprintf(stderr, "token がおかしいです\n");
        return 1;
    }
    printf("  mov rax, %d\n", tokens[0].val);

    int i = 1;
    while (tokens[i].type != TK_EOF)
    {
        if (tokens[i].type == '+')
        {
            i++;
            if (tokens[i].type != TK_NUM)
            {
                fprintf(stderr, "token がおかしいです\n");
                return 1;
            }
            printf("  add rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        if (tokens[i].type == '-')
        {
            i++;
            if (tokens[i].type != TK_NUM)
            {
                fprintf(stderr, "token がおかしいです\n");
                return 1;
            }
            printf("  sub rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        fprintf(stderr, "対象外です: %d : %d\n", tokens[i].type, tokens[i].val);
    }

    printf("  ret \n");
    return 0;
}