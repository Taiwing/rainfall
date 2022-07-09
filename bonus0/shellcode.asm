section .text
global _start
_start:
xor		edx, edx	; set edx to 0
push	edx			; push it to the stack to end the command string
push	0x68732f2f	; "//sh" end of the command string
push	0x6e69622f	; "/bin" start of the command string
mov		ebx, esp	; set ebx to the start of the command string
push	edx			; push a NULL
push	ebx			; push the command string's address
mov		ecx, esp	; set ecx as a pointer to the command string's address (argv)
push	0xb			; push 11 (execve int number)
pop		eax			; set eax to 11
int		0x80		; call execve
