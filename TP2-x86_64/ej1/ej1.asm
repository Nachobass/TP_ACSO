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
        push    rbp
        mov     rbp, rsp

        sub     rsp, 16
        mov     edi, 16
        call    malloc
        mov     QWORD [rbp-8], rax
        cmp     QWORD [rbp-8], 0
        jne     .L4

        mov     eax, 0
        jmp     .L5
.L4:
        mov     rax, QWORD [rbp-8]
        mov     QWORD [rax], 0
        mov     rax, QWORD [rbp-8]
        mov     QWORD [rax+8], 0
        mov     rax, QWORD [rbp-8]
.L5:
        leave
        ret



string_proc_node_create_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     eax, edi
        mov     QWORD [rbp-32], rsi
        mov     BYTE [rbp-20], al
        cmp     QWORD [rbp-32], 0
        jne     .L7
        mov     eax, 0
        jmp     .L8
.L7:
        mov     edi, 32
        call    malloc
        mov     QWORD [rbp-8], rax
        cmp     QWORD [rbp-8], 0
        jne     .L9
        mov     eax, 0
        jmp     .L8
.L9:
        mov     rax, QWORD [rbp-8]
        movzx   edx, BYTE [rbp-20]
        mov     BYTE [rax+16], dl
        mov     rax, QWORD [rbp-8]
        mov     rdx, QWORD [rbp-32]
        mov     QWORD [rax+24], rdx
        mov     rax, QWORD [rbp-8]
        mov     QWORD [rax], 0
        mov     rax, QWORD [rbp-8]
        mov     QWORD [rax+8], 0
        mov     rax, QWORD [rbp-8]
.L8:
        leave
        ret




string_proc_list_add_node_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 48
        mov     QWORD [rbp-24], rdi
        mov     eax, esi
        mov     QWORD [rbp-40], rdx
        mov     BYTE [rbp-28], al
        cmp     QWORD [rbp-24], 0
        je      .L16
        cmp     QWORD [rbp-40], 0
        je      .L16
        movzx   eax, BYTE [rbp-28]
        mov     rdx, QWORD [rbp-40]
        mov     rsi, rdx
        mov     edi, eax
        call    string_proc_node_create
        mov     QWORD [rbp-8], rax
        cmp     QWORD [rbp-8], 0
        je      .L17
        mov     rax, QWORD [rbp-24]
        mov     rax, QWORD [rax]
        test    rax, rax
        jne     .L15
        mov     rax, QWORD [rbp-24]
        mov     rdx, QWORD [rbp-8]
        mov     QWORD [rax], rdx
        mov     rax, QWORD [rbp-24]
        mov     rdx, QWORD [rbp-8]
        mov     QWORD [rax+8], rdx
        jmp     .L10
.L15:
        mov     rax, QWORD [rbp-24]
        mov     rax, QWORD [rax+8]
        mov     rdx, QWORD [rbp-8]
        mov     QWORD [rax], rdx
        mov     rax, QWORD [rbp-24]
        mov     rdx, QWORD [rax+8]
        mov     rax, QWORD [rbp-8]
        mov     QWORD [rax+8], rdx
        mov     rax, QWORD [rbp-24]
        mov     rdx, QWORD [rbp-8]
        mov     QWORD [rax+8], rdx
        jmp     .L10
.L16:
        nop
        jmp     .L10
.L17:
        nop
.L10:
        leave
        ret





string_proc_list_concat_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 96
        mov     QWORD [rbp-72], rdi
        mov     eax, esi
        mov     QWORD [rbp-88], rdx
        mov     BYTE [rbp-76], al
        cmp     QWORD [rbp-72], 0
        je      .L19
        cmp     QWORD [rbp-88], 0
        jne     .L20
.L19:
        mov     eax, 0
        jmp     .L21
