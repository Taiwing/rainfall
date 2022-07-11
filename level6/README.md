## Heap overflow, 2 mallocs in a row (level6)

This level's [binary](source.c) requires an argument, otherwise it segfaults.
Then, upon this argument treatment it prints "Nope" or it segfaults again if the
argument is too long.

This binary makes two malloc calls in a row. It first allocates 64 bytes and
then 4 bytes. The first allocated pointer is used as the destination buffer on
a strcpy call from argv[1]. The second allocation is set to the address of the
_m_ function which prints "Nope" before the strcpy call. After that, the second
pointer is dereferenced twice to execute the function it points to.

So what we have to do here is to overflow the 64 byte memory zone to overwrite
the second one to set it to the address of the _n_ function which calls system
to print the password of the next level. This works because multiple malloc
calls usually allocate memory next to one another.

First lets see from when we start writing into the second allocated memory zone:

```shell
# run gdb on binary
gdb ./level6
# give it an overflowing first argument
(gdb) run "a" * 64 + "bbbbccccddddeeeeffffgggghhhhiiii"

Program received signal SIGSEGV, Segmentation fault.
0x64646464 in ?? ()
```

Since 0x64 corresponds to the character 'd' in ascii, we can deduce that the
second memory zone is 8 bytes after the first one. Then we have to obtain the
_n_ function's address:

```shell
(gdb) i addr n
Symbol "n" is at 0x8048454 in a file compiled without debugging.
```

And now we can build our payload:

```shell
./level6 $(python -c 'print "a" * 72 + "\x08\x04\x84\x54"[::-1]')
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```
