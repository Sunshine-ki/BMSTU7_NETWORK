#include "file_server.h"
#include <experimental/filesystem>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <boost/algorithm/string.hpp>

const char* ws = " \t\n\r\f\v";

inline std::string& rtrim(std::string& s, const char* t = ws) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

inline std::string& ltrim(std::string& s, const char* t = ws) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

inline std::string& trim(std::string& s, const char* t = ws) {
    return ltrim(rtrim(s, t), t);
}

bool FileServer::handleRequest(std::vector<char> &msg, const size_t bytes, const int fd) {
    auto req_msg = std::string(msg.begin(), msg.end());
    std::ostringstream response;
    auto files_list_condition = req_msg.substr(0, 2) == "ls";
    if (files_list_condition) {
        std::string path = std::experimental::filesystem::current_path();
        for (const auto & entry : std::experimental::filesystem::directory_iterator(path)) {
            if (!std::experimental::filesystem::is_regular_file(entry))
                continue;
            std::string filename = std::experimental::filesystem::path(entry);
            response << filename.substr(filename.rfind('/') + 1) << std::endl;
        }
    } else {
        req_msg = trim(req_msg);
        std::cout << "Filename: '" << req_msg << "'\n";
        std::ifstream fs(req_msg);
        std::copy(std::istreambuf_iterator<char>(fs),
                std::istreambuf_iterator<char>(),
                std::ostreambuf_iterator<char>(response));
    }

    std::string response_string = response.str();
    if (send(fd, response_string.data(), response_string.size(), 0) == -1)
        throw std::runtime_error("Can't send response to client");

    if (!files_list_condition) {
        close(fd);
        return false;
    }

    return true;
}
