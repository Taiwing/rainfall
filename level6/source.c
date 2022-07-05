#include <stdlib.h>
#include <string.h>

void	n(void)
{
	system("/bin/cat /home/user/level7/.pass");
}


void	m(void *param_1, int param_2, char *param_3, int param_4, int param_5)

{
	puts("Nope");
}

int		main(int argc, char **argv)

{
	char	*dest;
	void	(**fptr)(void);

	dest = (char *)malloc(0x40);
	fptr = malloc(4);
	*fptr = m;
	strcpy(dest, argv[1]);
	(**fptr)();
	return (0);
}

