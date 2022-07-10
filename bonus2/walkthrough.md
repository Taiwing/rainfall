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
memory.
