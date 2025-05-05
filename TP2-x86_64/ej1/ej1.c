#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ej1.h"

#define MAX_RESULT_LEN 1048576  // 1 MB máx


/* crea una nueva lista vacía */
string_proc_list* string_proc_list_create(void){
    string_proc_list* list = malloc(sizeof(string_proc_list));
    if( list == NULL ) return NULL;
    list->first = NULL;
    list->last = NULL;
    return list;
}

/* crea un nuevo nodo apuntando al hash pasado */
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

/* agrega un nodo al final de la lista */
void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash){
    if( list == NULL || hash == NULL ) return;

    string_proc_node* node = string_proc_node_create(type, hash);
    if( node == NULL ) return;

    if( list->first == NULL ) {                     // si la lista está vacía
        list->first = node;
        list->last = node;
    } else {                                        // lista no vacía
        list->last->next = node;
        node->previous = list->last;
        list->last = node;
    }
}

/* concatena los hashes de los nodos que coinciden con el tipo dado */
char* string_proc_list_concat(string_proc_list* list, uint8_t type, char* hash) {
    if( list == NULL || hash == NULL ) return NULL;

    // detección de ciclo
    string_proc_node* slow = list->first;
    string_proc_node* fast = list->first;
    while( fast && fast->next ){
        slow = slow->next;
        fast = fast->next->next;
        if( slow == fast ) return NULL;  // ciclo detectado
    }

    size_t hash_len = strlen(hash);
    char* result = malloc(hash_len + 1);  // +1 para el '\0'
    if( result == NULL ) return NULL;
    memcpy(result, hash, hash_len + 1);   // copia incluyendo el '\0'

    if( result == NULL ) return NULL;

    size_t current_len = strlen(result);

    string_proc_node* current = list->first;
    while( current != NULL ){
        if( current->type == type && current->hash != NULL ){
            size_t add_len = strlen(current->hash);
            if( current_len + add_len > MAX_RESULT_LEN ){
                free(result);
                return NULL;  // overflow
            }

            char* new_result = str_concat(result, current->hash);
            if( new_result == NULL ){
                free(result);
                return NULL;
            }

            free(result);
            result = new_result;
            current_len += add_len;
        }
        current = current->next;
    }

    return result;
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
