%include "io.mac"

section .txt

global ackermann

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