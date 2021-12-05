#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <fstream>

#define SOCKET_PORT 3001
#define BUF_SIZE 100 * 1024 * 1024

int performQuery(const int sfd, char * msg, bool till_end = false) {
    if (send(sfd, msg, strlen(msg), 0) == -1)
        throw std::runtime_error("send call error");

    size_t s = 0;
    do {
        auto bytes = recv(sfd, msg + s, BUF_SIZE, 0);
        if (bytes < 0) {
            perror("recv?");
            return 0;
        }
        else if (bytes == 0)
            break;
        else
            s += bytes;
    } while(till_end);
    return s;
}

int main() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket call error");
        return errno;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(SOCKET_PORT);
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

    if (connect(sfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("connect call error");

    static char msg[BUF_SIZE] = {0};
    strcpy(msg, "ls");
    performQuery(sfd, msg);
    std::cout << "FILES:\n" << msg << std::endl;

    std::cout << "\n\nEnter file to download: ";
    std::cin >> msg;
    std::string filename = msg;
    auto bytes = performQuery(sfd, msg, true);
    std::cout << "Enter output file name: ";
    std::cin >> filename;
    std::ofstream f("downloads/" + filename, std::ios::binary);
    for (int i = 0; i < bytes; i++)
        f << msg[i];
    return 0;
}
