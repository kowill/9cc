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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')')
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

void error_token(Token *token)
{
    fprintf(stderr, "不正なトークンです: %s\n", token->input);
    exit(1);
}

Node *expr(Token *tokens);

Node *term(Token *tokens)
{
    if (tokens[pos].type == TK_NUM)
        return new_node_num(tokens[pos++].val);
    if (tokens[pos].type == '(')
    {
        pos++;
        Node *node = expr(tokens);
        if (tokens[pos].type == ')')
        {
            pos++;
            return node;
        }
    }
    error_token(&tokens[pos]);
}

Node *mul(Token *tokens)
{
    Node *lhs = term(tokens);
    if (tokens[pos].type == TK_EOF)
        return lhs;
    if (tokens[pos].type == '*')
    {
        pos++;
        return new_node('*', lhs, mul(tokens));
    }
    if (tokens[pos].type == '/')
    {
        pos++;
        return new_node('/', lhs, mul(tokens));
    }

    return lhs;
}

Node *expr(Token *tokens)
{
    Node *lhs = mul(tokens);
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
    return lhs;
}

int idx = 0;
void gen(Node *node)
{
    if (node->type == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->type)
    {
    case '+':
        printf("  add rax, rdi\n");
        break;

    case '-':
        printf("  sub rax, rdi\n");
        break;

    case '*':
        printf("  mul rdi\n");
        break;

    case '/':
        printf("  mov rdx, 0\n");
        printf("  div rdi\n");
        break;

    default:
        fprintf(stderr, "対象外です: %d : %d\n", node->type, node->val);
        return;
    }
    printf("  push rax\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の数が不正です\n");
        return 1;
    }

    Token *tokens = tokenize(argv[1]);
    Node *node = expr(tokens);

    // init part
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax \n");
    printf("  ret \n");
    return 0;
}