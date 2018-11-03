enum
{
    TK_NUM = 256,
    TK_IDENT,
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
    ND_IDENT,
};

typedef struct Node
{
    int type;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
} Node;

Token *tokenize(char *p);
Node *program(Token *tokens);