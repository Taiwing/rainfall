#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t	m;

void	p(char *format)
{
	printf(format);
}

void	n(void)
{
	char	buffer[520];

	fgets(buffer, 0x200, stdin);
	p(buffer);
	if (m == 0x1025544)
		system("/bin/cat /home/user/level5/.pass");
}


int		main(void)
{
	n();
	return (0);
}
