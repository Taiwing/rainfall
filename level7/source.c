#include <time.h>
#include <stdio.h>
#include <stdlib.h>

char	*c;

void	m(int param_1, int param_2, int param_3, int param_4, int param_5)
{
	time_t	now;

	now = time((time_t *)0x0);
	printf("%s - %d\n", c, now);
}

int		main(int argc, char **argv)
{
	int		*alloc1;
	int		*alloc2;
	FILE	*stream;

	alloc1 = (int *)malloc(8);
	alloc1[0] = 1;
	alloc1[1] = malloc(8);
	alloc2 = (int *)malloc(8);
	alloc2[0] = 2;
	alloc2[1] = malloc(8);
	strcpy((char *)alloc1[1], argv[1]);
	strcpy((char *)alloc2[1], argv[2]);
	stream = fopen("/home/user/level8/.pass", "r");
	fgets(c, 0x44, stream);
	puts("~~");
	return (0);
}
