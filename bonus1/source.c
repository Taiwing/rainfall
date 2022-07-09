#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char	buffer[40];
	int		n;

	n = atoi(argv[1]);
	if (n < 10) {
		memcpy(buffer, argv[2], n * 4);
		if (n == 0x574f4c46)
			execl("/bin/sh", "sh", 0);
		return (0);
	}
	return (1);
}

