#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char	*auth;
char	*service;

int		main(void)
{
	char 	user_input[5];
	char 	local_8b[2];
	char 	acStack137[125];

	while (42) {
		printf("%p, %p \n", auth, service);
		if (!fgets(user_input, 128, stdin))
			return (0);

		if (!strncmp("auth ", user_input, 5)) {
			auth = (char *)malloc(4);
			*auth = 0;
			if (strlen(local_8b) < 31)
				strcpy((char *)auth, local_8b);
		}

		if (!strncmp("reset", user_input, 5))
			free(auth);

		if (!strncmp("service", user_input, 6))
			service = strdup(acStack137);

		if (!strncmp("login", user_input, 5)) {
			if (auth[32] != 0)
				system("/bin/sh");
			else
				fwrite("Password:\n", 1, 10, stdout);
		}
	}
	return (0);
}
