## Press printf's f for fun and profit episode III (level5)

This level's [binary](level5/source.c) still does the same thing as the last
ones. It takes user input and prints it back to stdin. The input buffer is also
directly given to printf, which means we can once again perform a
[format string attack](https://infosecwriteups.com/exploiting-format-string-vulnerability-97e3d588da1b).
However this time there is no _m_ value to be set. All we have in our function
_n_, after our insecure printf call, is an exit. There is also an other
function, _o_, declared in the binary which calls system and spawns a shell.

We must obviously find a way to call the _o_ function. The only entry point we
have after the printf call is - ironically - our exit call. The call instruction
refers to an address where the function resides. If we could rewrite the call
instruction to point to _o_ or to make the exit address point to it, this would
do the trick.

One way to do this is through the
[got overwrite](https://infosecwriteups.com/got-overwrite-bb9ff5414628) exploit.
