#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h> 

#include "./HttpResponse.hpp"

namespace FileHandler {
    class FileHandler {
        private:
            std::string path;
            std::string directory;
            HttpResponse::HttpResponse& response;

            bool file_exists(const std::string& full_path) {
                struct stat buffer;
                return (stat(full_path.c_str(), &buffer) == 0);
            }

        public:
            FileHandler(const std::string& directory, const std::string& path, HttpResponse::HttpResponse& response)
                : path(path), directory(directory), response(response){}
            
            void handle() {
                std::string full_path = directory + "/" + path;

                if (file_exists(full_path)) {
                    std::ifstream infile(full_path, std::ios::binary);
                    if (!infile) {
                        response.set_status(HttpStatus::NotFound);
                        return;
                    }

                    std::stringstream buffer;
                    buffer << infile.rdbuf();
                    std::string file_content = buffer.str();
                    response.set_status(HttpStatus::OK);
                    response.set_header("Content-Type", "application/octet-stream");
                    response.set_header("Content-Length", std::to_string(file_content.size()));
                    response.set_body(file_content);
                } else {
                    response.set_status(HttpStatus::NotFound);
                }
            }
    };
}