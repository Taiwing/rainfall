#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

void p(void)
{
	char buffer [76];	// $ebp - 0x4c

	fflush(stdout);
	gets(buffer);
	// The return address is just before the buffer on the stack
	// so at buffer + len(buffer)
	if ((*(uint32_t *)(buffer + 76) & 0xb0000000) == 0xb0000000) {
		printf("(%p)\n", (void *)*(uint32_t *)(buffer + 76));
		exit(1);
	}
	puts(buffer);
	strdup(buffer);
	return;
}

int	main(void)
{
	p();
	return (0);
}
