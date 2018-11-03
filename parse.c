#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';')
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

        if ('a' <= *p && *p <= 'z')
        {
            t->type = TK_IDENT;
            t->input = p;
            i++;
            p++;
            continue;
        }

        fprintf(stderr, "can not tokenize: %s\n", p);
        exit(1);
    }

    tokens[i].type = TK_EOF;
    tokens[i].input = p;

    return tokens;
}
