#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include "my_driverIOCTL.h"

int main()
{
	int fd = 0;
	const char* path = "/dev/kij";
	fd = open(path, fd, O_RDWR);
	if(fd < 0)
		return -1;
	int counter_value = 0;
	int timeout_value = 2000;
	if(ioctl(fd, IOC_READC, &counter_value))
		return -2;
	printf("counter = %d\n", counter_value);
	if(ioctl(fd, IOC_CLEARC))
		return -2;
	if(ioctl(fd, IOC_READC, &counter_value))
		return -2;
	printf("counter = %d\n", counter_value);
	if(ioctl(fd, IOC_SET_TIMEOUT, &timeout_value))
		return -2;
	printf("counter = %d\n", counter_value);

	for(int i = 0; i < 10; i++)
	{
		if(ioctl(fd, IOC_READC, &counter_value))
			return -2;
		printf("counter = %d\n", counter_value);
	}

	close(fd);
	return 0;
}

