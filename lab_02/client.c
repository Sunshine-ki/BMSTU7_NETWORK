#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

#include "constants.h"

int main(void)
{
	setbuf(stdout, NULL);

	int number;

	struct sockaddr_in serv_addr =
	{
		.sin_family = AF_INET,
		// INADDR_ANY - все адреса локального хоста (0.0.0.0);
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(PORT)
	};

	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket < 0)
	{
		perror("socket() failed");
		return ERROR_CREATE_SOCKET;
	}

	printf(GREEN "Input number: ");
	printf(BLUE);
	scanf("%d", &number);

	if (sendto(sock, &number, sizeof(int), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("sendto() failed");
		close(sock);
		return ERROR_SENDTO_SOCKET;
	}

	close(sock);

	return SECCESS;
}