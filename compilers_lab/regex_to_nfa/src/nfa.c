/**
 * nfa.c
 *
 * Construccion de un NFA a partir de una regex en notacion postfija (algoritmo de Thompson) y simulacion de ese NFA sobre una cadena
 * de entrada para decidir si pertenece al lenguaje que se describe
 */

#include "nfa.h"
#include <stdlib.h>
#include <string.h>

/* 
 * Fragmento temporal construido por Thompson, un pedazo de NFA con un unico estado de entrada y un unico estado
 * de salida, antes de combinarse con otros fragmentos.
 */
typedef struct
{
    int start;
    int accept;
} fragment;

/**
 * Crea un nuevo estado en el NFA.
 *
 * Los estados se numeran de forma consecutiva empezando en 0, en el orden en que se van creando durante la construccion de Thompson.
 *
 * @param n el NFA al que se le agrega el estado y se modifica
 * @return el numero (id) del estado recien creado
 */
static int create_state(nfa *n)
{
    int state = n->state_count;
    n->state_count++;
    return state;
}

/**
 * Agrega una transicion al NFA.
 *
 * Hace crecer el arreglo de transiciones en uno, ya que de antemano no se sabe cuantas transiciones va a tener el
 * NFA completo.
 *
 * @param n el NFA al que se le agrega la transicion (se modifica)
 * @param from estado de origen
 * @param to estado de destino
 * @param symbol simbolo que se consume al cruzar la transicion o EPSILON si es una transicion vacia
 */
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

/**
 * Construye un NFA a partir de una regex en notacion postfija,
 * usando el algoritmo de Thompson.
 * Recorre la regex de izquierda a derecha manteniendo una pila de fragmentos. Cada literal empuja un fragmento nuevo de dos estados,
 * cada operador ., |, *, +, ? saca de la pila los fragmentos que necesita, los combina agregando estados y transiciones epsilon
 * segun corresponda, y empuja el fragmento resultante. Al terminar,
 * debe quedar un unico fragmento en la pila el cual es el NFA completo.
 *
 * @param r la regex en notacion postfija con concatenacion explicita
 * @return el NFA que acepta el mismo lenguaje que la regex
 */
nfa regex_to_nfa(regex r)
{
    /* Preparamos un NFA vacio */
    nfa n;
    n.start = -1;
    n.accept = -1;
    n.transitions = NULL;
    n.transition_count = 0;
    n.state_count = 0;

    /* Pila de fragmentos utilizada por el algoritmo de Thompson */
    fragment *stack = malloc(sizeof(fragment) * r.size);
    int top = 0;

    /* Recorremos la expresion en notacion postfija */
    for (int i = 0; i < r.size; i++)
    {
        char symbol = r.items[i].value;

        /* Concatenacion, une el final de left con el inicio de right a traves de una epsilon */
        if (symbol == '.')
        {
            fragment right = stack[--top];
            fragment left = stack[--top];

            add_transition(&n, left.accept, right.start, EPSILON);

            fragment result = {left.start, right.accept};
            stack[top++] = result;
        }

        /* Union, un nuevo inicio y nuevo final, con epsilon hacia ambas ramas y desde ambas ramas */
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

        /* Cerradura de Kleene, cero o mas repeticiones. Permite saltarse el fragmento cero veces o repetirlo un ciclo de
           epsilon de vuelta al inicio del fragmento */
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

        /* Una o mas repeticiones, igual que *, pero sin la opcion de saltarse el fragmento por completo */
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

        /* Cero o una aparicion, permite saltarse el fragmento, pero no repetirlo */
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

        /* Cualquier otro simbolo se considera un literal, un fragmento de dos estados con una sola transicion */
        else
        {
            int start = create_state(&n);
            int accept = create_state(&n);

            add_transition(&n, start, accept, symbol);

            fragment result = {start, accept};
            stack[top++] = result;
        }
    }

    /* Al terminar debe quedar un unico fragmento, ese es el NFA completo */
    if (top == 1)
    {
        fragment result = stack[0];
        n.start = result.start;
        n.accept = result.accept;
    }

    free(stack);

    return n;
}

