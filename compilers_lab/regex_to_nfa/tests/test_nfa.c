#include "nfa.h"
#include "regex.h"
#include <stdio.h>
#include <stdlib.h>

/* Contadores para las pruebas */
static int total = 0;
static int fallidas = 0;

/* Verifica una condición y muestra el resultado */
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

/* Busca una transición específica dentro del NFA */
static int has_transition(nfa n, int from, int to, char symbol)
{
    for (int i = 0; i < n.transition_count; i++)
    {
        if (n.transitions[i].from == from &&
            n.transitions[i].to == to &&
            n.transitions[i].symbol == symbol)
        {
            return 1;
        }
    }

    return 0;
}

/* Prueba de un símbolo literal */
static void test_literal(void)
{
    regex r = parse_regex("a");
    nfa n = regex_to_nfa(r);

    check(n.state_count == 2,
          "\"a\" genera 2 estados");

    check(n.transition_count == 1,
          "\"a\" genera 1 transicion");

    check(n.start == 0,
          "\"a\" tiene estado inicial 0");

    check(n.accept == 1,
          "\"a\" tiene estado final 1");

    check(has_transition(n, 0, 1, 'a'),
          "\"a\" contiene 0 --a--> 1");

    free_nfa(&n);
    free(r.items);
}

/* Prueba de concatenación */
static void test_concatenation(void)
{
    regex r = parse_regex("ab");
    nfa n = regex_to_nfa(r);

    check(n.state_count == 4,
          "\"ab\" genera 4 estados");

    check(n.transition_count == 3,
          "\"ab\" genera 3 transiciones");

    check(n.start == 0,
          "\"ab\" inicia en 0");

    check(n.accept == 3,
          "\"ab\" termina en 3");

    check(has_transition(n, 0, 1, 'a'),
          "\"ab\" contiene transicion para a");

    check(has_transition(n, 1, 2, EPSILON),
          "\"ab\" conecta ambos fragmentos con epsilon");

    check(has_transition(n, 2, 3, 'b'),
          "\"ab\" contiene transicion para b");

    free_nfa(&n);
    free(r.items);
}

/* Prueba de unión */
static void test_union(void)
{
    regex r = parse_regex("a|b");
    nfa n = regex_to_nfa(r);

    check(n.state_count == 6,
          "\"a|b\" genera 6 estados");

    check(n.transition_count == 6,
          "\"a|b\" genera 6 transiciones");

    check(n.start == 4,
          "\"a|b\" tiene nuevo estado inicial");

    check(n.accept == 5,
          "\"a|b\" tiene nuevo estado final");

    check(has_transition(n, 0, 1, 'a'),
          "union conserva la transicion para a");

    check(has_transition(n, 2, 3, 'b'),
          "union conserva la transicion para b");

    check(has_transition(n, 4, 0, EPSILON),
          "union conecta inicio con a");

    check(has_transition(n, 4, 2, EPSILON),
          "union conecta inicio con b");

    check(has_transition(n, 1, 5, EPSILON),
          "union conecta a con el final");

    check(has_transition(n, 3, 5, EPSILON),
          "union conecta b con el final");

    free_nfa(&n);
    free(r.items);
}

/* Prueba de cerradura de Kleene */
static void test_kleene(void)
{
    regex r = parse_regex("a*");
    nfa n = regex_to_nfa(r);

    check(n.state_count == 4,
          "\"a*\" genera 4 estados");

    check(n.transition_count == 5,
          "\"a*\" genera 5 transiciones");

    check(n.start == 2,
          "\"a*\" tiene nuevo inicio");

    check(n.accept == 3,
          "\"a*\" tiene nuevo final");

    check(has_transition(n, 0, 1, 'a'),
          "kleene conserva la transicion para a");

    check(has_transition(n, 2, 0, EPSILON),
          "kleene entra al fragmento");

    check(has_transition(n, 2, 3, EPSILON),
          "kleene permite cero repeticiones");

    check(has_transition(n, 1, 0, EPSILON),
          "kleene permite repetir");

    check(has_transition(n, 1, 3, EPSILON),
          "kleene permite terminar");

    free_nfa(&n);
    free(r.items);
}

