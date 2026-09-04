#include "regex.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>

/*
* Convierte una expresion de regular a notacion posfija usando
* el algoritmo Shunting-Yard.
*/

regex parse_regex(const char *str)
{
    token_list tokens;
    tokenize_status status = tokenize(str, &tokens);
    if (status != TOKENIZE_OK)
    {
        fprintf(stderr, "Error: %s\n", tokenize_error_message(status));
        exit(1);
    }

    /* TODO aplicar Shunting Yard sobre los tokens para obtener el postfijo.
       Por ahora se regresan tal cual salen del tokenizador, en orden infijo
       y con la concatenación ya explícita, para que la siguiente etapa los
       consuma directamente. */
    regex r;
    r.size = tokens.size;
    r.items = malloc(sizeof(regex_item) * tokens.size);
    for (int i = 0; i < tokens.size; i++)
        r.items[i].value = tokens.items[i].value;

    free_token_list(&tokens);
    return r;
}
