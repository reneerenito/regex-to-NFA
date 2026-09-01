#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef enum
{
    TOKEN_LITERAL,
    TOKEN_OPERATOR,
    TOKEN_LPAREN,
    TOKEN_RPAREN
} token_type;

typedef struct
{
    token_type type;
    char value;
} token;

typedef struct
{
    token *items;
    int size;
} token_list;

typedef enum
{
    TOKENIZE_OK,
    TOKENIZE_ERR_EMPTY,
    TOKENIZE_ERR_INVALID_CHAR,
    TOKENIZE_ERR_UNBALANCED_PARENS,
    TOKENIZE_ERR_EMPTY_PARENS,
    TOKENIZE_ERR_MISPLACED_OPERATOR
} tokenize_status;

tokenize_status tokenize(const char *str, token_list *out);

const char *tokenize_error_message(tokenize_status status);
void free_token_list(token_list *list);

#endif
