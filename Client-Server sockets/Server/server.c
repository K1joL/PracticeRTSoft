#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "my_driverIOCTL.h"

int main()
{
	int fd = 0;
	const char* path = "/dev/kij";
	fd = open(path, fd, O_RDWR);
	if(fd < 0)
		return -1;
		
	int counter_value = 0;
	
	int sock;
	struct sockaddr_in server_addr, client_addr ;
	const int buf_size = 1;
	int buf_from[buf_size], buf_to[buf_size];
	int bytes_read;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0)
	{
		perror("socket");
		return -3;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(3232);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind");
		return -4;
	}
	
	int client_address_len = sizeof(struct sockaddr_in);
	while(1)
	{
		//printf("recv:\n");
		bytes_read = recvfrom(sock, buf_from, buf_size, 0,  
		(struct sockaddr*)&client_addr,
		  &client_address_len);
		//printf("received: from client %d, addr: %x\n", bytes_read, 
		//client_addr.sin_port);
		
		if(bytes_read > 0)
			if(ioctl(fd, IOC_READC, &counter_value))
				return -2;
		buf_to[0] = counter_value;
		//printf("c = %d, buf[0] = %d\n", counter_value, buf_to[0]);
		//printf("send:\n");
		//printf("sendto returned = %d\n",
		sendto(sock, buf_to, buf_size, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
	}
	return 0;
}

