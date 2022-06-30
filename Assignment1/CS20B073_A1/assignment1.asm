%include "io.mac"

section .data
sum dd 0                        ;initialize the sum value to zero 

section .bss
n resd 1                        ;4 byte space for number of inputs

section .text
global _start

_start:
	GetLInt [n]                 ;we get the input and store in [n]

    mov ebx, 0                  ;we initialize the temp sum holder to zero
    mov ecx, 0                  ;we initialize the counter to 0

loop:                           ;we iterate over every integer and add it to ebx
    cmp ecx, [n]                ;compare the counter with [n]
    je end              
    inc ecx
    GetLInt eax                 ;getting input integer and adding
    add ebx, eax
    xor eax, eax                ;resetting the register
    jmp loop

end:
    mov [sum], ebx              ;storing our sum in [sum]
    xor ebx, ebx                ;resetting the registers with 0
    xor ecx, ecx

display: 

    PutLInt [n]                 ;printing the number of integers 
    nwln                
    PutLInt [sum]               ;printing the sum
    nwln
    mov  eax, 1                 ;system exit call
    int  0x80


	

