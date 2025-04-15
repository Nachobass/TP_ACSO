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



; orden de parametros RDI, RSI, RDX, RSX

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



; orden de parametros RDI, RSI, RDX, RSX

string_proc_node_create_asm:
    push rbp
    mov rbp, rsp
    push rbx                ; usamos rbx para guardar direcciones temporales
    xor rax, rax

    ; guardar argumentos
    mov rdx, rsi        ; hash → rdx
    movzx rcx, dil      ; type → rcx    guardo la parte baja de rdi en rcx y lleno con ceros el resto

    mov rdi, 32
    call malloc
    test rax, rax
    je .return_null
    mov rbx, rax            ; guardamos el puntero al nodo en rbx

    ; strdup(hash) - primero strlen
    mov rdi, rdx            ; hash como argumento
    call strlen
    inc rax                 ; +1 para null terminator
    mov rdi, rax
    call malloc
    test rax, rax
    je .free_node_and_return_null
    mov rsi, rdx            ; rsi ← hash
    mov rdi, rax            ; rdi ← destino malloc
    call strcpy             ; duplica el string

    ; ahora rax ← copia del hash
    ; construir el nodo
    ; rax = nodo
    mov qword [rax + 0], 0      ; next
    mov qword [rax + 8], 0      ; previous
    mov byte  [rax + 16], cl       ; type, cl es la parte baja de rcx
    mov qword [rax + 24], rax      ; hash

    mov rax, rbx                ; devolver el puntero al nodo

    pop rbx
    pop rbp
    ret

.free_node_and_return_null:
    mov rdi, rbx
    call free

.return_null:
    xor rax, rax
    pop rbx
    pop rbp
    ret



; orden de parametros RDI, RSI, RDX, RSX

string_proc_list_add_node_asm:
    ; rdi = list, rsi = type, rdx = hash
    push rbp
    mov rbp, rsp
    xor rax, rax

    test rdi, rdi           ; chequeo si list es NULL
    je .ret

    ; crear nodo: string_proc_node_create_asm(type, hash) preparo sus parametros
    mov rcx, rdx            ; hash → rcx
    mov rdx, rcx            ; hash → rdx
    movzx rsi, sil          ; type → rsi           ; sil es la parte baja de rsi
    call string_proc_node_create_asm
    test rax, rax
    je .ret

    mov r8, rdi             ; list → r8, siendo rdi list->first y r8+8 list->last
    mov r9, rax             ; node → r9

    ; list->first == NULL ?
    mov r10, [r8]           ; muevo a r10 la dirección de list->first (ya que list->last seria [r8 +8 ])
    test r10, r10           ; chequeo si es NULL, es decir, si la lista está vacía
    jne .not_empty          ; jne si no es NULL, es decir, si la lista no está vacía

    ; lista vacía
    mov [r8], r9            ; list->first = node
    mov [r8 + 8], r9        ; list->last = node
    jmp .ret

.not_empty:
    mov r10, [r8 + 8]       ; last = list->last
    mov [r10], r9           ; last->next = node
    mov [r9 + 8], r10       ; node->previous = last
    mov [r8 + 8], r9        ; list->last = node

.ret:
    pop rbp
    ret



; orden de parametros RDI, RSI, RDX, RSX

string_proc_list_concat_asm:
    ; rdi = list, rsi = type, rdx = hash
    push rbp
    mov rbp, rsp
    xor rax, rax

    test rdi, rdi        ; chequeo si list es NULL
    je .return_null

    mov r8, rdi          ; guarda list en r8 antes de pisar rdi
    mov r9b, sil         ; guardo type → r9b   (r9b:parte baja de r9), (sil:parte baja de rsi)

    mov rdi, rdx         ; strdup(hash)
    call strdup
    test rax, rax        ; rax = resultado del strdup, si falla retorno NULL
    je .return_null

    mov r10, rax         ; result (string concatenado) → r10

    mov r11, [r8]        ; current = list->first, es decir, el primer nodo de la lista

.loop:
    test r11, r11       ; chequeo si current es NULL
    je .done

    movzx r12b, byte [r11 + 16]   ; current->type
    cmp r12b, r9b                 ; comparo current->type con type buscado
    jne .next                     ; si no coincide, salto a next

    mov r13, [r11 + 24]           ; current->hash
    test r13, r13                 ; chequeo si current->hash es NULL
    je .next                      ; si es NULL, salto a next

    ; concatenar
    mov rdi, r10                  ; rdi = string actual (result)
    mov rsi, r13                  ; rsi = nuevo string a concatenar
    call str_concat               ; rax = nuevo result

    mov rdi, r10                  ; libero el string anterior
    call free

    mov r10, rax                  ; guardo el nuevo string concatenado en r10

.next:
    mov r11, [r11]                ; current = current->next, avanzo de nodo
    jmp .loop

.done:
    mov rax, r10
    pop rbp
    ret

.return_null:
    xor rax, rax
    pop rbp
    ret
