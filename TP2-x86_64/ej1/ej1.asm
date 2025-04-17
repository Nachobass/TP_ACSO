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
     ; rdi = list, rsi = type, rdx = hash
     push rbp
     mov rbp, rsp
     xor rax, rax
 
     test rdi, rdi        ; chequeo si list es NULL
     je .return            ; si es NULL, retorno

     test rdx, rdx        ; chequeo si hash es NULL
     je .return            ; si es NULL, retorno

     ; llamar a string_proc_node_create(type, hash)
     mov rdi, rsi         ; rdi = type
     mov rsi, rdx         ; rsi = hash
     call string_proc_node_create_asm
     test rax, rax        ; chequeo si node es NULL
     je .return            ; si es NULL, retorno
 
     mov r8, rax          ; node = resultado de string_proc_node_create
 
     ; Verificar si list->first es NULL
     mov r9, [rdi]        ; r9 = list->first
     test r9, r9
     je .list_empty        ; si list->first es NULL, la lista está vacía

     ; Verificar si list->last es NULL (caso de lista mal construida)
     mov r10, [rdi + 8]   ; r10 = list->last
     test r10, r10
     je .list_corrupted    ; si list->last es NULL, la lista está corrupta

     ; Lista no vacía: list->last->next = node, node->previous = list->last, list->last = node
     mov r11, [r10]       ; r11 = list->last->next
     mov [r10], r8        ; list->last->next = node
     mov [r8 + 8], r10    ; node->previous = list->last
     mov [rdi + 8], r8    ; list->last = node
     jmp .done

 .list_empty:
     ; La lista está vacía: list->first = node, list->last = node
     mov [rdi], r8        ; list->first = node
     mov [rdi + 8], r8    ; list->last = node
     jmp .done

 .list_corrupted:
     ; Lista corrupta: liberar node
     mov rdi, r8          ; rdi = node
     call free
     jmp .done

 .return:
     xor rax, rax         ; retorno NULL
     pop rbp
     ret

 .done:
     pop rbp
     ret





section .data
    MAX_RESULT_LEN equ 1048576  ; 1 MB máximo

section .text
string_proc_list_concat_asm:
    ; rdi = list, rsi = type, rdx = hash
    push rbp
    mov rbp, rsp
    xor rax, rax

    test rdi, rdi            ; si list == NULL
    je .return_null

    mov r8, rdi              ; guardo list en r8 antes de pisar rdi
    mov r9b, sil             ; guardo type → r9b   (r9b:parte baja de r9), (sil:parte baja de rsi)

    ; rdx = hash
    mov rdi, rdx             ; argumento para strlen(hash)
    call strlen              ; rax = strlen(hash)
    mov rcx, rax             ; guardo longitud de hash en rcx
    inc rcx                  ; +1 para el '\0'

    mov rdi, rcx             ; malloc(strlen + 1)
    call malloc
    test rax, rax
    je .return_null          ; si malloc falla

    mov r10, rax             ; result = malloc(...) → r10

    ; copiar hash a result
    mov rdi, r10             ; destino
    mov rsi, rdx             ; fuente (hash)
    mov rdx, rcx             ; cantidad de bytes (strlen + 1)
    call memcpy              ; memcpy(result, hash, strlen + 1)


    mov r11, [r8]            ; current = list->first, es decir, el primer nodo de la lista

    ; Detección de ciclo
    mov r12, r11             ; slow = current
    mov r13, r11             ; fast = current

.loop_cycle_check:
    test r13, r13            ; fast == NULL?
    je .no_cycle_detected
    mov r13, [r13 + 8]       ; fast = fast->next->next
    test r13, r13
    je .no_cycle_detected
    mov r13, [r13 + 8]
    mov r14, [r12 + 8]       ; slow = slow->next
    test r12, r12
    je .no_cycle_detected
    cmp r12, r13             ; slow == fast?
    je .return_null          ; ciclo detectado

    mov r12, r14
    jmp .loop_cycle_check

.no_cycle_detected:

    ; Continuo con la concatenación de cadenas
    mov r11, [r8]                ; current = list->first

.loop:
    test r11, r11                ; chequeo si current es NULL
    je .done

    movzx r12, byte [r11 + 16]  ; current->type
    cmp r12b, r9b                ; comparo current->type con type buscado
    jne .next                    ; si no coincide, salto a next

    mov r13, [r11 + 24]          ; current->hash
    test r13, r13                ; si current->hash == NULL
    je .next                     ; si es NULL, salto a next

    ; Verificar si concatenar causaría overflow
    mov rsi, r13                 ; rsi = current->hash
    call strlen
    add rax, r10                 ; rax = current_len + add_len
    cmp rax, MAX_RESULT_LEN      ; si excede el tamaño máximo
    ja .return_null              ; si excede, retorno NULL

    ; concatenar
    mov rdi, r10                 ; rdi = string actual (result)
    mov rsi, r13                 ; rsi = nuevo string a concatenar
    call str_concat              ; rax = nuevo result

    mov rdi, r10                 ; libero el string anterior
    call free

    mov r10, rax                 ; guardo el nuevo string concatenado en r10

.next:
    mov r11, [r11]               ; current = current->next, avanzo de nodo
    jmp .loop

.done:
    mov rax, r10
    pop rbp
    ret

.return_null:
    xor rax, rax
    pop rbp
    ret