.L20:
        mov     rax, QWORD [rbp-72]
        mov     rax, QWORD [rax]
        mov     QWORD [rbp-8], rax
        mov     rax, QWORD [rbp-72]
        mov     rax, QWORD [rax]
        mov     QWORD [rbp-16], rax
        jmp     .L22
.L24:
        mov     rax, QWORD [rbp-8]
        mov     rax, QWORD [rax]
        mov     QWORD [rbp-8], rax
        mov     rax, QWORD [rbp-16]
        mov     rax, QWORD [rax]
        mov     rax, QWORD [rax]
        mov     QWORD [rbp-16], rax
        mov     rax, QWORD [rbp-8]
        cmp     rax, QWORD [rbp-16]
        jne     .L22
        mov     eax, 0
        jmp     .L21
.L22:
        cmp     QWORD [rbp-16], 0
        je      .L23
        mov     rax, QWORD [rbp-16]
        mov     rax, QWORD [rax]
        test    rax, rax
        jne     .L24
.L23:
        mov     rax, QWORD [rbp-88]
        mov     rdi, rax
        call    strlen
        mov     QWORD [rbp-48], rax
        mov     rax, QWORD [rbp-48]
        add     rax, 1
        mov     rdi, rax
        call    malloc
        mov     QWORD [rbp-24], rax
        cmp     QWORD [rbp-24], 0
        jne     .L25
        mov     eax, 0
        jmp     .L21
.L25:
        mov     rax, QWORD [rbp-48]
        lea     rdx, [rax+1]
        mov     rcx, QWORD [rbp-88]
        mov     rax, QWORD [rbp-24]
        mov     rsi, rcx
        mov     rdi, rax
        call    memcpy
        cmp     QWORD [rbp-24], 0
        jne     .L26
        mov     eax, 0
        jmp     .L21
.L26:
        mov     rax, QWORD [rbp-24]
        mov     rdi, rax
        call    strlen
        mov     QWORD [rbp-32], rax
        mov     rax, QWORD [rbp-72]
        mov     rax, QWORD [rax]
        mov     QWORD [rbp-40], rax
        jmp     .L27
.L31:
        mov     rax, QWORD [rbp-40]
        movzx   eax, BYTE [rax+16]
        cmp     BYTE [rbp-76], al
        jne     .L28
        mov     rax, QWORD [rbp-40]
        mov     rax, QWORD [rax+24]
        test    rax, rax
        je      .L28
        mov     rax, QWORD [rbp-40]
        mov     rax, QWORD [rax+24]
        mov     rdi, rax
        call    strlen
        mov     QWORD [rbp-56], rax
        mov     rdx, QWORD [rbp-32]
        mov     rax, QWORD [rbp-56]
        add     rax, rdx
        cmp     rax, 1048576
        jbe     .L29
        mov     rax, QWORD [rbp-24]
        mov     rdi, rax
        call    free
        mov     eax, 0
        jmp     .L21
.L29:
        mov     rax, QWORD [rbp-40]
        mov     rdx, QWORD [rax+24]
        mov     rax, QWORD [rbp-24]
        mov     rsi, rdx
        mov     rdi, rax
        call    str_concat
        mov     QWORD [rbp-64], rax
        cmp     QWORD [rbp-64], 0
        jne     .L30
        mov     rax, QWORD [rbp-24]
        mov     rdi, rax
        call    free
        mov     eax, 0
        jmp     .L21
.L30:
        mov     rax, QWORD [rbp-24]
        mov     rdi, rax
        call    free
        mov     rax, QWORD [rbp-64]
        mov     QWORD [rbp-24], rax
        mov     rax, QWORD [rbp-56]
        add     QWORD [rbp-32], rax
.L28:
        mov     rax, QWORD [rbp-40]
        mov     rax, QWORD [rax]
        mov     QWORD [rbp-40], rax
.L27:
        cmp     QWORD [rbp-40], 0
        jne     .L31
        mov     rax, QWORD [rbp-24]
.L21:
        leave
        ret