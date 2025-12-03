#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <utility>
#include <unordered_map>
#include <stdexcept>

#include "./StringUtils.hpp"
#include "./HttpStatus.hpp"
#include "./CompressionUtils.hpp"
namespace HttpResponse {
    class HttpHeaders {
        private:
            std::unordered_map<std::string, std::string> map_;
            std::vector<std::string> order_;
        public:
            void set(const std::string& key, const std::string& value) {
                if (map_.find(StringUtils::lower(key)) == map_.end()) {
                    order_.push_back(StringUtils::lower(key));  // preserve insertion order
                }
                map_[StringUtils::lower(key)] = value;
            }

            const std::string& get(const std::string& key) const {
                auto it = map_.find(StringUtils::lower(key));
                if (it == map_.end()) {
                    throw std::runtime_error("Header not found: " + key);
                }
                return it->second;
            }

            bool contains(const std::string& key) const {
                return map_.find(StringUtils::lower(key)) != map_.end();
            }

            void insert_at(const std::string& key, const std::string& value, size_t pos) {
                std::string lower_key = StringUtils::lower(key);

                if (map_.find(lower_key) == map_.end()) {
                    if (pos > order_.size()) {
                        pos = order_.size();
                    }
                    order_.insert(order_.begin() + pos, lower_key);
                } else {
                    auto it = std::find(order_.begin(), order_.end(), lower_key);
                    if (it != order_.end()) {
                        order_.erase(it);
                        if (pos > order_.size()) {
                            pos = order_.size();
                        }
                        order_.insert(order_.begin() + pos, lower_key);
                    }
                }
                map_[lower_key] = value;
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
                this->set_header("content-type", "text/plain");
                this->set_header("content-length", std::to_string(body.size()));
                this->set_body(body);
            }

            void write_compressed_text(const std::string& body) {
                std::string compressed_body = Compression::compress_gzip(body);
                this->set_status(HttpStatus::OK);
                this->set_header("content-type", "text/plain");
                this->set_header("content-length", std::to_string(compressed_body.size()));
                StringUtils::print_hex(compressed_body);
                this->set_body(compressed_body);
            }

            void close_connection() {
                this->set_header("connection:", "close");
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