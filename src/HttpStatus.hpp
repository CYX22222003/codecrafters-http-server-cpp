#pragma once
#include <string>

namespace HttpStatus {
    std::string OK = "HTTP/1.1 200 OK\r\n";
    std::string NotFound = "HTTP/1.1 404 Not Found\r\n";
}