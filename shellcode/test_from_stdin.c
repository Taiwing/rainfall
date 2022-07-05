#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char	buf[256];
	int	(*func)();

	if (read(0, buf, 256) < 0)
		exit(1);
	func = (int (*)())buf;
	func();
	return (0);
}
