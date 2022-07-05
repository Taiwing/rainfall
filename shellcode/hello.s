.global _start
.text
_start:
	# safe null, whatever that means
	xor %eax, %eax
	# push the "hello world\n" string to the stack
	push %eax
	push $0x0a646c72
	push $0x6f77206f
	push $0x6c6c6548
	# write(1, string, 13)
	mov $4, %eax	# int 4 is write
	mov $1, %ebx	# fd 1 is stdout
	mov %esp, %ecx	# hello world string
	mov $13, %edx	# number of bytes
	int $0x80
	# exit (0)
	mov $1, %eax	# int 1 is exit
	xor %ebx, %ebx	# return code 0
	int $0x80
