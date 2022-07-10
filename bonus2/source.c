#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int		language;

void	greetuser(char *user_input)
{
	char	greeting[72];

	if (language == 1)
		strcpy(greeting, "Hyvää päivää ");
	else if (language == 2)
		strcpy(greeting, "Goedemiddag! ");
	else if (language == 0)
		strcpy(greeting, "Hello ");
	strcat(greeting, user_input);
	puts(greeting);
}

int		main(int argc, char **argv)
{
	char	*locale;
	char	buffer[156];

	if (argc == 3) {
		bzero(buffer + 80, 19);
		strncpy(buffer + 80, argv[1], 40);
		strncpy(buffer + 120, argv[2], 32);
		if ((locale = getenv("LANG")) != NULL) {
			if (!memcmp(locale, "fi", 2))
				language = 1;
			else if (!memcmp(locale, "nl", 2))
				language = 2;
		}
		// THIS THE THING: the memcpy is not on 19 bytes but 19 words!
		// That explains the overflow in the greet_user function.
		memcpy(buffer, buffer + 80, 19 * 4);	// 19 * 4 = 76 bytes
		greetuser(buffer);
		return (0);
	}
	return (1);
}
