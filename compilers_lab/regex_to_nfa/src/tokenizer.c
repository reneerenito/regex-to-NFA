#include "tokenizer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static int is_unary_operator(char c)
{
    return c == '*' || c == '+' || c == '?';
}

static int needs_concat(const token *prev, const token *curr)
{
    if (prev == NULL)
        return 0;

    int prev_ends_expr = prev->type == TOKEN_LITERAL ||
                         prev->type == TOKEN_RPAREN ||
                         (prev->type == TOKEN_OPERATOR && is_unary_operator(prev->value));
    int curr_starts_expr = curr->type == TOKEN_LITERAL ||
                           curr->type == TOKEN_LPAREN;

    return prev_ends_expr && curr_starts_expr;
}

static void push(token_list *list, token_type type, char value)
{
    list->items[list->size].type = type;
    list->items[list->size].value = value;
    list->size++;
}

tokenize_status tokenize(const char *str, token_list *out)
{
    int len = strlen(str);
    out->size = 0;
    out->items = malloc(sizeof(token) * (len * 2 + 1));

    int depth = 0;
    tokenize_status status = TOKENIZE_OK;

    for (int i = 0; i < len && status == TOKENIZE_OK; i++)
    {
        char c = str[i];
        token curr;

        if (isspace((unsigned char)c))
            continue;

        if (isalnum((unsigned char)c))
        {
            curr.type = TOKEN_LITERAL;
        }
        else if (c == '|' || is_unary_operator(c))
        {
            curr.type = TOKEN_OPERATOR;
        }
        else if (c == '(')
        {
            curr.type = TOKEN_LPAREN;
            depth++;
        }
        else if (c == ')')
        {
            curr.type = TOKEN_RPAREN;
            depth--;
            if (depth < 0)
            {
                status = TOKENIZE_ERR_UNBALANCED_PARENS;
                break;
            }
        }
        else
        {
            status = TOKENIZE_ERR_INVALID_CHAR;
            break;
        }
        curr.value = c;

        token *prev = out->size > 0 ? &out->items[out->size - 1] : NULL;

        if (curr.type == TOKEN_OPERATOR)
        {
            int prev_is_operand = prev != NULL &&
                                  (prev->type == TOKEN_LITERAL ||
                                   prev->type == TOKEN_RPAREN ||
                                   (prev->type == TOKEN_OPERATOR && is_unary_operator(prev->value)));
            if (!prev_is_operand)
            {
                status = TOKENIZE_ERR_MISPLACED_OPERATOR;
                break;
            }
        }

        if (curr.type == TOKEN_RPAREN && prev != NULL &&
            prev->type == TOKEN_OPERATOR && prev->value == '|')
        {
            status = TOKENIZE_ERR_MISPLACED_OPERATOR;
            break;
        }

        if (curr.type == TOKEN_RPAREN && prev != NULL && prev->type == TOKEN_LPAREN)
        {
            status = TOKENIZE_ERR_EMPTY_PARENS;
            break;
        }

        if (needs_concat(prev, &curr))
            push(out, TOKEN_OPERATOR, '.');
        push(out, curr.type, curr.value);
    }

    if (status == TOKENIZE_OK)
    {
        if (out->size == 0)
            status = TOKENIZE_ERR_EMPTY;
        else if (depth != 0)
            status = TOKENIZE_ERR_UNBALANCED_PARENS;
        else
        {
            token *last = &out->items[out->size - 1];
            if (last->type == TOKEN_OPERATOR && last->value == '|')
                status = TOKENIZE_ERR_MISPLACED_OPERATOR;
        }
    }

    if (status != TOKENIZE_OK)
        free_token_list(out);
    return status;
}

const char *tokenize_error_message(tokenize_status status)
{
    switch (status)
    {
    case TOKENIZE_ERR_EMPTY:
        return "regex vacia";
    case TOKENIZE_ERR_INVALID_CHAR:
        return "caracter invalido en la regex";
    case TOKENIZE_ERR_UNBALANCED_PARENS:
        return "parentesis desbalanceados";
    case TOKENIZE_ERR_EMPTY_PARENS:
        return "parentesis vacios '()'";
    case TOKENIZE_ERR_MISPLACED_OPERATOR:
        return "operador sin operando";
    default:
        return "ok";
    }
}

void free_token_list(token_list *list)
{
    free(list->items);
    list->items = NULL;
    list->size = 0;
}
