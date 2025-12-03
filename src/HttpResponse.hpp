#pragma one
#include "./HttpStatus.hpp"
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <utility>
#include <unordered_map>
#include <stdexcept>

namespace HttpResponse {
    class HttpHeaders {
        private:
            std::unordered_map<std::string, std::string> map_;
            std::vector<std::string> order_;
        public:
            void set(const std::string& key, const std::string& value) {
                if (map_.find(key) == map_.end()) {
                    order_.push_back(key);  // preserve insertion order
                }
                map_[key] = value;
            }

            const std::string& get(const std::string& key) const {
                auto it = map_.find(key);
                if (it == map_.end()) {
                    throw std::runtime_error("Header not found: " + key);
                }
                return it->second;
            }

            bool contains(const std::string& key) const {
                return map_.find(key) != map_.end();
            }
            
            std::string to_string() const {
                std::ostringstream oss;
                for (const auto& key : order_) {
                    oss << key << ": " << map_.at(key) << "\r\n";
                }
                return oss.str();
            }
    };

    class HttpResponse {
        private:
            std::string status_line;
            HttpHeaders headers;
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
                headers.set(key, value);
            }

            void set_body(const std::string& body) {
                response_body = body;
            }

            void write_text_plain(const std::string& body) {
                this->set_status(HttpStatus::OK);
                this->set_header("Content-Type", "text/plain");
                this->set_header("Content-Length", std::to_string(body.size()));
                this->set_body(body);
            }

            std::string to_string() const {
                std::ostringstream oss;
                oss << status_line;

                oss << headers.to_string();

                oss << "\r\n";  // End of headers
                oss << response_body;

                return oss.str();
            }
    };
}