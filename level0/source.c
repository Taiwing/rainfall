#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
	uid_t	uid;
	gid_t	gid;

	if (atoi(argv[1]) == 0x1a7) {
		char	*args[2] = {
			strdup("/bin/sh"),
			NULL,
		};
		gid = getegid();
		uid = geteuid();
		setresgid(gid, gid, gid);
		setresuid(uid, uid, uid);
		execv("/bin/sh", args);
	}
	else
		fwrite("No !\n", 1, 5, stderr);
	return 0;
}

