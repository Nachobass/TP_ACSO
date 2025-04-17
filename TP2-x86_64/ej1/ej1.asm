; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat
extern strlen
extern memcpy
extern strcpy





string_proc_list_create_asm:
    push rbp
    mov rbp, rsp
    xor rax, rax

    mov rdi, 16                     ; tamaño de la lista
    call malloc                     ; malloc(16) retorna rax que es la dirección
    test rax, rax                   ; si malloc falla, rax = NULL
    je .return_null

    mov qword [rax], NULL           ; first, qword es 8 bytes
    mov qword [rax + 8], NULL       ; last

    pop rbp
    ret

.return_null:
    xor rax, rax
    pop rbp
    ret




string_proc_node_create_asm:
    push rbp
    mov rbp, rsp
    push rbx                    ; rbx para guardar direcciones temporales
    xor rax, rax

    cmp rsi, 0                  ; si hash == NULL
    je .return_null

    ; guardar argumentos
    mov rdx, rsi                ; hash → rdx
    movzx rcx, dil              ; type → rcx    guardo la parte baja de rdi en rcx y lleno con ceros el resto

    mov rdi, 32
    call malloc
    test rax, rax
    je .return_null

    mov rbx, rax                ; guardo el puntero al nodo en rbx

    mov qword [rbx + 0], 0      ; next
    mov qword [rbx + 8], 0      ; previous
    mov byte  [rbx + 16], cl    ; type, cl es la parte baja de rcx
    mov qword [rbx + 24], rdx   ; hash

    mov rax, rbx                ; devolver el puntero al nodo

    pop rbx
    pop rbp
    ret

.return_null:
    xor rax, rax
    pop rbx
    pop rbp
    ret




string_proc_list_add_node_asm:
    ; rdi = list, sil = type, rdx = hash
    push rbp
    mov rbp, rsp

    ; Verificar si list == NULL || hash == NULL
    test rdi, rdi
    je .return
    test rdx, rdx
    je .return

    ; Guardar registros usados
    mov r8, rdi       ; r8 = list
    movzx r9d, sil    ; r9d = (uint8_t)type

    ; Crear nodo: string_proc_node_create(type, hash)
    movzx edi, sil    ; 1er parámetro: type → edi
    mov rsi, rdx      ; 2do parámetro: hash → rsi
    call string_proc_node_create_asm
    test rax, rax
    je .return        ; si node == NULL, salgo

    ; rax = node
    mov r10, rax      ; r10 = node

    ; if (list->first == NULL)
    mov r11, [r8]     ; r11 = list->first
    test r11, r11
    je .empty_list

    ; else: lista no vacía
    ; list->last->next = node
    mov r12, [r8 + 8]     ; r12 = list->last
    mov [r12], r10        ; r12->next = node

    ; node->previous = list->last
    mov [r10 + 8], r12    ; node->previous = list->last

    ; list->last = node
    mov [r8 + 8], r10     ; list->last = node
    jmp .done

.empty_list:
    ; list->first = node
    mov [r8], r10
    ; list->last = node
    mov [r8 + 8], r10

.done:
    pop rbp
    ret

.return:
    pop rbp
    ret






string_proc_list_concat_asm:
     ; rdi = list, rsi = type, rdx = hash
     push rbp
     mov rbp, rsp
     xor rax, rax
 
     test rdi, rdi        ; chequeo si list es NULL
     je .return_null
 
     mov r8, rdi          ; guarda list en r8 antes de pisar rdi
     mov r9b, sil         ; guardo type → r9b   (r9b:parte baja de r9), (sil:parte baja de rsi)
 
     ; reemplazo de strdup(hash)
     mov rdi, rdx         ; rdi = hash
     call strlen          ; calcula strlen(hash)
     mov r10, rax         ; r10 = longitud de hash

     inc r10              ; +1 para el '\0'
     mov rdi, r10         ; rdi = longitud + 1
     call malloc          ; reserva memoria para la copia del hash
     test rax, rax        ; chequeo si malloc falló
     je .return_null
 
     mov r11, rax         ; result (string concatenado) → r11
 
     ; copiar el hash a result
     mov rdi, r11         ; destino: r11
     mov rsi, rdx         ; fuente: rdx (hash)
     call strcpy          ; copio hash a result
 
     ; ahora r11 tiene la copia del hash
     mov r10, r11         ; guardo el resultado (r10 es el resultado concatenado)

     mov r12, [r8]        ; current = list->first, es decir, el primer nodo de la lista
 
 .loop:
     test r12, r12        ; chequeo si current es NULL
     je .done
 
     movzx r13, byte [r12 + 16]   ; current->type
     cmp r13b, r9b                 ; comparo current->type con type buscado
     jne .next                     ; si no coincide, salto a next
 
     mov r14, [r12 + 24]           ; current->hash
     test r14, r14                 ; chequeo si current->hash es NULL
     je .next                      ; si es NULL, salto a next
 
     ; concatenar
     mov rdi, r10                  ; rdi = string actual (result)
     mov rsi, r14                  ; rsi = nuevo string a concatenar
     call str_concat               ; rax = nuevo result
 
     mov rdi, r10                  ; libero el string anterior
     call free
 
     mov r10, rax                  ; guardo el nuevo string concatenado en r10
 
 .next:
     mov r12, [r12]                ; current = current->next, avanzo de nodo
     jmp .loop
 
 .done:
     mov rax, r10
     pop rbp
     ret
 
 .return_null:
     xor rax, rax
     pop rbp
     ret
