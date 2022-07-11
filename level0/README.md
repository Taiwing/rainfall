## Pass simple condition (level0)

Here we have a binary that will execute a shell as the next user if a condition
is passed. The level0 [binary](source.c) takes an argument, runs atoi() on it
and compares it to a constant _0x1a7_ (which is _423_ in decimal).

```shell
# execute the binary with the right value
./level0 423
# the shell is yours:
$ cat /home/user/level1/.pass
```
