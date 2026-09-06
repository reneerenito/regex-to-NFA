/* Representación de la regex que main.c consume e imprime. Cada item
   guarda un carácter de la expresión ya procesada. */
#include<stdbool.h>
#ifndef REGEX_H
#define REGEX_H

typedef struct
{
  
  char value;
  struct regex_item *next;
} regex_item;
  

typedef struct
{
    regex_item *items;
    int size;
} regex;


regex parse_regex(const char *str);
bool is_operator(char* c);
bool has_higher_precedence(char* current_c, char* top_stack_c);

#endif
