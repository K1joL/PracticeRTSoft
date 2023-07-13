#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
	int fd;
	char *buf;
	char *path = "/dev/kij";
	printf("Path: ");
	//scanf("%s", path);
	fd = open(path, O_RDWR);
	if(fd < 0)
	{
		printf("Cant open file %s! \n", path);
		return -1;
	}
	//printf("buf: ");
	//scanf("%s", buf);
	int length = 5;
	buf = "hello";
	//printf("length: ");
	//scanf("%d", &length);
	printf("buf = %s, legnth = %d \n", buf, length);
	
	write(fd, buf, length);
	close(fd);
}
