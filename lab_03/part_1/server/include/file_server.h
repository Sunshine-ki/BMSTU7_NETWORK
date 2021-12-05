#pragma once
#include "server.h"

class FileServer : public Server {
public:
    FileServer(const int port) : Server(port) {}
    bool handleRequest(std::vector<char> &msg, const size_t bytes, const int fd) override;
};
