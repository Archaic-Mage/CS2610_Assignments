%include "function.inc"

section .data

ask_m db "Please enter value of m: ", 0
ask_n db "Please enter value of n: ", 0

section .bss

m resd 1
n resd 1

section .txt 

global _start

_start:

    PutStr ask_m
    GetLInt [m]
    PutStr ask_n
    GetLInt [n]

    ;we start to call the function

    mov eax, [m]
    mov ebx, [n]
    push eax
    push ebx
    call ackermann
    pop ebx
    pop eax

    ;ans is in ecx

    PutLInt ecx                             ;we print the ans
    nwln

    mov eax, 1                              ;system exit code
    int 0x80