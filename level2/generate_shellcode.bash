#!/bin/bash

# takes a compiled binary as input
if [ -z $1 ]; then
	echo "usage: $0 <file>"
	exit 1
fi

for byte in $(objdump -d $1 | grep "^ " | cut -f2); do
	echo -n '\x'$byte;
done
echo
