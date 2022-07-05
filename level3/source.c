#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Uninitialzed global variable in bss (set to 0)
uint32_t	m;

void	v(void)
{
	char	buffer[520];

	fgets(buffer, 0x200, stdin);
	printf(buffer);
	// Checking if m's value has been overwritten
	if (m == 0x40) {
		fwrite("Wait what?!\n", 1, 0xc, stdout);
		system("/bin/sh");
	}
	return;
}

int		main(void)
{
	v();
	return (0);
}
