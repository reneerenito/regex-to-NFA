#include "nfa.h"
#include <stdlib.h>

/* Representa un fragmento temporal construido por Thompson */
typedef struct
{
    int start;
    int accept;
} fragment;

/* Crea un nuevo estado y devuelve su número */
static int create_state(nfa *n)
{
    int state = n->state_count;
    n->state_count++;
    return state;
}

/* Agrega una transición entre dos estados */
static void add_transition(nfa *n, int from, int to, char symbol)
{
    n->transitions = realloc(
        n->transitions,
        sizeof(transition) * (n->transition_count + 1)
    );

    n->transitions[n->transition_count].from = from;
    n->transitions[n->transition_count].to = to;
    n->transitions[n->transition_count].symbol = symbol;

    n->transition_count++;
}

nfa regex_to_nfa(regex r)
{
    /* Inicializamos un NFA vacío */
    nfa n;
    n.start = -1;
    n.accept = -1;
    n.transitions = NULL;
    n.transition_count = 0;
    n.state_count = 0;

    /* Pila de fragmentos utilizada por el algoritmo de Thompson */
    fragment *stack = malloc(sizeof(fragment) * r.size);
    int top = 0;

    /* Recorremos la expresión en notación postfija */
    for (int i = 0; i < r.size; i++)
    {
        char symbol = r.items[i].value;

        /* Concatenación */
        if (symbol == '.')
        {
            fragment right = stack[--top];
            fragment left = stack[--top];

            add_transition(&n, left.accept, right.start, EPSILON);

            fragment result = {left.start, right.accept};
            stack[top++] = result;
        }

        /* Unión */
        else if (symbol == '|')
        {
            fragment right = stack[--top];
            fragment left = stack[--top];

            int start = create_state(&n);
            int accept = create_state(&n);

            add_transition(&n, start, left.start, EPSILON);
            add_transition(&n, start, right.start, EPSILON);

            add_transition(&n, left.accept, accept, EPSILON);
            add_transition(&n, right.accept, accept, EPSILON);

            fragment result = {start, accept};
            stack[top++] = result;
        }

        /* Cerradura de Kleene: cero o más repeticiones */
        else if (symbol == '*')
        {
            fragment f = stack[--top];

            int start = create_state(&n);
            int accept = create_state(&n);

            add_transition(&n, start, f.start, EPSILON);
            add_transition(&n, start, accept, EPSILON);

            add_transition(&n, f.accept, f.start, EPSILON);
            add_transition(&n, f.accept, accept, EPSILON);

            fragment result = {start, accept};
            stack[top++] = result;
        }

        /* Una o más repeticiones */
        else if (symbol == '+')
        {
            fragment f = stack[--top];

            int start = create_state(&n);
            int accept = create_state(&n);

            add_transition(&n, start, f.start, EPSILON);

            add_transition(&n, f.accept, f.start, EPSILON);
            add_transition(&n, f.accept, accept, EPSILON);

            fragment result = {start, accept};
            stack[top++] = result;
        }

        /* Cero o una aparición */
        else if (symbol == '?')
        {
            fragment f = stack[--top];

            int start = create_state(&n);
            int accept = create_state(&n);

            add_transition(&n, start, f.start, EPSILON);
            add_transition(&n, start, accept, EPSILON);

            add_transition(&n, f.accept, accept, EPSILON);

            fragment result = {start, accept};
            stack[top++] = result;
        }

        /* Cualquier otro símbolo se considera un literal */
        else
        {
            int start = create_state(&n);
            int accept = create_state(&n);

            add_transition(&n, start, accept, symbol);

            fragment result = {start, accept};
            stack[top++] = result;
        }
    }

    /* Al terminar debe quedar un único fragmento */
    if (top == 1)
    {
        fragment result = stack[0];
        n.start = result.start;
        n.accept = result.accept;
    }

    free(stack);

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

void free_nfa(nfa *n)
{
    free(n->transitions);

    n->transitions = NULL;
    n->transition_count = 0;
    n->state_count = 0;
    n->start = -1;
    n->accept = -1;
}