#include "ej1.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>


#define MAX_RESULT_LEN 1048576  // 1 MB máx
/**
*	crea y destruye a una lista vacía
*/
void test_create_destroy_list(){
	string_proc_list * list	= string_proc_list_create_asm();
	string_proc_list_destroy(list);
}

/**
*	crea y destruye un nodo
*/
void test_create_destroy_node(){
	string_proc_node* node	= string_proc_node_create_asm(0, "hash");
	string_proc_node_destroy(node);
}

/**
 * 	crea una lista y le agrega nodos
*/
void test_create_list_add_nodes()
{	
	string_proc_list * list	= string_proc_list_create_asm();
	string_proc_list_add_node_asm(list, 0, "hola");
	string_proc_list_add_node_asm(list, 0, "a");
	string_proc_list_add_node_asm(list, 0, "todos!");
	string_proc_list_destroy(list);
}

/**
 * 	crea una lista y le agrega nodos. Luego aplica la lista a un hash.
*/

void test_list_concat()
{
	string_proc_list * list	= string_proc_list_create();
	string_proc_list_add_node(list, 0, "hola");
	string_proc_list_add_node(list, 0, "a");
	string_proc_list_add_node(list, 0, "todos!");	
	char* new_hash = string_proc_list_concat(list, 0, "hash");
	string_proc_list_destroy(list);
	free(new_hash);
}


/** TESTS **/
void test_node_create_null_hash() {
	string_proc_node* node = string_proc_node_create_asm(1, NULL);
	assert(node == NULL);  // No debe crearse el nodo
}
void test_add_node_null_inputs() {
	string_proc_list_add_node_asm(NULL, 0, "hola");  // No debe romper
	string_proc_list * list = string_proc_list_create_asm();
	string_proc_list_add_node_asm(list, 0, NULL);    // No debe romper
	string_proc_list_destroy(list);
}
void test_cycle_detection() {
	string_proc_list * list = string_proc_list_create();

	string_proc_list_add_node(list, 0, "a");
	string_proc_list_add_node(list, 0, "b");
	string_proc_list_add_node(list, 0, "c");

	// Forzar un ciclo: apuntar el último al primero
	list->last->next = list->first;

	char* result = string_proc_list_concat(list, 0, "hash");
	assert(result == NULL);  // debe detectar el ciclo y devolver NULL

	// Como se hizo un ciclo, no se puede liberar correctamente
}
void test_concat_overflow() {
	string_proc_list* list = string_proc_list_create();

	char* big_string = malloc(MAX_RESULT_LEN);
	memset(big_string, 'A', MAX_RESULT_LEN - 1);
	big_string[MAX_RESULT_LEN - 1] = '\0';

	string_proc_list_add_node(list, 0, big_string);

	char* result = string_proc_list_concat(list, 0, "B");  // hash inicial con 1 char

	assert(result == NULL);  // debe detectar overflow

	free(big_string);
	string_proc_list_destroy(list);
}
void test_concat_correctness() {
	string_proc_list * list = string_proc_list_create();

	string_proc_list_add_node(list, 1, "abc");
	string_proc_list_add_node(list, 1, "def");
	string_proc_list_add_node(list, 2, "ghi"); // este se ignora

	char* result = string_proc_list_concat(list, 1, "hash-");

	assert(strcmp(result, "hash-abcdef") == 0);  // validación exacta
	free(result);
	string_proc_list_destroy(list);
}




/**
* Corre los test a se escritos por lxs alumnxs	
*/
void run_tests(){

	/* Aqui pueden comenzar a probar su codigo */
	test_create_destroy_list();

	test_create_destroy_node();

	test_create_list_add_nodes();

	test_list_concat();

	test_node_create_null_hash();
	test_add_node_null_inputs();
	test_cycle_detection();
	test_concat_overflow();
	test_concat_correctness();

}

int main (void){
	run_tests();
	return 0;    
}

