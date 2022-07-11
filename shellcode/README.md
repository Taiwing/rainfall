## How to create a shellcode

This is a hello world gnu-asm example:

```assembly
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
```

Here the write system call and exit are directly called inside the \_start
function. Then the resulting binary will only contain the needed instructions
for printing our string and exiting. From there we compile our code and turn the
resulting binary into a string:

```shell
# we compile the asm file above
gcc -c hello.s
# link it
ld -o hello hello.o
# check that it works
./hello # -> Hello world
# turn it into a string
./generate_shellcode.bash hello
```

Which will finally output our hello world shellcode string:

```
\x50\x68\x72\x6c\x64\x0a\x68\x6f\x20\x77\x6f\x68\x48\x65\x6c\x6c\xb8\x04\x00\x00\x00\xbb\x01\x00\x00\x00\x89\xe1\xba\x0d\x00\x00\x00\xcd\x80\xb8\x01\x00\x00\x00\x31\xdb\xcd\x80
```

To test that it works, we can simpy use this C program by replacing
`<shellcode_string>` by the above:

```C
int main(void)
{
	char *code = "<shellcode_string>";
	int (*func)();
	func = (int (*)())code;
	func();
	return (0);
}
```

This should compile and run like this:

```shell
# compile it
gcc test.c -o test
# run it
./test # -> Hello world
```
