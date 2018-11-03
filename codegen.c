#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

void error_token(Token *token)
{
    fprintf(stderr, "不正なトークンです: %s\n", token->input);
    exit(1);
}

void error_node(Node *node)
{
    fprintf(stderr, "不正なノードです: %d\n", node->type);
    exit(1);
}
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

Node *new_node_ident(char *c)
{
    Node *node = malloc(sizeof(Node));
    node->type = ND_IDENT;
    node->name = *c;
    return node;
}

int pos = 0;

Node *expr(Token *tokens);

Node *term(Token *tokens)
{
    if (tokens[pos].type == TK_NUM)
        return new_node_num(tokens[pos++].val);
    if (tokens[pos].type == TK_IDENT)
        return new_node_ident(tokens[pos++].input);
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

Node *assign(Token *tokens)
{
    Node *lhs = expr(tokens);
    if (tokens[pos].type == TK_EOF)
    {
        return lhs;
    }
    if (tokens[pos].type == ';')
    {
        pos++;
        return lhs;
    }
    if (tokens[pos].type == '=')
    {
        pos++;
        return new_node('=', lhs, assign(tokens));
    }

    return lhs;
}

Node *program(Token *tokens)
{
    Node *code = malloc(sizeof(Node) *100);
    memset(code, 0, sizeof(code));
    int num = 0;

    while (tokens[pos].type != TK_EOF)
    {
        Node *node = assign(tokens);
        code[num] = *node;
        num++;
    }

    return code;
}

void gen_lval(Node *node)
{
    if (node->type == ND_IDENT)
    {
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", ('z' - node->name + 1) * 8);
        printf("  push rax\n");
        return;
    }
    error_node(node);
}

void gen(Node *node)
{
    if (node->type == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    if (node->type == ND_IDENT)
    {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    }

    if (node->type == '=')
    {
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
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