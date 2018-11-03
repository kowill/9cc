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

typedef struct
{
    void **data;
    int capacity;
    int len;
} Vector;

typedef struct
{
    Vector *keys;
    Vector *vals;
} Map;

Token *tokenize(char *p);
Node *program(Token *tokens);
void gen(Node *node);

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

//  for DEBUG
void runtest();