#include "regex.h"
#include "tokenizer.h"
#include "pila.h"
#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>

/*
* Convierte una expresion de regular a notacion posfija usando
* el algoritmo Shunting-Yard.
*/

char operator[] = "|*+.()?";

/**
 * Aplica el algoitmo de Shunting-Yard, la jerarquia de operadores es la que sigue en orden de mayor a menor
 * [|, ., * ]. Para esta parte, definimos una pila para guardar los operadores y quitamos de la misma en tres
 * situaciones
 * 1. Sea <value> el simbolo actual, si es mayor o igual quitamos de la pila hasta que este vacia, nos encontremos
 * un parentesis abierto o ya encontramos en la pila uno de jerarquia mayor, los simbolos extraidos se mandan a la 
 * salida en orden LIFO y el simbolo actual se almacena en la pila.
 * 2. Los que no son operadores se mandan directamente a la salida.
 * La implementacion hace uso de una pila de expresiones regulares definida nativamente en el archivo de cabecera <pila.h>,
 * con las funciones definidas en <pila.c> respetando las complejidades, comportamientos y operacioones que se pueden realizzar
 * en una pila.
 * La implementacion consiste en 3 casos generales; el primero es verificar que el simbolo no sea un operador, si resulta que es operador
 * como en la pila solo se almacenan operadores, verificamos si esta vacia, si no esta aplicamos Shunting-Yard.
 * Nota: para vaciar iterativamente la pila es necesario usar free() el cual consiste en liberar el espacio en memoria dinamica asignada
 * de tal forma que nos aseguramos de eliminar un elemento de forma segura.
 * @param <str> recibe un apuntador de la cadena antes de tokenizar en concatenacion explicita
 * @return <r> la expresion regular en notacion posfija con concatenacion explicita. 
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
	// Define una pila y un indice que se aumenta cada vez que se agrega un simbolo en la salida
	regex_stack *stack_r = start_stack();
	int index = 0;

	/*Nos movemos por el regex*/
    for (int i = 0; i < tokens.size; i++){
	  char value = tokens.items[i].value;
	  regex_item *item_value;
	  item_value = (regex_item*)malloc(sizeof (regex_item));
	  item_value->value = value;
	  item_value->next = NULL;
	  
	  /* Si no es un operador entonces mandalo a la salida */
	  if (!is_operator(&value)){
		r.items[index].value = value;
		index++;
		
	  }else if (!empty(stack_r)) {
		// auxiliar para mirar el tope de la pila sin extraerlo
		regex_item* look_item_inside = look(stack_r);

		/* Shunting-Yard casos*/

		// 1. Si el simbolo actual es un parentesis abierto solo agregalo
		if (value == '('){
		  push(stack_r, item_value);

		  
		}// 2. Si encontraste un cierre de parentesis, muestra todo en la salida vaciando la pila 
		else if (value == ')'){
		  
		  while (!empty(stack_r)) {
			regex_item* item_r_last = pop(stack_r);
			if (item_r_last->value == '('){
			  free(item_r_last);
			  break;
			}
			
			r.items[index].value = item_r_last->value;
			index++;
		  }
		  
		  
		}//3. Conflicto de jerarquia
		else if (has_higher_precedence(&value, &look_item_inside->value)) {
		  while (!empty(stack_r)){
		  	regex_item* last_r_item = pop(stack_r);
		 	r.items[index].value = last_r_item->value;
		  	index++;
			free(last_r_item);
			if (empty(stack_r)) break;
		  	look_item_inside = look(stack_r);
			if (look_item_inside->value == '(') break;
		  	if (!has_higher_precedence(&value, &look_item_inside->value))
			  break;
		  	
		  	}
		  // Metemos el valor con el que se genero el conflicto una vez resuelto. 
		  push(stack_r, item_value);
		  
		}//4. Ninguna de las anteriores, agrega el operador
		else
		  push(stack_r, item_value);
		
	  }// Agrega el primer operador para iniciar el algoritmo
	  else
		push(stack_r, item_value);
	 
	}
	  
   
	
	// Vaciamos el resto de operadores de la pila
	while (!empty(stack_r)){
	  regex_item* item_r_last = pop(stack_r);
	  r.items[index].value = item_r_last->value;
	  index++;
	  free(item_r_last);
	}
	
	r.size = index;
    free_token_list(&tokens);
	delete(stack_r);
    return r;
}

/**
 * Funcion que recibe un apuntador de tipo caracter, verifica
 * si es un operador o no en el arreglo de operadores.
 * @param c el simbolo 
 * @return true o false si es un operador 
*/
bool is_operator(char* c){
  for (int i = 0; operator[i] != '\0'; i++){
	if (*c == operator[i])
	  return true;
  }
  return false;
}
/**
 * Funcion que verifica si <current_c> tiene una procedencia mas alta o igual que <top_stack_c>,
 * es decir, el simbolo actual con el tope de la pila.
 * @param <current_c> el simbolo actual
 * @param <top_stack_c> el simbolo en el tope
 * @return true o false si tiene mayor o igual jerarquia alta.
*/
bool has_higher_precedence(char* current_c, char* top_stack_c){
  if ((*current_c == '|') && ((*top_stack_c == '|' ) || (*top_stack_c == '*') || (*top_stack_c == '.') || (*top_stack_c == '?')))
	return true;
  else if ((*current_c == '?') && ((*top_stack_c == '|' ) || (*top_stack_c == '*') || (*top_stack_c == '.') || (*top_stack_c == '?')))
	return true;
  else if ((*current_c == '.') && ((*top_stack_c == '*') || (*top_stack_c == '.')))
	return true;
  else if ((*current_c == '*') && (*top_stack_c == '*'))
	return true;
  
  return false;
}

