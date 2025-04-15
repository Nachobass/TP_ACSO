/** Lista **/
/* 
// string_proc_list tiene dos punteros (8 bytes cada uno en x86_64), ocupando en total 16 bytes
typedef struct string_proc_list_t {
	struct string_proc_node_t* first;           // offset 0
	struct string_proc_node_t* last;            // offset 8
} string_proc_list;
*/

/** Nodo **/
/* 
// next (0), previous (8), type (16), padding (17-23), hash (24), ocupando en total 32 bytes
typedef struct string_proc_node_t {
	struct string_proc_node_t* next;
	struct string_proc_node_t* previous;
	uint8_t type;
	char* hash;
} string_proc_node;
*/
	

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ej1.h"

/* Crea una nueva lista vacía */
string_proc_list* string_proc_list_create(void){
    string_proc_list* list = malloc(sizeof(string_proc_list));
    if( list == NULL ) return NULL;
    list->first = NULL;
    list->last = NULL;
    return list;
}

/* Crea un nuevo nodo apuntando al hash pasado */
string_proc_node* string_proc_node_create(uint8_t type, char* hash){
    if( hash == NULL ) return NULL;
    string_proc_node* node = malloc(sizeof(string_proc_node));
    if( node == NULL ) return NULL;
    node->type = type;
    node->hash = hash;
    node->next = NULL;
    node->previous = NULL;
    return node;
}

/* Agrega un nodo al final de la lista */
void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash){
    if( list == NULL ) return;
    string_proc_node* node = string_proc_node_create(type, hash);
    if( node == NULL ) return;

    if( list->first == NULL ) {         // Lista vacía
        list->first = node;
        list->last = node;
    } else {                            // Lista no vacía
        list->last->next = node;
        node->previous = list->last;
        list->last = node;
    }
}

/* Concatena los hashes de los nodos que coinciden con el tipo dado */
char* string_proc_list_concat(string_proc_list* list, uint8_t type, char* hash){
    if( list == NULL ) return NULL;

    char* result = strdup(hash);  // copiamos el hash inicial
    if( result == NULL ) return NULL;

    string_proc_node* current = list->first;
    while( current != NULL ) {
        if( current->type == type && current->hash != NULL ) {
            char* new_result = str_concat(result, current->hash);
            free(result);  // liberamos la anterior
            result = new_result;
        }
        current = current->next;
    }

    return result;  // el llamador debe liberar esto
}



/** AUX FUNCTIONS **/

void string_proc_list_destroy(string_proc_list* list){

	/* borro los nodos: */
	string_proc_node* current_node	= list->first;
	string_proc_node* next_node		= NULL;
	while(current_node != NULL){
		next_node = current_node->next;
		string_proc_node_destroy(current_node);
		current_node	= next_node;
	}
	/*borro la lista:*/
	list->first = NULL;
	list->last  = NULL;
	free(list);
}
void string_proc_node_destroy(string_proc_node* node){
	node->next      = NULL;
	node->previous	= NULL;
	node->hash		= NULL;
	node->type      = 0;			
	free(node);
}


char* str_concat(char* a, char* b) {
	int len1 = strlen(a);
    int len2 = strlen(b);
	int totalLength = len1 + len2;
    char *result = (char *)malloc(totalLength + 1); 
    strcpy(result, a);
    strcat(result, b);
    return result;  
}

void string_proc_list_print(string_proc_list* list, FILE* file){
        uint32_t length = 0;
        string_proc_node* current_node  = list->first;
        while(current_node != NULL){
                length++;
                current_node = current_node->next;
        }
        fprintf( file, "List length: %d\n", length );
		current_node    = list->first;
        while(current_node != NULL){
                fprintf(file, "\tnode hash: %s | type: %d\n", current_node->hash, current_node->type);
                current_node = current_node->next;
        }
}
