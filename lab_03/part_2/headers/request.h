#pragma once

#include <string>
#include <map>
#include <cstring>
#include <string.h>
#include <iostream>

class Request 
{
private:
    std::string _method, _url, _version;
    std::map<std::string, std::string> _headers;

    bool _is_success;

public :
    std::map<std::string, std::string> get_headers(); 
    std::string get_method();
    std::string get_version(); 
    std::string get_url();
    bool is_success();

    Request(char* req);
};

// Get методы.
std::string Request::get_method() 
{
    return this->_method;
}

std::string Request::get_url() 
{
    return this->_url;
}

std::string Request::get_version() 
{
    return this->_version;
}

std::map<std::string, std::string> Request::get_headers() 
{
    return this->_headers;
}

// 

bool Request::is_success() 
{
    return this->_is_success;
}


// Инициализирует свои поля в завсимости от запроса req. 
// (Парсит строку req)
Request::Request(char* req) 
{
    // Метод
    char *elems[4];
    elems[0] = strtok(req, " \r\n");
    if (!elems[0]) 
    {
        _is_success = false;
        return;
    }
    this->_method = elems[0];

    // Путь к ресурсу.
    elems[1] = strtok(NULL, " \r\n");
    if (!elems[1]) 
    {
        _is_success = false;
        return;
    }
    this->_url = elems[1];
    
    // Версия 
    elems[2] = strtok(NULL, " \r\n");
    if (!elems[2]) 
    {
        _is_success = false;
        return;
    }
    this->_version = elems[2];
    
    // Заголовки.
    while ((elems[3] = strtok(NULL, "\r\n"))) 
    {
        std::string str = elems[3];
        int pos = str.find(": ");
        
        if (pos == std::string::npos)
        {
            _is_success = false;
            return;
        }

        std::string token;
        token = str.substr(0, pos);
        str.erase(0, pos + 2);
        
        this->_headers[token] = str;
    }

    _is_success = true;

    // std::cout << "is_success" << '\n';
    return;
}
