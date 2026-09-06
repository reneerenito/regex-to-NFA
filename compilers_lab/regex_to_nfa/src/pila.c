#include "pila.h"
#include<stdlib.h>
#include<stdio.h>

/*
 * Implementa una pila de expresiones regulares "regex" con sus 
 * operaciones basicas. 
**/

/**
 * Crea e inicializa una pila vacia
 *
 * @return puntero a la nueva pila reservada en el heap
 */
regex_stack* start_stack(){
  
  regex_stack *stack_r;
  stack_r = (regex_stack*) malloc(sizeof (regex_stack));
  
  stack_r->head = NULL;
  stack_r->size = 0;
  return stack_r;
}


/**
 * Inserta un item en el tope de la pila
 *
 * @param stack_r puntero a la pila destino
 * @param item_value puntero al item a insertar, si es NULL no hace nada
 */
void push(regex_stack *stack_r, regex_item *item_value){
  
  
  if (item_value == NULL)
	return;
  
  if (empty(stack_r)){
	stack_r->head = item_value;
	stack_r->head->next = NULL;
	
  } else {
	item_value->next = (struct regex_item*)stack_r->head;
	stack_r->head = item_value;
  }
  
  stack_r->size++; 
}


/**
 * Saca y devuelve el item en el tope de la pila
 *
 * Transfiere la propiedad del nodo devuelto a quien llama la funcion,
 * por lo que el llamador es responsable de liberarlo con free()
 *
 * @param stack_r puntero a la pila de la cual sacar el item
 * @return puntero al item que estaba en la cima
 */
regex_item* pop(regex_stack *stack_r){
  
  if (empty(stack_r)){
	printf("Error: La pila ya se encuentra vacia");
	exit(1);
  }
  
  regex_item *item_r_last;
  item_r_last = stack_r->head;
  stack_r->head = (regex_item*) stack_r->head->next;
  item_r_last->next = NULL;

  stack_r->size--;
  return item_r_last;
}

/**
 * Devuelve el item en el tope de la pila sin sacarlo
 *
 * El puntero devuelto sigue siendo propiedad de la pila,
 * no debe liberarse con free()
 *
 * @param stack_r puntero a la pila a consultar
 * @return puntero al item en la cima
 */
regex_item* look(regex_stack *stack_r){
  
  if (empty(stack_r)){
	printf("Error: La pila ya se encuentra vacia");
	exit(1);
  }
  return stack_r->head;
  
}


/**
 * Indica si la pila no tiene elementos
 *
 * @param stack_r puntero a la pila a consultar
 * @return true si la pila esta vacia, false en caso contrario
 */
bool empty(regex_stack *stack_r){
  
  if (stack_r->size == 0)
	return true;
  
  return false;
}


/**
 * Libera todos los items de la pila y la pila misma
 *
 * @param stack_r puntero a la pila a destruir, no debe usarse despues de esta llamada
 */
void delete(regex_stack *stack_r){
  
  while (!empty(stack_r)){
	pop(stack_r);
  }
  
  free(stack_r->head);
  free(stack_r);
  
}


/**
 * Imprime todos los valores de la pila de la cima hacia el fondo
 *
 * No modifica la pila, solo la recorre para diagnostico
 *
 * @param stack_r puntero a la pila a imprimir
 */
void print_stack(regex_stack* stack_r){
  
  regex_item* item_r;
  int size_aux = stack_r->size;
  
  printf("exit --\n");

  item_r = stack_r->head;
  while (size_aux != 0){
	printf("|%c|\n", item_r->value);
	item_r = (regex_item*)item_r->next;
	size_aux--;
  }
  
}
