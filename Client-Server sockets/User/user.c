#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>

#define BILLION 1000000000L
#define SIZE_HISTGRM 50
#define STEP_HISTGRM 100

void printHistogramm(int *data, int N, int step)
{
	int n,j,i,x;
	printf("\n");
	//printf("         |\n");
	for (n = 0; n < N; ++n)
	{
		for (i = 1; i <= 1; i++)
		{
			if (i == 2)
				printf("Group- %d |", n + 1);
			else
				printf("         |");
			if(data[n] == 0)
				printf("(), value: %d\n", step*(n+1));
			else
				printf("(%d), value: %d\n", data[n], step*(n+1));
		}
		//printf("         |\n");
	}
}

void fillHistogramm(int *data, int size, int nanosec, int step)
{
	//Проверяем до предпоследнего значения в гистограмме
	for (int mcsec = 0; mcsec < size-2; mcsec++)
	{
		if (nanosec / 1000 < (mcsec + 1) * step)
		{
			data[mcsec]++;
			return;
		}
	}
	data[size-1]++;
}
int main()
{
	struct timespec start, end;
	int histogramm[SIZE_HISTGRM];
	for (int k = 0; k < SIZE_HISTGRM; k++)
		histogramm[k] = 0;
		
	int counter_value = 0;
	
	int sock;
	struct sockaddr_in addr;
	const int buf_size = 1;
	int buf_from[buf_size], buf_to[buf_size];
	int bytes_read;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0)
	{
		perror("socket");
		return -3;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3232);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	unsigned long diff = 0;
	int i = 0;
	buf_to[0] = 1;
	while(i < 10000)
	{
		clock_gettime(CLOCK_MONOTONIC, &start);
		sendto(sock, buf_to, buf_size, 0, (struct sockaddr*)&addr, sizeof(addr));
		bytes_read = recvfrom(sock, buf_from, buf_size, 0, NULL, NULL);
		clock_gettime(CLOCK_MONOTONIC, &end);
		if(bytes_read > 0)
		{
			counter_value = buf_from[0];
			diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
			//printf("c = %d, time: %lu nanoseconds\n", counter_value, diff);
			fillHistogramm(histogramm, SIZE_HISTGRM, diff, STEP_HISTGRM);
		}
		i++;
	}
	
	printHistogramm(histogramm, SIZE_HISTGRM, STEP_HISTGRM);

	close(sock);
	return 0;
}

