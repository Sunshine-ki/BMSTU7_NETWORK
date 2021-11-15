#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>
#include <string>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <signal.h>

#include "../headers/threads.h"
#include "../headers/response.h"
#include "../headers/request.h"
#include "../headers/constants.h"

// Считывает информацию из input в readed_data 
void read_file(std::vector<char> &readed_data, FILE* input) 
{
    unsigned char byte;
    readed_data.clear();
    while (fread(&byte, sizeof(char), 1, input)) 
    {
        readed_data.push_back(byte);
    };
}

// Возвращает путь к дирректории из которой выполняется программа.
std::string get_folder()
{
    char arg1[20];
    char folder[FOLDER_MAX + 1] = {0};
    sprintf(arg1, "/proc/self/cwd");
    readlink(arg1, folder, 1024);
    return std::string(folder);
}

// Возвращает полный путь к файлу.
// Добавляет к root url, отбрасывая из url параметры запроса, которые идут после "?"
std::string parse_url(const std::string url, const std::string root) 
{
    std::string new_url(url);
    int pos = new_url.find("?");
    if (pos != std::string::npos) {
        new_url = new_url.substr(0, pos);
    }
    std::string path = root + new_url;
    return path;
}

// Возвращает тип контента.
std::string get_content_type(std::string path)
{
    int dotpos = path.rfind('.');
    std::string type = path;
    type.erase(0, dotpos+1);
    auto cont_type = TYPES[type];

    if (cont_type == "") 
    {
        cont_type = "text/plain";
    }

    return cont_type;
}

// Возвращает результат запроса Request req.
// root - папка из которой необходимо получить файлы.
std::string create_res(Request &req, const std::string root) 
{
    std::vector<char> data;
    
    if (!req.is_success()) 
    {
        Response res(BAD_REQUEST);
        return res.get_string("", 0, data);
    }

    // Допускаются только get (или head) запросы.  
    if (req.get_method() != GET && req.get_method() != HEAD) 
    {
        Response res(NOT_ALLOWED);
        return res.get_string("",0, data);
    }

    // Получаем путь к файлу, который хочет получить клиент.
    std::string path = parse_url(req.get_url(), root);

    // Клиенту разрешен доступ только к файлам,
    // Которые находятся в той же директории (или ниже), что и server
    // Если путь содержит "../", значит пользователь хочет получить доступ к файлу,
    // Который находится уже на другом уровне (на уровень выше).
    if (path.find("../") != std::string::npos) 
    {
        // Поэтому мы возвращаем ему FORBIDDEN.
        Response res(FORBIDDEN);
        return res.get_string("", 0, data);
    }

    // Если пользователь хочет получить доступ к корню ("/")
    // То возвращаем ему index.html
    bool is_dir = false;
    struct stat s;
    if( stat(path.c_str(),&s) == 0 )
    {
        if( s.st_mode & S_IFDIR )
        {
            path += "index.html";
            is_dir = true;
        }
    }

    FILE *f = fopen(path.c_str(), "rb");

    // Если не получилось открыть файл, значит либо:
    // 1. клиент ввел неверный путь;
    // 2. клиент хочет получить доступ к папке (если установлен флаг is_dir).
    if (!f) 
    {
        if (is_dir) 
        {
            Response res(FORBIDDEN);
            return res.get_string("",0, data);
        } 
        else 
        {
            Response res(NOT_FOUND);
            return res.get_string("",0, data);
        }
    }

    Response res(OK);

    read_file(data, f);
    fclose(f);

    auto cont_type = get_content_type(path);

    // Если метод HEAD, то не возвращаем тело.
    if (req.get_method() == HEAD) 
    {
        std::vector<char> nulldata; 
        return res.get_string(cont_type, data.size(), nulldata);
    }
    return res.get_string(cont_type, data.size(), data);
}

void handler(int sock, std::string addr, std::string root) 
{
    int request;
    char buf[BUFFER_SIZE], *request_lines[3];
    char *file_name;

    // Читаем данные из сокета.
    request = recv(sock, buf, BUFFER_SIZE, 0);

    if (request < 0)
    {
        red();
        std::cout << "Recv failed";
        white();
        close(sock);
        return;
    }
    else if (request == 0)
    {
        yellow();
        std::cout << "Client disconnected unexpectedly.";
        white();
        close(sock);
        return;
    }

    green();
    std::cout << buf;
    white();

    Request req(buf);

    // TODO:
    // statistic
    
    std::string result = create_res(req, root);
    std::cout << result;  
    send(sock, result.c_str(), result.size(), 0);

    close(sock);
}

int sock;

void close_socket(void)
{
	close(sock);
    yellow();
    std::cout << "Socket closed\n";
    white();
}

void catch_sigint(int signum)
{
	close_socket();	
	exit(0);
}

int main(void)
{
    struct sockaddr_in server_addr, client_addr;
    unsigned int client_addr_len = sizeof(client_addr);
    int new_socket;
    char buf[MSG_LEN];

    green();
    std::cout << "Server started\n";
    white();

    // Создаем сокет.
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) 
    {
        red();
        perror("Socket error");
        white();
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Назначаем адрес server_addr сокету sock. 
    if (bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) 
    {
        close(sock);
        red();
        perror("bind");
        white();
        exit(1);
    }

    green();
    std::cout << "Bind ended\n";
    white();

    // Вешаем catch_sigint на обработку сигнала SIGINT,
    // Чтобы корректно закрывалось соединение при выходе из программы. 
    signal(SIGINT, catch_sigint);

    listen(sock, 1);
    green();
    std::cout << "Server is listening on " << SOCK_PORT << "\n";
    white();

    ThreadPool pool(THREADS);
    // TODO: statistic create here.

    auto folder = get_folder();

    // Ожидаем соединения на сокете sock.
    // Информация о клиенте запишется в client_addr.
    while ((new_socket = accept(sock, (struct sockaddr*) &client_addr, &client_addr_len))) 
    {
        // Преобразует ip-адрес клиента, заданный в сетевом порядке расположения байтов, в стандартный строчный вид.
            std::string ip =  inet_ntoa(client_addr.sin_addr);
            // auto port = std::to_string(ntohs(client_addr.sin_port));
            // Ставим обработку запроса в очередь пула потоков.
            pool.enqueue(handler, new_socket, ip, folder);
    }

    return SUCCESS;
}
