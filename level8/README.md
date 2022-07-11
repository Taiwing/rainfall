## Overcomplication shmimplificatrition (level8)

Ok so this level does really simple things in a very complicated way. In this
level's [source](source.c) you'll find the simplified version. For a trip to
hell just check the assembly with gdb or even the decompiled code with ghidra
(this level's instructions are so convoluted that even ghidra got some things
wrong here).

So what this program does is print two pointers, wait for user input, check it
against a list of possible commands and execute the corresponding one if the
input matches.

The `auth` command will allocate a 4 bytes memory zone and set the _auth_ global
variable to this address. Then it will copy the user input into it if it is more
than 30 bytes long.

`reset` will free the _auth_ global variable. Even if it is not initialized or
has already been freed.

`service` [strdup](https://linux.die.net/man/3/strdup)'s the user input starting
from the 7th byte regardless of its length. Then it writes the new pointer to
the global variable _service_, overwriting the precedent value if any.

Finally there is the `login` command. This one will simply check that the 32nd
byte after auth is equal to 0. If it is it will print some bullshit password
prompt, otherwise it will launch a shell.

So obviously we must find a way to set this byte to a non-zero value. We cannot
simply input a long enough string through the `auth` command since its length is
checked. Running this command multiple times will not work either since each
malloc calls returns an address located after the last one (the heap grows
upwards contrary to the stack), so our previous writes will simply be skipped
and auth + 32 will still point to a 0 byte. The `reset` command is useless, it's
the `service` that will do the trick.

The `service` command will also allocate memory at each call. All we have to do
is to call it once after the _auth_ pointer has been initialized so that
_service_'s address is after _auth_. Then we give it a long enough parameter to
overwrite the byte we want:

```shell
# run the binary
./level8
(nil), (nil)
# enter the first part of the payload and see the address of auth
auth 123
0x804a008, (nil)
# then the second part to set the magic byte
service aaaaaaaaaaaaaaa
0x804a008, 0x804a018
# and the shell is ours
login
$ whoami
level9
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```

The second payload only needs to be 17 bytes long to set the right byte since
the _service_ address is 0x804a018, so 16 bytes after _auth_. This is the case
here (counting the space and the newline character).
