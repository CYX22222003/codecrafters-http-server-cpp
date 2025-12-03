#pragma one
#include "./HttpStatus.hpp"
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <utility>

namespace HttpResponse {
    class HttpResponse {
        private:
            std::string status_line;
            std::vector<std::pair<std::string, std::string>> headers;
            std::string response_body;

        public:
            HttpResponse() 
                : status_line(HttpStatus::OK) 
            {
                // headers.push_back({"Content-Type", "text/plain"});
            }

            void set_status(const std::string& status) {
                status_line = status;
            }

            void set_header(const std::string& key, const std::string& value) {
                headers.push_back({key, value});
            }

            void set_body(const std::string& body) {
                response_body = body;
            }

            std::string to_string() const {
                std::ostringstream oss;
                oss << status_line;

                for (const auto& [k, v] : headers) {
                    oss << k << ": " << v << "\r\n";
                }

                oss << "\r\n";  // End of headers
                oss << response_body;

                return oss.str();
            }
    };
}