%include "io.mac"

section .data

ask_m db "Please enter value of m: ", 0
ask_n db "Please enter value of n: ", 0

section .bss

m resd 1
n resd 1

section .txt 

global _start

ackermann:

    enter 0, 0
    mov eax, [ebp+12]                   ; eax is set to m
    mov ebx, [ebp+8]                    ; ebx is set to n

; if(m == 0) return n+1

    cmp eax, 0
    je set_ret1

;if(n == 0) return A(m-1, 1)

    cmp ebx, 0
    je set_ret2

;return A(m-1, A(m, n-1))

    ;first calculating the inner part A(m, n-1)

    dec ebx
    push eax                            ; m is pushed
    push ebx                            ; n-1 is pushed
    call ackermann
    pop ebx
    pop eax

    ;Now calculating the outer part A(m-1, A(m, n-1))
    dec eax
    push eax                            ; m-1 is pushed
    push ecx                            ; the ans to A(m, n-1) is pushed
    call ackermann
    pop ebx
    pop eax
    leave 
    ret

set_ret1:                               ; the ans is stored in ecx (register)

    mov ecx, ebx
    inc ecx
    leave 
    ret 

set_ret2:

    dec eax
    mov edx, 1
    push eax                            ; m is pushed
    push edx                            ; one is pushed
    call ackermann
    pop ebx
    pop eax
    leave
    ret

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