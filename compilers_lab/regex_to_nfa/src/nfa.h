/* Estructura del NFA. */

#ifndef NFA_H
#define NFA_H

#include "regex.h"
#include <stddef.h>

/* Usamos '\0' para representar una transición epsilon */
#define EPSILON '\0'

/* Representa una transición del autómata:
   desde un estado, hacia otro, consumiendo un símbolo o epsilon */
typedef struct
{
    int from;
    int to;
    char symbol;
} transition;

/* Representa el NFA completo */
typedef struct
{
    int start;
    int accept;

    transition *transitions;
    int transition_count;

    int state_count;
} nfa;

/* Construye un NFA a partir de una regex en notación postfija */
nfa regex_to_nfa(regex r);

/* Simula el NFA sobre una cadena */
int match_nfa(nfa n, const char *str, size_t len);

/* Libera la memoria dinámica usada por el NFA */
void free_nfa(nfa *n);

#endif