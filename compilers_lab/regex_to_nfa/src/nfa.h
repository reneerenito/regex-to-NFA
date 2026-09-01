/* Estructura provisional del NFA. Quien implemente esta parte puede
   cambiarla por completo según lo necesite la construcción de Thompson. */

#ifndef NFA_H
#define NFA_H

#include "regex.h"
#include <stddef.h>

typedef struct
{
    int start;
    int accept;
} nfa;

nfa regex_to_nfa(regex r);
int match_nfa(nfa n, const char *str, size_t len);

#endif
