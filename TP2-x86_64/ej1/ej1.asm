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
    push    rbp
    mov     rbp, rsp
    sub     rsp, 96
    mov     QWORD PTR [rbp-72], rdi
    mov     eax, esi
    mov     QWORD PTR [rbp-88], rdx
    mov     BYTE PTR [rbp-76], al
    cmp     QWORD PTR [rbp-72], 0
    je      .L19
    cmp     QWORD PTR [rbp-88], 0
    jne     .L20
.L19:
    mov     eax, 0
    jmp     .L21
.L20:
    mov     rax, QWORD PTR [rbp-72]
    mov     rax, QWORD PTR [rax]
    mov     QWORD PTR [rbp-8], rax
    mov     rax, QWORD PTR [rbp-72]
    mov     rax, QWORD PTR [rax]
    mov     QWORD PTR [rbp-16], rax
    jmp     .L22
.L24:
    mov     rax, QWORD PTR [rbp-8]
    mov     rax, QWORD PTR [rax]
    mov     QWORD PTR [rbp-8], rax
    mov     rax, QWORD PTR [rbp-16]
    mov     rax, QWORD PTR [rax]
    mov     rax, QWORD PTR [rax]
    mov     QWORD PTR [rbp-16], rax
    mov     rax, QWORD PTR [rbp-8]
    cmp     rax, QWORD PTR [rbp-16]
    jne     .L22
    mov     eax, 0
    jmp     .L21
.L22:
    cmp     QWORD PTR [rbp-16], 0
    je      .L23
    mov     rax, QWORD PTR [rbp-16]
    mov     rax, QWORD PTR [rax]
    test    rax, rax
    jne     .L24
.L23:
    mov     rax, QWORD PTR [rbp-88]
    mov     rdi, rax
    call    strlen
    mov     QWORD PTR [rbp-48], rax
    mov     rax, QWORD PTR [rbp-48]
    add     rax, 1
    mov     rdi, rax
    call    malloc
    mov     QWORD PTR [rbp-24], rax
    cmp     QWORD PTR [rbp-24], 0
    jne     .L25
    mov     eax, 0
    jmp     .L21
.L25:
    mov     rax, QWORD PTR [rbp-48]
    lea     rdx, [rax+1]
    mov     rcx, QWORD PTR [rbp-88]
    mov     rax, QWORD PTR [rbp-24]
    mov     rsi, rcx
    mov     rdi, rax
    call    memcpy
    cmp     QWORD PTR [rbp-24], 0
    jne     .L26
    mov     eax, 0
    jmp     .L21
.L26:
    mov     rax, QWORD PTR [rbp-24]
    mov     rdi, rax
    call    strlen
    mov     QWORD PTR [rbp-32], rax
    mov     rax, QWORD PTR [rbp-72]
    mov     rax, QWORD PTR [rax]
    mov     QWORD PTR [rbp-40], rax
    jmp     .L27
.L31:
    mov     rax, QWORD PTR [rbp-40]
    movzx   eax, BYTE PTR [rax+16]
    cmp     BYTE PTR [rbp-76], al
    jne     .L28
    mov     rax, QWORD PTR [rbp-40]
    mov     rax, QWORD PTR [rax+24]
    test    rax, rax
    je      .L28
    mov     rax, QWORD PTR [rbp-40]
    mov     rax, QWORD PTR [rax+24]
    mov     rdi, rax
    call    strlen
    mov     QWORD PTR [rbp-56], rax
    mov     rdx, QWORD PTR [rbp-32]
    mov     rax, QWORD PTR [rbp-56]
    add     rax, rdx
    cmp     rax, 1048576
    jbe     .L29
    mov     rax, QWORD PTR [rbp-24]
    mov     rdi, rax
    call    free
    mov     eax, 0
    jmp     .L21
.L29:
    mov     rax, QWORD PTR [rbp-40]
    mov     rdx, QWORD PTR [rax+24]
    mov     rax, QWORD PTR [rbp-24]
    mov     rsi, rdx
    mov     rdi, rax
    call    str_concat
    mov     QWORD PTR [rbp-64], rax
    cmp     QWORD PTR [rbp-64], 0
    jne     .L30
    mov     rax, QWORD PTR [rbp-24]
    mov     rdi, rax
    call    free
    mov     eax, 0
    jmp     .L21
.L30:
    mov     rax, QWORD PTR [rbp-24]
    mov     rdi, rax
    call    free
    mov     rax, QWORD PTR [rbp-64]
    mov     QWORD PTR [rbp-24], rax
    mov     rax, QWORD PTR [rbp-56]
    add     QWORD PTR [rbp-32], rax
.L28:
    mov     rax, QWORD PTR [rbp-40]
    mov     rax, QWORD PTR [rax]
    mov     QWORD PTR [rbp-40], rax
.L27:
    cmp     QWORD PTR [rbp-40], 0
    jne     .L31
    mov     rax, QWORD PTR [rbp-24]
.L21:
    leave
    ret