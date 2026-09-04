/* Pruebas de parse_regex. Mientras Shunting Yard no esté implementado el
   contrato es regresar los tokens en orden infijo con la concatenación
   explícita. Cuando la parte dos exista basta con actualizar los valores
   esperados a la notación postfija. */

#include "regex.h"
#include <stdio.h>
#include <stdlib.h>
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

static void esperar(const char *entrada, const char *esperado)
{
    char nombre[128];
    snprintf(nombre, sizeof(nombre), "\"%s\" produce \"%s\"", entrada, esperado);

    regex r = parse_regex(entrada);

    char valores[128];
    int i;
    for (i = 0; i < r.size && i < 127; i++)
        valores[i] = r.items[i].value;
    valores[i] = '\0';

    check(strcmp(valores, esperado) == 0, nombre);
    free(r.items);
}

int main(void)
{
    printf("Pruebas de parse_regex\n\n");

    esperar("ab", "a.b");
    esperar("a(b|c)*", "a.(b|c)*");
    esperar("(ab)*", "(a.b)*");
    esperar("a|b|c", "a|b|c");
    esperar(" (ab) ", "(a.b)");

    printf("\n%d pruebas, %d fallidas\n", total, fallidas);
    return fallidas > 0;
}
