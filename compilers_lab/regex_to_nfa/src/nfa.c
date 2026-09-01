#include "nfa.h"

nfa regex_to_nfa(regex r)
{
    /* TODO construir el NFA con el algoritmo de Thompson a partir de la regex postfija */
    (void)r;
    nfa n = {0, 0};
    return n;
}

int match_nfa(nfa n, const char *str, size_t len)
{
    /* TODO simular el NFA sobre la cadena de entrada */
    (void)n;
    (void)str;
    (void)len;
    return 0;
}
