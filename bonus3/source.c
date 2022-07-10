#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

int main(int argc, char **argv)
{
	int		index;
	char	buffer_1[16];
	char	buffer_2[67];
	FILE	*stream;

	stream = fopen("/home/user/end/.pass","r");
	bzero(buffer_1, 0x21);
	if (!stream || argc != 2)
		return (-1);
	fread(buffer_1, 1, 0x42, stream);
	buffer_2[66] = 0;
	index = atoi(argv[1]);
	buffer_1[index] = 0;
	fread(buffer_2, 1, 0x41, stream);
	fclose(stream);
	if (!strcmp(buffer_1, argv[1]))
		execl("/bin/sh", "sh", 0);
	else
		puts(buffer_2);
	return (0);
}

