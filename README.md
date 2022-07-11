# rainfall

This is a CTF made in 42. The flags, you gotta catch them all.

## Setup

```shell
# launch the rainfall vm with qemu
./vm_lauch.bash
# connect to it with ssh (pass: level00)
ssh -p 2222 level00@localhost
```

> The vm\_launch.bash script takes an optional parameter for the ISO path.

## Content

Each level contains a binary owned by the next level user with the setuid bit
on and also a _.pass_ file that contains the flag of the current level (which is
also, as the name indicates, the password for the next level). Every level is
documented in its own sub-directory which contains at least a README.md file
that is walkthrough to the solution and and source file which is a decompiled
version of the level's binary. Other files may be included too if they were used
for the exploit or to find the solution.
