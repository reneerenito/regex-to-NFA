/* Pruebas unitarias del tokenizador. Se ejercita por separado la
   clasificación de tokens, la validación de sintaxis y la inserción del
   punto de concatenación, sin depender del resto del programa. */

#include "tokenizer.h"
#include <stdio.h>
#include <string.h>

static int total = 0;
static int fallidas = 0;

static void check(int condicion, const char *nombre)
{
    total++;
    if (condicion)
    {
        printf("  ok     %s\n", nombre);
    }
    else
    {
        fallidas++;
        printf("  FALLO  %s\n", nombre);
    }
}

/* Tokeniza la entrada y compara la secuencia de valores contra lo esperado */
static void esperar_tokens(const char *entrada, const char *esperado)
{
    char nombre[128];
    snprintf(nombre, sizeof(nombre), "\"%s\" produce \"%s\"", entrada, esperado);

    token_list lista;
    if (tokenize(entrada, &lista) != TOKENIZE_OK)
    {
        check(0, nombre);
        return;
    }

    char valores[128];
    int i;
    for (i = 0; i < lista.size && i < 127; i++)
        valores[i] = lista.items[i].value;
    valores[i] = '\0';

    check(strcmp(valores, esperado) == 0, nombre);
    free_token_list(&lista);
}

static void esperar_error(const char *entrada, tokenize_status esperado)
{
    char nombre[128];
    snprintf(nombre, sizeof(nombre), "\"%s\" reporta %s", entrada,
             tokenize_error_message(esperado));

    token_list lista;
    check(tokenize(entrada, &lista) == esperado, nombre);
}

static void probar_clasificacion(void)
{
    token_list lista;
    if (tokenize("a(b|c)*", &lista) != TOKENIZE_OK)
    {
        check(0, "clasificacion de tipos de token");
        return;
    }

    token_type esperados[] = {TOKEN_LITERAL, TOKEN_OPERATOR, TOKEN_LPAREN,
                              TOKEN_LITERAL, TOKEN_OPERATOR, TOKEN_LITERAL,
                              TOKEN_RPAREN, TOKEN_OPERATOR};
    int ok = lista.size == 8;
    for (int i = 0; ok && i < 8; i++)
        ok = lista.items[i].type == esperados[i];

    check(ok, "clasificacion de tipos de token");
    free_token_list(&lista);
}

int main(void)
{
    printf("Pruebas del tokenizador\n");

    printf("\nInsercion de concatenacion explicita\n");
    esperar_tokens("ab", "a.b");
    esperar_tokens("a(b|c)*", "a.(b|c)*");
    esperar_tokens("(ab)*", "(a.b)*");
    esperar_tokens("(ab)*c+d?", "(a.b)*.c+.d?");
    esperar_tokens("a*b", "a*.b");
    esperar_tokens("a|b|c", "a|b|c");
    esperar_tokens("a**", "a**");

    printf("\nManejo de espacios en la entrada\n");
    esperar_tokens(" a (b) ", "a.(b)");
    esperar_tokens("\ta b\t", "a.b");

    printf("\nClasificacion de tokens\n");
    probar_clasificacion();

    printf("\nValidacion de sintaxis\n");
    esperar_error("*ab", TOKENIZE_ERR_MISPLACED_OPERATOR);
    esperar_error("+ab", TOKENIZE_ERR_MISPLACED_OPERATOR);
    esperar_error("|ab", TOKENIZE_ERR_MISPLACED_OPERATOR);
    esperar_error("a||b", TOKENIZE_ERR_MISPLACED_OPERATOR);
    esperar_error("ab|", TOKENIZE_ERR_MISPLACED_OPERATOR);
    esperar_error("(a|)", TOKENIZE_ERR_MISPLACED_OPERATOR);
    esperar_error("(*a)", TOKENIZE_ERR_MISPLACED_OPERATOR);
    esperar_error("a(", TOKENIZE_ERR_UNBALANCED_PARENS);
    esperar_error("a)b", TOKENIZE_ERR_UNBALANCED_PARENS);
    esperar_error("((a)", TOKENIZE_ERR_UNBALANCED_PARENS);
    esperar_error("()", TOKENIZE_ERR_EMPTY_PARENS);
    esperar_error("a-b", TOKENIZE_ERR_INVALID_CHAR);
    esperar_error("", TOKENIZE_ERR_EMPTY);
    esperar_error("   ", TOKENIZE_ERR_EMPTY);

    printf("\n%d pruebas, %d fallidas\n", total, fallidas);
    return fallidas > 0;
}
