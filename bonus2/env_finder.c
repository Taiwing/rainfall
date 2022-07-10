#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int	len = 0;

	for (int i = 1; i < argc; ++i)
		len += strlen(argv[i]);
	printf("ran with %d arguments for a total len of %d\n", argc-1, len);
	printf("SHELL's address %p\n", getenv("SHELL"));
	return (0);
}
