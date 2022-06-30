%include "io.mac"

section .data                        ;We define the output strings 

ask_string db "Please Enter the string: ", 0
ask_end db "Do you want to End program (Press y/Y for Yes or n/N for NO): ", 0


section .bss                         ;We allocate space for string to be encrypted and exit command

encrypt resb 20
end_command resb 1

section .text

global _start


_start:                              

loop: 

    PutStr ask_string               ;Ask User of input string
    GetStr encrypt                  ;Store the string 
    mov eax,encrypt                 ;Address of string is stored in eax

encode:

    xor bl, bl                      ;We initialize bl to zero (for no overwritting)
    mov bl, [eax]                   ;The data of eax (only the first 8 bits (1 byte)) is stored in bl

    cmp bl, 0                       ;We check whether the character is null
    je end_encode                   ;We exit encoding-loop if null

    cmp bl, '0'                     ;We compare the 'bl' character with numbers and jump to their respective places
    je c0
    cmp bl, '1'
    je c1
    cmp bl, '2'
    je c2
    cmp bl, '3'
    je c3
    cmp bl, '4'
    je c4
    cmp bl, '5'
    je c5
    cmp bl, '6'
    je c6
    cmp bl, '7'
    je c7
    cmp bl, '8'
    je c8
    cmp bl, '9'
    je c9

    jmp inc_eax                     ;If the current character is a number then it is changed else we go to increament eax

c0:                                 ;changes is defined according to the encryption

    xor bl, bl
    mov bl, '5'
    jmp inc_eax

c1:
 
    xor bl, bl
    mov bl, '7'
    jmp inc_eax

c2:

    xor bl, bl
    mov bl, '9'
    jmp inc_eax

c3:
    
    xor bl, bl 
    mov bl, '1'
    jmp inc_eax

c4:

    xor bl, bl 
    mov bl, '3'
    jmp inc_eax

c5: 

    xor bl, bl
    mov bl, '6'
    jmp inc_eax

c6:

    xor bl, bl
    mov bl, '8'
    jmp inc_eax

c7:

    xor bl, bl
    mov bl, '0'
    jmp inc_eax

c8:

    xor bl, bl
    mov bl, '2'
    jmp inc_eax

c9:

    xor bl, bl
    mov bl, '4'
    jmp inc_eax

inc_eax:                                ;the changed data 'bl' is stored back in the memory addressed by eax

    mov [eax], bl

    inc eax                             ;eax is increamented
    jmp encode                          ;we loop for the next character


end_encode:

    PutStr encrypt                      ;After the string is looped completely and encrypted 
    nwln                                ;We print the encrypted string
    PutStr ask_end                      ;Ask for exit command
    GetStr end_command, 1
    xor ebx, ebx                        ;We initialize ebx to zero(for no overwritting)
    mov bl, [end_command]               ;bl has the character (exit command)

check_end:                              ;we exit loop if command is yes(y/Y)
    
    cmp bl, 'y'
    je _end
    cmp bl, 'Y'
    je _end
    jmp loop                            ;Else we loop again for another string

_end:

    xor eax, eax                        ;system exit call
    mov eax, 1
    int 0x80