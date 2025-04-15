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
// string_proc_node* string_proc_node_create(uint8_t type, char* hash){
//     string_proc_node* node = malloc(sizeof(string_proc_node));
//     if( node == NULL ) return NULL;
//     node->type = type;
//     // node->hash = hash;
//     node->hash = strdup(hash);
//     if (node->hash == NULL) {
//         free(node);
//         return NULL;
//     }
//     node->next = NULL;
//     node->previous = NULL;
//     return node;
// }
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
    if( list == NULL || hash == NULL ) return;

    string_proc_node* node = string_proc_node_create(type, hash);
    if( node == NULL ) return;

    if( list->first == NULL ) {         // Lista vacía
        list->first = node;
        list->last = node;
        return;
    } elif( list->last == NULL ){                            // Lista no vacía
        free(node);
        return;
    } else {                            // Lista no vacía
        list->last->next = node;
        node->previous = list->last;
        list->last = node;
    }
}


/* Concatena los hashes de los nodos que coinciden con el tipo dado */
// char* string_proc_list_concat(string_proc_list* list, uint8_t type, char* hash) {
//     if (list == NULL || hash == NULL) return NULL;

//     // Copiamos el hash inicial (no lo modificamos)
//     char* result = strdup(hash);
//     if (result == NULL) return NULL;

//     string_proc_node* current = list->first;

//     // Prevención de ciclos: lista de nodos visitados (hasta 100)
//     string_proc_node* visited[100] = {NULL};
//     size_t visit_count = 0;

//     while( current != NULL ) {
//         // Chequeo de ciclos
//         for( size_t i = 0; i < visit_count; i++ ){
//             if( current == visited[i] ){
//                 free(result);
//                 return NULL;
//             }
//         }
//         if( visit_count < 100 ) visited[visit_count++] = current;

//         // Concatenamos si el tipo coincide
//         if (current->type == type && current->hash != NULL) {
//             char* new_result = str_concat(result, current->hash);
//             if (new_result == NULL) {
//                 free(result);
//                 return NULL;
//             }
//             free(result);
//             result = new_result;
//         }

//         current = current->next;
//     }

//     return result;  // El llamador debe liberar
// }

#define MAX_RESULT_LEN 1048576  // 1 MB máx, ajustable

char* string_proc_list_concat(string_proc_list* list, uint8_t type, char* hash) {
    if (list == NULL || hash == NULL) return NULL;

    // Detección de ciclo con algoritmo tortuga y liebre
    string_proc_node* slow = list->first;
    string_proc_node* fast = list->first;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            // Ciclo detectado
            return NULL;
        }
    }

    // Primero, calcular longitud total para evitar múltiples realloc/str_concat
    size_t total_len = strlen(hash);
    string_proc_node* current = list->first;
    while (current != NULL) {
        if (current->type == type && current->hash != NULL) {
            total_len += strlen(current->hash);
            if (total_len > MAX_RESULT_LEN) return NULL;  // Prevención de overflow
        }
        current = current->next;
    }

    // Asignar una sola vez
    char* result = malloc(total_len + 1);
    if (result == NULL) return NULL;

    // Copiar hash inicial
    strcpy(result, hash);

    // Concatenar el resto
    current = list->first;
    while (current != NULL) {
        if (current->type == type && current->hash != NULL) {
            strcat(result, current->hash);
        }
        current = current->next;
    }

    return result;  // El llamador debe liberar esto
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

