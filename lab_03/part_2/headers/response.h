#pragma once
#include <string>
#include <map>

#include <vector>
#include <string>

#include "constants.h"

class Response
{
private:
    std::string status;
    std::string main;

public:
    Response(int status);
    std::string get_string(std::string cont_type, int cont_len, std::vector<char> data);
};

Response::Response(int status)
{
    this->status = codes[status];
    this->main = 
        ("HTTP/1.1 " + this->status + "\r\n" +
        "Connection: close\r\n" +
        "Server: localhost\r\n");
}

// Возвращает строку запроса.
std::string Response::get_string(std::string cont_type, int cont_len, std::vector<char> data)
{
    if (this->status != codes[OK]) 
    {
        return main;
    }

    this->main += 
        ("Content-Type: " + cont_type + "\r\n" +
        "Content-Length: " + std::to_string(cont_len) + "\r\n\r\n");

    for (int i =0; i < data.size(); i++) 
    {
        this->main += data[i];
    }

    return this->main;
}
