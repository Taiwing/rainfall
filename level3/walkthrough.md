## Press printf's f for fun and profit (level3)

This level's [binary](level3/source.c) is fairly similar to the last one. It
takes user input and prints it back to stdin:

```shell
# run the binary
./level3
# give it some input
blablabla
# get our input
blablabla
```

However, this time fgets is used to get user input and the buffer length is
valid so the input itself is protected. Although now it's not puts that is used
to print back the input but printf, directly on the input buffer. This means
that we can feed printf format string directly into this binary which allows us
to perform a
[format string attack](https://infosecwriteups.com/exploiting-format-string-vulnerability-97e3d588da1b).

After analysis of the binary through ghidra and gdb, we can see that there is an
unitialized global variable _m_ (thus living in the .bss part of our program's
memory). Since it is not explicitely given a value by the user, it will be set
to 0. This variable is only used once. It is compared to 0x40 (so if it is equal
to 64). If it is indeed equal to this value the binary will give us a shell. So
the goal here is to find a way to set _m_ to 64 through the printf call.

First let's get _m_'s address:

```shell
# run gdb on the binary
gdb ./level3
# get the address
(gdb) i addr m
Symbol "m" is at 0x804988c in a file compiled without debugging.
```
