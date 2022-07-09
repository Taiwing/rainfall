section .text
global _start
_start:
push	0x0b		; push 11 to the stack
pop		eax			; pop it into eax (this is execve's int number)
push	0x0068732f	; "/sh" + "\0"
push	0x6e69622f  ; "/bin"
mov		ebx, esp	; move "/bin/sh" to ebx for int execve's first argument
int		0x80		; call execve
