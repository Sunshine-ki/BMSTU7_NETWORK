#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

#include <iostream>

#include <string>
#include <cstdlib>

#include "../headers/constants.h"

// Создание GET запроса
std::string create_request(std::string url) 
{
    std::string req = "GET " + url + " HTTP/1.1\r\n";
    req += "Host: localhost:" + std::to_string(SOCK_PORT) + "\r\n\r\n"; 
    
    return req;
}

int main(void)
{
    char buf[BUFFER_SIZE];
    const char *message;
    std::string file;

    // Создаем сокет.
    int client_sock = socket(AF_INET,  SOCK_STREAM, IPPROTO_TCP);
    if (client_sock < 0)
    {
        perror("socket error");
        exit(client_sock);
    }

    struct hostent* host = gethostbyname("localhost");
    if (!host)
    {
        perror("gethostbyname error");
        return -1;
    }

    // Заполняем информацию о Сервере.
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    server_addr.sin_addr = *((struct in_addr*) host->h_addr_list[0]);

    // Инициируем подключение к сокету.
    if (connect(client_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect error");
        return -1;
    }

    green();
    std::cout << "Please, input file name for HTTP GET: \n";
    white();
    std::cin >> file;

    std::string mes =  create_request(file);
    message = mes.c_str();

    // Отправляем сообщение.
    sendto(client_sock, message, strlen(message), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));

    unsigned int server_addr_len = sizeof(server_addr);
    // Получаем сообщение из сокета.
    if (recvfrom(client_sock, buf, BUFFER_SIZE, 0, (struct sockaddr*) &server_addr, &server_addr_len) == -1) 
    {
        perror("recvfrom()");
        exit(1);
    }
    
    std::cout << buf;
    
    close(client_sock);

    return SUCCESS;
}
