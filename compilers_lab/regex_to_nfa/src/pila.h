#include "regex.h"
#include <stdbool.h>


#ifndef PILA_H
#define PILA_H

typedef struct
{
  regex_item *head;
  int size;
  
} regex_stack;


regex_stack* start_stack();
void push(regex_stack *stack, regex_item *item_value);
regex_item* pop(regex_stack *stack);
regex_item* look(regex_stack *stack);
void delete(regex_stack *stack);
bool empty(regex_stack* stack);
void print_stack(regex_stack* stack);

#endif