/* Prueba del operador +: una o más repeticiones */
static void test_plus(void)
{
    regex r = parse_regex("a+");
    nfa n = regex_to_nfa(r);

    check(n.state_count == 4,
          "\"a+\" genera 4 estados");

    check(n.transition_count == 4,
          "\"a+\" genera 4 transiciones");

    check(n.start == 2,
          "\"a+\" tiene nuevo inicio");

    check(n.accept == 3,
          "\"a+\" tiene nuevo final");

    check(has_transition(n, 0, 1, 'a'),
          "plus conserva la transicion para a");

    check(has_transition(n, 2, 0, EPSILON),
          "plus obliga a entrar al fragmento");

    check(has_transition(n, 1, 0, EPSILON),
          "plus permite repetir");

    check(has_transition(n, 1, 3, EPSILON),
          "plus permite terminar");

    /* A diferencia de *, + no puede saltarse el fragmento */
    check(!has_transition(n, 2, 3, EPSILON),
          "plus no permite cero repeticiones");

    free_nfa(&n);
    free(r.items);
}

/* Prueba del operador ?: cero o una aparición */
static void test_optional(void)
{
    regex r = parse_regex("a?");
    nfa n = regex_to_nfa(r);

    check(n.state_count == 4,
          "\"a?\" genera 4 estados");

    check(n.transition_count == 4,
          "\"a?\" genera 4 transiciones");

    check(n.start == 2,
          "\"a?\" tiene nuevo inicio");

    check(n.accept == 3,
          "\"a?\" tiene nuevo final");

    check(has_transition(n, 0, 1, 'a'),
          "optional conserva la transicion para a");

    check(has_transition(n, 2, 0, EPSILON),
          "optional permite entrar al fragmento");

    check(has_transition(n, 2, 3, EPSILON),
          "optional permite omitir el fragmento");

    check(has_transition(n, 1, 3, EPSILON),
          "optional permite terminar");

    /* ? no permite repetir */
    check(!has_transition(n, 1, 0, EPSILON),
          "optional no permite repetir");

    free_nfa(&n);
    free(r.items);
}

/* Prueba de una expresión compuesta */
static void test_compuesta(void)
{
    regex r = parse_regex("a(b|c)*");
    nfa n = regex_to_nfa(r);

    /*
     * a(b|c)*
     *
     * en:
     *
     * abc|*.
     *
     * Thompson debe construir un único NFA.
     */

    check(n.start >= 0,
          "\"a(b|c)*\" tiene estado inicial");

    check(n.accept >= 0,
          "\"a(b|c)*\" tiene estado final");

    check(n.start != n.accept,
          "\"a(b|c)*\" tiene inicio y final diferentes");

    check(n.state_count == 10,
          "\"a(b|c)*\" genera 10 estados");

    check(n.transition_count == 12,
          "\"a(b|c)*\" genera 12 transiciones");

    /* El primer fragmento corresponde al literal a */
    check(has_transition(n, 0, 1, 'a'),
          "expresion compuesta contiene a");

    /* Los siguientes fragmentos corresponden a b y c */
    check(has_transition(n, 2, 3, 'b'),
          "expresion compuesta contiene b");

    check(has_transition(n, 4, 5, 'c'),
          "expresion compuesta contiene c");

    /* Debe existir al menos una transición epsilon */
    int has_epsilon = 0;

    for (int i = 0; i < n.transition_count; i++)
    {
        if (n.transitions[i].symbol == EPSILON)
        {
            has_epsilon = 1;
            break;
        }
    }

    check(has_epsilon,
          "expresion compuesta contiene transiciones epsilon");

    free_nfa(&n);
    free(r.items);
}

int main(void)
{
    printf("Pruebas de construccion Thompson\n\n");

    test_literal();
    test_concatenation();
    test_union();
    test_kleene();
    test_plus();
    test_optional();
    test_compuesta();

    printf("\n%d pruebas, %d fallidas\n", total, fallidas);

    return fallidas > 0;
}