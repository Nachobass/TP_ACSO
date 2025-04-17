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
    xor rax, rax

    cmp rsi, 0                  ; si hash == NULL
    je .return_null

    ; guardar argumentos
    movzx rcx, dil              ; type → rcx    guardo la parte baja de rdi en rcx y lleno con ceros el resto

    mov rdi, 32
    call malloc
    test rax, rax
    je .return_null


    mov qword [rax], 0      ; next
    mov qword [rax + 8], 0      ; previous
    mov byte  [rax + 16], dil    ; type
    mov qword [rax + 24], rsi   ; hash

    mov rsp, rbp
    pop rbp
    ret

.return_null:
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret




string_proc_list_add_node_asm:
    ; rdi = list, sil = type, rdx = hash

    push rbp
    mov rbp, rsp

    ; Verificación inicial: list == NULL || hash == NULL
    test rdi, rdi
    je .ret
    test rdx, rdx
    je .ret

    ; Guardar valores en registros temporales
    mov r8, rdi         ; r8 = list (usaremos este para operar con la lista)
    mov r9, rdx         ; r9 = hash
    movzx edi, sil      ; edi = (uint32_t) type (1er arg para la función create)
    mov rsi, r9         ; rsi = hash (2do arg para la función create)

    ; Llamar a string_proc_node_create(type, hash)
    call string_proc_node_create_asm
    test rax, rax
    je .ret             ; si node == NULL, salimos

    ; rax contiene el nodo creado
    mov r10, rax        ; r10 = node

    ; if (list->first == NULL)
    mov r11, [r8]       ; r11 = list->first
    test r11, r11
    je .empty_list      ; si es NULL → lista vacía

    ; Lista no vacía:
    mov r12, [r8 + 8]   ; r12 = list->last
    mov [r12], r10      ; list->last->next = node
    mov [r10 + 8], r12  ; node->previous = list->last
    mov [r8 + 8], r10   ; list->last = node
    jmp .done

.empty_list:
    ; lista vacía: poner first y last al nuevo nodo
    mov [r8], r10       ; list->first = node
    mov [r8 + 8], r10   ; list->last = node

.done:
    pop rbp
    ret

.ret:
    pop rbp
    ret







string_proc_list_concat_asm:
    ; rdi = list, rsi = type, rdx = hash
    push rbp
    mov rbp, rsp
    sub rsp, 96            ; reservar espacio local

    mov [rbp-72], rdi      ; list
    movzx eax, sil         ; pasar type como 8 bits → eax
    mov [rbp-88], rdx      ; hash
    mov [rbp-76], al       ; guardar type en [rbp-76]

    cmp qword [rbp-72], 0
    je .return_null
    cmp qword [rbp-88], 0
    jne .continue
.return_null:
    mov rax, 0
    jmp .done

.continue:
    mov rax, [rbp-72]
    mov rax, [rax]
    mov [rbp-8], rax       ; aux1
    mov rax, [rbp-72]
    mov rax, [rax]
    mov [rbp-16], rax      ; current

.loop_check:
    ; verificar que fast y fast->next no sean NULL
    mov rax, [rbp-16]
    test rax, rax
    je .init_result
    mov rax, [rax]
    test rax, rax
    je .init_result

    ; slow = slow->next
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-8], rax

    ; fast = fast->next->next
    mov rax, [rbp-16]
    mov rax, [rax]
    mov rax, [rax]
    mov [rbp-16], rax

    ; comparar slow y fast
    mov rax, [rbp-8]
    cmp rax, [rbp-16]
    jne .loop_check

    mov rax, 0
    jmp .done

.loop_start:
    cmp qword [rbp-16], 0
    je .init_result

    mov rax, [rbp-16]
    mov rax, [rax]
    test rax, rax
    jne .loop_check

.init_result:
    mov rax, [rbp-88]
    mov rdi, rax
    call strlen
    mov [rbp-48], rax

    mov rax, [rbp-48]
    add rax, 1
    mov rdi, rax
    call malloc
    mov [rbp-24], rax

    cmp qword [rbp-24], 0
    jne .copy_hash
    mov rax, 0
    jmp .done

.copy_hash:
    mov rax, [rbp-48]
    lea rdx, [rax+1]
    mov rcx, [rbp-88]
    mov rsi, rcx
    mov rdi, [rbp-24]
    call memcpy

    cmp qword [rbp-24], 0
    jne .prepare_loop
    mov rax, 0
    jmp .done

.prepare_loop:
    mov rdi, [rbp-24]
    call strlen
    mov [rbp-32], rax

    mov rax, [rbp-72]
    mov rax, [rax]
    mov [rbp-40], rax

.loop_nodes:
    cmp qword [rbp-40], 0
    je .end_loop

    mov rax, [rbp-40]
    movzx eax, byte [rax+16]
    cmp al, [rbp-76]
    jne .next_node

    mov rax, [rbp-40]
    mov rax, [rax+24]
    test rax, rax
    je .next_node

    mov rdi, [rbp-40]
    mov rdi, [rdi+24]
    call strlen
    mov [rbp-56], rax

    mov rdx, [rbp-32]
    mov rax, [rbp-56]
    add rax, rdx
    cmp rax, 1048576
    jbe .concat_ok

    ; si supera el tamaño permitido
    mov rdi, [rbp-24]
    call free
    mov rax, 0
    jmp .done

.concat_ok:
    mov rdx, [rbp-40]
    mov rdx, [rdx+24]
    mov rsi, rdx
    mov rdi, [rbp-24]
    call str_concat
    mov [rbp-64], rax

    cmp qword [rbp-64], 0
    jne .replace_result

    mov rdi, [rbp-24]
    call free
    mov rax, 0
    jmp .done

.replace_result:
    mov rdi, [rbp-24]
    call free
    mov rax, [rbp-64]
    mov [rbp-24], rax

    mov rax, [rbp-56]
    add [rbp-32], rax

.next_node:
    mov rax, [rbp-40]
    mov rax, [rax]
    mov [rbp-40], rax
    jmp .loop_nodes

.end_loop:
    mov rax, [rbp-24]

.done:
    mov rsp, rbp
    pop rbp
    ret