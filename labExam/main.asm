%include "io.mac"

section .data

section .bss

gstr resb 21            ;for getting the string in sub_task_1
n resd 1                ;for getting the number in sub_task_2
c resd 1                ;for getting the opcode

section .text
global _start

sub_task_1:             ;for carrying out the sub_task_1

    enter 0, 0
    mov eax, [ebp+12]
    mov ecx, [ebp+8]    ;initialize ecx to zero

loop:                   ; for comparing each char in the string

    xor bl, bl          ;initialize bl to zero
    mov bl, [eax]       ;moves one byte of string to bl

    cmp bl, 0           ;if null character then quit
    je end_loop

    cmp bl, 'a'         ;if(char == 'a') then don't increase
    je next_char
    cmp bl, 'A'         ;Similarily for all the vowel and its Capital form we don't increase cnt
    je next_char


    cmp bl, 'e'
    je next_char        ;for 'e'
    cmp bl, 'E'
    je next_char


    cmp bl, 'i'
    je next_char        ;for 'i'
    cmp bl, 'I'
    je next_char


    cmp bl, 'o'
    je next_char        ;for 'o'
    cmp bl, 'O'
    je next_char


    cmp bl, 'u'
    je next_char        ;for 'u'
    cmp bl, 'U'
    je next_char

inc_cnt:                ;we increament the cnt if no vowels are found

    inc ecx
    jmp next_char

next_char:              ;we then go to the next character

    inc eax
    jmp loop

end_loop:
    mov [ebp+8], ecx
    leave
    ret

sub_task_2:             ;this is a function for sub_task_2 F(x)

    enter 0, 0          ;we push esp and set ebp to esp
    mov eax, [ebp+8]    ;we consider the passed parameter x in eax

    cmp eax, 10         ;if eax > 10 then type_1
    jg type_1
    cmp eax, 7          ;if eax <= 10 && eax >7 then type_2
    jg type_2
    cmp eax, 0          ;if eax <= 7 && eax > 0 then type_1
    jg type_3
    mov ebx, 0          ;if non of the case then ret 0
    ret

type_3:
    mov ebx, 9          ;ret 9 
    leave
    ret

type_2:                 ;ret 2*x + 4*F(x-2)

    ; first calculating f(x-2)
    sub eax, 2
    push eax
    call sub_task_2
    pop eax

    ; we get answer in ebx
    
    add eax, 2          ; we restore the value of eax
    imul ebx, 4         ; 4*F(x-2)
    imul eax, 2         ; 2*x
    add ebx, eax        ;2*x + 4*F(x-2)
    leave
    ret

type_1:                 ;ret F(x-1) + (x-1)

    ; first calculating f(x-1)

    dec eax             ;x-1
    push eax            ;pass x-1
    call sub_task_2     ;F(x-1)
    pop eax             ;pop x-1
    add ebx, eax        ;F(x-1) in ebx, so ebx+eax == x-1 + F(x-1)
    leave
    ret

_start:

    GetLInt [c];        ;Get the value of opcode
    mov edx, [c]        
    cmp edx, 1          ;if(c == 1) then do sub_task_1
    je do_1
    cmp edx, 2          ;else if(c == 2) then do sub_task_2
    je do_2
    jmp exit            ;else do nothing

do_1:
    GetStr gstr         ;get the string
    mov eax, gstr       ;mov the address of the string to eax
    push eax
    push 0
    call sub_task_1
    pop ebx
    pop eax
    PutLInt ebx
    nwln
    jmp exit

do_2:
    GetLInt [n]
    mov eax, [n]
    push eax
    call sub_task_2
    pop eax
    PutLInt ebx
    nwln
    jmp exit


exit:

    mov eax,1
    int 0x80
	
