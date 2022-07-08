## Don't forget, sentences end with a point and strings with a '\0' (bonus0)

With this [binary](bonus0/source.c) we're back to good old C and classic buffer
overflows. There's three functions including _main_. The _main_ has a local
buffer of 54 bytes, and calls the function _pp_ on it. There we have two other
buffers each 20 bytes long. _pp_ will call the third function _p_ on both to
fill them with some user input. After that the two buffers are copied and
insecurely concatenated in the buffer provided by the _main_ function.

After some gdb analysis, we can see the state of the stack from _p_:

# TODO: remove the "shell quotes" and use html to set the background of the
# different memory zones:

* 0xbffffa88 -> 0xbffffaa0 (24 bytes): end of _p_'s big read buffer (all "0x61")
* 0xbffffaa8 (4 bytes): 0xbffffb08 _pp_'s ebp
* 0xbffffaac (4 bytes): 0x0804854c _p_'s return address <_pp_+46>
* 0xbffffad8 -> 0xbffffaec (20 bytes): _pp_'s first buffer (all "0x61")
* 0xbffffaec -> 0xbffffb00 (20 bytes): _pp_'s second buffer (all "0x62")
* 0xbffffb08 (4 bytes): 0xbffffb58 _main_'s ebp
* 0xbffffb0c (4 bytes): 0x080485b9 _pp_'s return address <_main_+21>
# TODO: find out where main's buffer starts and get a picture of what it looks
# like when it is filled by continuing to step in gdb
* somewhere here there's _main_'s buffer
* 0xbffffb58 (4 bytes): 0x00000000 *\_\_libc\_start\_main*'s ebp ? (look that up)
* 0xbffffb5c (4 bytes): 0xb7e454d3 _main_'s return address <*\_\_libc\_start\_main*+243>

```shell
(gdb) x/56wx 0xbffffa88
0xbffffa88:     0x61616161      0x61616161      0x61616161      0x61616161
0xbffffa98:     0x61616161      0x00616161      0x00000001      0x00000000
0xbffffaa8:     0xbffffb08      0x0804854c      0xbffffaec      0x080486a0
0xbffffab8:     0x00000000      0x00000001      0x00000000      0x00000000
0xbffffac8:     0x00000000      0x00000000      0x00000000      0x00000000
0xbffffad8:     0x61616161      0x61616161      0x61616161      0x61616161
0xbffffae8:     0x61616161      0x62626262      0x62626262      0x62626262
0xbffffaf8:     0x62626262      0x00626262      0xb7fd0ff4      0x00000000
0xbffffb08:     0xbffffb58      0x080485b9      0xbffffb26      0x080498d8
0xbffffb18:     0x00000001      0x0804835d      0xb7fd13e4      0x00080000
0xbffffb28:     0x080498d8      0x080485f1      0xffffffff      0xb7e5edc6
0xbffffb38:     0xb7fd0ff4      0xb7e5ee55      0xb7fed280      0x00000000
0xbffffb48:     0x080485d9      0xb7fd0ff4      0x080485d0      0x00000000
0xbffffb58:     0x00000000      0xb7e454d3      0x00000001      0xbffffbf4
```

# TODO: after adding the nice colors explain a little bit how we got there, both
# inputs (the first "a" * 4095 and the second "b" * 19)
