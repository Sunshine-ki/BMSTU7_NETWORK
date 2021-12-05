#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>
#include <set>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <iostream>

class Server {
    int listen_socket_fd = -1;
    std::set<int> client_fds;
public:
    Server(const int port);
    void start_processing();
    ~Server();
protected:
    static struct sockaddr_in buildAddr(const int port);
    static int socket();
    static void bind(const int listen_socket_fd, const struct sockaddr_in & addr);
    static void listen(const int listen_fd);
    int buildFdSet(fd_set & readfds);
    void processNewConnection();
    virtual bool processRawRequest(const int fd);
    virtual bool handleRequest(std::vector<char> & msg, const size_t bytes, const int fd) = 0;
};
