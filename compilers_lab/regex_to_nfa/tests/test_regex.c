/* Pruebas de parse_regex. Mientras Shunting Yard el
   contrato es regresar los tokens en orden posfijo con la concatenación
   explícita. */

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

    esperar("ab", "ab.");
    esperar("a(b|c)*", "abc|*.");
    esperar("(ab)*", "ab.*");
    esperar("a|b|c", "ab|c|");
    esperar("(ab)", "ab.");
	esperar("((a))", "a");
	esperar("(a|b)(c|d)", "ab|cd|.");
	esperar("a(b|c)d", "abc|.d.");
	esperar("(a|b)*c", "ab|*c.");
	esperar("(a|(b|c))", "abc||");
	esperar("a(bc)*d", "abc.*.d.");

    printf("\n%d pruebas, %d fallidas\n", total, fallidas);

	regex resultado = parse_regex("((a))");
	char resultado_string[128];
    int i;
    for (i = 0; i < resultado.size && i < 127; i++)
        resultado_string[i] = resultado.items[i].value;
    resultado_string[i] = '\0';
    printf("[%s] len=%zu size=%d\n", resultado_string, strlen(resultado_string), resultado.size);
    free(resultado.items);
    return fallidas > 0;
}
