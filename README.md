# rainfall

This is a CTF made in 42. The flags, you gotta catch them all.

<br />
<p align="center">
	<img src="https://github.com/Taiwing/rainfall/blob/master/resources/rain.jpeg?raw=true" alt="city-rain" style="width: 50%;"/>
</p>

Each level in the virtual machine contains an executable binary owned by the
next level user with the [setuid bit](https://en.wikipedia.org/wiki/Setuid) on.
There is also a _.pass_ file which contains the password of the current level
(except for the first one which is used to connect to the vm). The goal is to
reverse engineer each binary and find a way to get access to the password file
of the next level which will be the flag for the current level. This is
generally done by exploiting some insecure function calls and unchecked user
input.

## Setup

```shell
# launch the rainfall vm with qemu
./vm_lauch.bash
# connect to it with ssh (pass: level0)
ssh -p 2222 level0@localhost
```

> The vm\_launch.bash script takes an optional parameter for the ISO path.

## About this repo

Every level is documented in its own sub-directory which contains at least a
README.md file that is a walkthrough to the solution and and source file which
is a decompiled version of the level's binary. Other files may be included too
if they were used for the exploit or to find the solution. There is also a
[shellcode](shellcode) directory which is a an introduction to shellcode
writing and contains some helper tools to facilitate the process.
