#pragma once

#include <map>

#define MSG_LEN 512
#define SRV_IP "127.0.0.1"
#define SOCK_PORT 5007
#define THREADS 16
#define BUFFER_SIZE 1024 * 10
#define FOLDER_MAX 1024
#define STATISTICS_FILE_NAME "statistics.txt"

// codes
#define OK 200
#define BAD_REQUEST 400
#define FORBIDDEN  403
#define NOT_FOUND  404
#define NOT_ALLOWED  405

std::map<std::string, std::string> TYPES = 
{
    {"txt", "text/plain"},
    {"html", "text/html"},
    {"jpg", "image/jpeg"},
    {"png", "image/png"},
    {"jpeg", "image/jpeg"},
    {"gif", "image/gif"},
    {"swf", "application/x-shockwave-flash"},
    {"css", "text/css"},
    {"js", "application/javascript"},
};

std::map<int, std::string> codes = 
{   
    {OK, "200 OK"},
    {BAD_REQUEST, "400 Bad Request"},
    {FORBIDDEN, "403 Forbidden"},
    {NOT_FOUND,"404 Not Found"},
    {NOT_ALLOWED, "405 Method Not Allowed"}
};

// methods 
#define HEAD "HEAD"
#define GET "GET"

// Colors
#define red() printf("\33[31m")
#define green() printf("\33[32m")
#define yellow() printf("\33[33m")
#define blue() printf("\33[34m")
#define white() printf("\33[34m")
// #define white() printf("\33[37m")
#define blinks() printf("\33[5m");

#define SUCCESS 0