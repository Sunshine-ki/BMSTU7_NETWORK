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

void bin_print(unsigned n) 
{
    if (n) 
	{
        bin_print(n >> 1);
        printf("%u", n & 1);
    }
}

void output(int number)
{
	printf("\nClient sent: %d\n", number);
	printf("2: ");
	bin_print(number);
	printf("\n8: %o\n", number);
	printf("16: %x\n", number);
	// А у меня 16 номер по списку...
}

int main(void)
{
	setbuf(stdout, NULL);

	// Структура специально предназначенная
	// для хранения адресов в формате Интернета.
	struct sockaddr_in serv_addr =
	{
		.sin_family = AF_INET,
		// INADDR_ANY - зарегистрировать нашу программу-сервер
		// на всех адресах машини, на которой она выполняется.
		.sin_addr.s_addr = INADDR_ANY,
		// htons() переписывает двухбайтовое значение порта так,
		// чтобы порядок байтов соответствовал сетевому.
		.sin_port = htons(PORT)
	};
	struct sockaddr_in cli_addr;

	// SOCK_DGRAM for UDP 
	// 0 протокол по умолчанию
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket < 0)
	{
		printf("socket() failed: %d\n", errno);
		return ERROR_CREATE_SOCKET;
	}

	// bind() - связывает сокет с заданным адресом.
	// После вызова bind() программа-сервер становится доступна для соединения по заданному адресу (имени файла)
	if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("bind() failed: %d\n", errno);
		close(sock);
		return ERROR_BIND_SOCKET;
	}

	int number;
	int len = sizeof(cli_addr);
	recvfrom(sock, &number, sizeof(int), MSG_WAITALL, ( struct sockaddr *) &cli_addr, &len);
	output(number);

	close(sock);

	return SECCESS;
}