/**
 * Encuentra todos los estados a los que se puede llegar con epsilon sin leer ninguna letra desde los estados que ya estan activos,
 * y los va agregando al mismo arreglo active.
 *
 * @param n el NFA sobre el que se busca
 * @param active arreglo de estados activos, se modifica agregando los nuevos estados que se van encontrando
*/
static void epsilon_closure(nfa n, char active[])
{
    int *pending = malloc(sizeof(int) * n.state_count);
    int top = 0;

    /* agregamos a la pila con los estados que ya venian activos */
    for (int s = 0; s < n.state_count; s++)
        if (active[s])
            pending[top++] = s;

    while (top > 0)
    {
        int s = pending[--top];

        /* revisamos todas las transiciones epsilon que salen de s */
        for (int i = 0; i < n.transition_count; i++)
        {
            if (n.transitions[i].from != s || n.transitions[i].symbol != EPSILON)
                continue;

            int t = n.transitions[i].to;

            /* solo lo agregamos si es la primera vez que lo vemos, esto evita ciclos infinitos cuando hay repeticiones de epsilon
               como los de el operador * */
            if (!active[t])
            {
                active[t] = 1;
                pending[top++] = t;
            }
        }
    }

    free(pending);
}

/**
 * Simula el NFA sobre una cadena de entrada.
 * Va leyendo la cadena letra por letra y llevando la cuenta de en que estados podria estar el NFA en cada momento. Si en algun momento
 * ya no queda ningun estado posible, se corta y se rechaza de una vez.
 *
 * @param n el NFA a usar
 * @param str la cadena a revisar
 * @param len cuantas letras tiene str
 * @return 1 si la acepta, 0 si no
*/
int match_nfa(nfa n, const char str[], size_t len)
{
    if (n.state_count == 0 || n.start < 0 || n.accept < 0)
        return 0;

    char *active = calloc(n.state_count, sizeof(char));
    char *next = calloc(n.state_count, sizeof(char));

    /* S = epsilon-closure({s0}) */
    active[n.start] = 1;
    epsilon_closure(n, active);

    for (size_t pos = 0; pos < len; pos++)
    {
        char c = str[pos];
        memset(next, 0, n.state_count);

        /* move(S, c): a donde se llega desde los estados activos
           consumiendo exactamente el simbolo c */
        for (int s = 0; s < n.state_count; s++)
        {
            if (!active[s])
                continue;
            for (int i = 0; i < n.transition_count; i++)
                if (n.transitions[i].from == s && n.transitions[i].symbol == c)
                    next[n.transitions[i].to] = 1;
        }

        /* S = epsilon-closure(move(S, c)) */
        epsilon_closure(n, next);
        memcpy(active, next, n.state_count);

        /* rechazo si nadie sobrevivio, ninguna continuacion
           de la cadena puede llevar a aceptar */
        int any_active = 0;
        for (int s = 0; s < n.state_count; s++)
            if (active[s]) { any_active = 1; break; }
        if (!any_active)
        {
            free(active);
            free(next);
            return 0;
        }
    }

    /* aceptamos si el estado de aceptacion sigue activo al terminar */
    int accepted = active[n.accept];
    free(active);
    free(next);
    return accepted;
}

/**
 * Libera la memoria dinamica reservada por el NFA (el arreglo de
 * transiciones) y deja la estructura en un estado limpio, por si se
 * vuelve a usar por accidente despues de liberada.
 *
 * @param n el NFA a liberar
*/
void free_nfa(nfa *n)
{
    free(n->transitions);

    n->transitions = NULL;
    n->transition_count = 0;
    n->state_count = 0;
    n->start = -1;
    n->accept = -1;
}