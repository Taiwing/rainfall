#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	int	n = 0;

	if (argc > 1)
		n = atoi(argv[1]);
	else
		for (n = -1; n < 0 && n * 4 != 44; --n);
	printf("%d * 4 = %d\n", n, n * 4);
	return (0);
}
