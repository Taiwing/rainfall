#!/bin/bash

[ -z $1 ] && echo "usage: $0 <source_file.asm>" && exit 1

FILE=$(basename $1 .asm)

nasm -f elf $FILE.asm
ld -m elf_i386 -s -o $FILE $FILE.o
