#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include "IOCTL.h"

int main()
{
	int fd = 0;
	const char* path = "/dev/MyPCI";
	fd = open(path, fd, O_RDWR);
	if(fd < 0)
	{
		printf("failed to open");
		return -1;
	}
	int mac[6];
	if(ioctl(fd, IOC_GETMAC, mac))
	{
		printf("failed to cmd");
		return -2;
	}

	int i = 0;
	while (i < 6)
	{
		printf("[%d] = %02x", i, (mac[i]));
		i++;
	}

	close(fd);
	return 0;
}

