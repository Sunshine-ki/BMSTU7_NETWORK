#include "server.h"

std::ostream & operator<<(std::ostream & out, const std::vector<char> & s) {
    for (const auto & c : s)
        out << c;
    return out;
}

Server::Server(const int port) {
    listen_socket_fd = Server::socket();
    struct sockaddr_in addr = Server::buildAddr(port);
    Server::bind(listen_socket_fd, addr);
    Server::listen(listen_socket_fd);
    std::cout << "Server is running on " << inet_ntoa(addr.sin_addr) << ':' 
        << ntohs(addr.sin_port) << '\n';
}

int Server::buildFdSet(fd_set & readfds) {
    FD_ZERO(&readfds);
    FD_SET(listen_socket_fd, &readfds);
    int max_fd = listen_socket_fd;
    for (const auto & fd : client_fds) {
        FD_SET(fd, &readfds);
        if (fd > max_fd)
          max_fd = fd;
    }
    return max_fd;
}

void Server::processNewConnection() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    const int client_fd = accept(listen_socket_fd, (struct sockaddr*) &addr, &len);
    if (client_fd == -1) {
        perror("accept call error");
        throw std::runtime_error("accept call error");
    }
    printf("New connect from %s:%d\n", inet_ntoa(addr.sin_addr), addr.sin_port);
    client_fds.insert(client_fd);
}

bool Server::processRawRequest(const int fd) {
    std::vector<char> msg(2 * 1024 * 1024, 0);
    size_t bytes = recv(fd, msg.data(), msg.size(), 0);
    if (!bytes) {
        std::cout << "client (with fd = " << fd << ") disconnected\n";
        close(fd);
        return false;
    } else {
        return handleRequest(msg, bytes, fd);
    }
}

void Server::start_processing() {
    fd_set readfds;
    for (;;) {
        int max_fd = buildFdSet(readfds);

        if (pselect(max_fd + 1, &readfds, NULL, NULL, NULL, NULL) == -1) {
            perror("select error");
            throw std::runtime_error("pselect call error");
        }

        if (FD_ISSET(listen_socket_fd, &readfds))
            processNewConnection();

        auto it = client_fds.begin();
        while (it != client_fds.end()) {
            auto & fd = *it;
            if (fd && FD_ISSET(fd, &readfds) && !processRawRequest(fd))
                it = client_fds.erase(it);
            else
                it++;
        }
    }
}

int Server::socket() {
    int fd;
    if ((fd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        throw std::runtime_error("failed socket call");
    return fd;
}

struct sockaddr_in Server::buildAddr(const int port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    return addr;
}

void Server::bind(const int listen_socket_fd, const struct sockaddr_in & addr) {
    if (::bind(listen_socket_fd, (struct sockaddr*)&addr, sizeof addr) == -1)
        throw std::runtime_error("bind call error");

}

Server::~Server() {
    if (listen_socket_fd != -1)
        close(listen_socket_fd);
    for (auto & fd : client_fds)
        close(fd);
    std::cout << "Server is down\n";
}

void Server::listen(const int listen_fd) {
    if (::listen(listen_fd, SOMAXCONN) == -1)
        throw std::runtime_error("listen failed");
}
