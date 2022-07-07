#include <stdio.h>
#include <string.h>
#include <unistd.h>

void	p(char *dest, char *separator)
{
	char	*new_line;
	char 	user_input[4104];

	puts(separator);
	read(0, user_input, 4096);
	new_line = strchr(user_input, '\n');
	*new_line = '\0';
	strncpy(dest, user_input, 20);
}

void	pp(char *dest)
{
	char 	input_1[20];
	char 	input_2[20];

	p(input_1, " - ");
	p(input_2, " - ");
	strcpy(dest, input_1);
	dest[strlen(dest)] = ' ';
	strcat(dest, input_2);
}

int		main(void)
{
	char	buffer[54];

	pp(buffer);
	puts(buffer);
	return (0);
}
