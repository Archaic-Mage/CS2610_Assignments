%include "io.mac"

section .data

section .bss
n resd 1
element resd 1

section .text
global _start
_start:
	
        GetLInt [n]
        mov ecx, 0
        mov eax, 0

loop:

    cmp ecx, [n]
    je endLoop
    GetLInt [element]
    add eax, [element]
    inc ecx
    jmp loop

endLoop:
    
    PutLInt eax
    mov eax, 1
    int 0x80
