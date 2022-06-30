%include "io.mac"

section .data

fname db 'input.txt',0              ; name of the file
parent_msg db 'Parent Output: ', 0
child_msg db 'Child Output: ', 0
mmap_arg:                           ;structure for the mmap 
.addr: dd 0
.len: dd 4
.prot: dd 1
.flags: dd 2
.fd: dd -1
.offset: dd 0

section .bss

n resb 4                            ;saves the number of input
num resb 4                          ;saves each element temporarily
fd_out resb 1                       ;stores the file descriptor for mmap
arr resd 1000000                    ;temporary storage for sorting
sum resd 1


section .txt

global _start

_start:


; creating the file to store n numbers

;opens and creates file
mov eax, 8
mov ebx, fname
mov ecx, 0777o
int 0x80

mov [fd_out], eax

;get the number of inputs
GetLInt eax
mov [n], eax
push eax
push 0

;for each we take the input
_loop:
pop eax
pop ebx
cmp eax, ebx
je end

inc eax
push ebx
push eax

;Get input if not equal

;Writing to the file
GetLInt eax
mov [num], eax
mov edx, 4
mov eax, 4
mov ebx, [fd_out]
mov ecx, num
int 0x80
jmp _loop

end:

;closing the file

mov eax, 6
mov ebx, [fd_out]
int 0x80

;opening file to read and write
mov eax, 5
mov ebx, fname
mov ecx, 2
mov edx, 0777o
int 0x80

;mapping the file to the memory
mov [mmap_arg.fd], eax
mov eax, 90
mov ebx, mmap_arg
int 0x80
mov [fd_out], eax
push eax



; for(int i = 0; i<n; i++) 

mov ecx, [n]                ;total num of inputs
mov ebx, [n]                ;for upper bound while looping 
imul ebx, ebx, 4
push ebx
mov edx, arr                ;arr starting index
mov ebx, [fd_out]           ;starting index for the input file

copy_input:
cmp ecx, 0              ;condition of the loop
je sort
mov eax, [ebx]
mov [edx], eax
add edx, 4
add ebx, 4
dec ecx
jmp copy_input

sort:
mov ecx, arr            ;arr starting index
push ecx                

loop_1:
pop edx
pop ebx
push ebx
add ebx, edx        ;calculating the upper bound
cmp ecx, ebx
push edx
je end_sort

mov ebx, ecx        ;j = arr
mov eax, [ecx]      ;temp = arr[i]
sub ebx, 4          ;j = arr-1

loop_2:
cmp ebx, arr - 4   ;if(j > arr-1)
je assign

cmp eax, [ebx]     ;if(arr[i] > *j)
jge assign

mov edx, [ebx]     ;*(j+1) = *j
mov [ebx + 4], edx
sub ebx, 4         ;j--
jmp loop_2

assign:
mov [ebx + 4], eax   ;*j = arr[i]
add ecx, 4
jmp loop_1

end_sort:
mov ecx, arr            ;pointer to the arr
PutStr parent_msg
nwln

print:                  ;prints the sorted array
pop edx
pop ebx
push ebx
add ebx, edx
cmp ecx, ebx
push edx
je exit_print
PutLInt [ecx]
nwln
add ecx, 4
jmp print

exit_print:

pop edx
pop ebx


;setting up counters for the process
mov ecx, 0
mov edx, [n]
mov ebx, 0


;system call to fork processes
mov eax, 2
int 0x80
cmp eax, 0
jz child

parent:
jmp exit

child:

pop eax                 ;gets the start index for the file
cmp edx, ecx            ;check the conditions
je exit_child         
add ebx, [eax]          ; adds the numbers
add eax, 4
push eax
inc ecx
jmp child

exit_child:

PutStr child_msg
nwln
PutLInt ebx
nwln

mov eax, 91
mov ebx, [fd_out]
mov ecx, 8
int 0x80

mov eax, 6
mov ebx, [fd_out]
int 0x80


exit:
mov eax, 91
mov ebx, [fd_out]
mov ecx, 8
int 0x80

mov eax, 6
mov ebx, [fd_out]
int 0x80


mov eax, 1
int 0x80


