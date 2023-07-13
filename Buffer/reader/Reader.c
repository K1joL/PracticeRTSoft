#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
	int fd;
	char buf[100];
	char *path = "/dev/kij";
	printf("Path: ");
	// scanf("%s", path);

	fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		printf("Cant open file %s! \n", path);
		return -1;
	}
	read(fd, buf, 5);
	buf[13] = 0;
	printf("Input: %s \n", buf);
	close(fd);
}
