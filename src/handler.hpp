#pragma once

#include <stdexcept>
#include <regex>
#include <cstring>


#include "./HttpRequest.hpp"
#include "./HttpStatus.hpp"
#include "./HttpResponse.hpp"
#include "./FileHandler.hpp"
#include "./CompressionUtils.hpp"

namespace rules {
    std::regex echo_r("^/echo/([^/]+)$");
    std::regex files_r("^/files/([^/]+)$");
}

namespace handler {
    std::string handle_get(HttpRequest::HttpRequest&, std::string&, bool);
    std::string handle_post(HttpRequest::HttpRequest&, std::string&, bool);
    bool add_compression(HttpRequest::HttpRequest&, HttpResponse::HttpResponse&);
    
    bool isClose(HttpRequest::HttpRequest req) {
        auto headers = req.headers;
        if (headers.find("connection") != headers.end()) {
            return headers["connection"] == "close";
        }
        return false;
    }

    std::string handle(HttpRequest::HttpRequest req, std::string directory, bool isClose = false) {
        std::string method = req.requestLine.method;
        if (method == "GET") {
            return handle_get(req, directory, isClose);
        } else if (method == "POST") {
            return handle_post(req, directory, isClose);
        } else {
            throw std::runtime_error("Unknown methods");
        }
    }

    std::string handle_get(HttpRequest::HttpRequest& req, std::string& directory, bool isClose = false) {
        std::smatch match;
        HttpResponse::HttpResponse httpResponse;
        bool is_compressed = add_compression(req, httpResponse);

        std::string target = req.requestLine.target;
        if (target == "/") {
            httpResponse.set_status(HttpStatus::OK);
        } else if (std::regex_match(target, match, rules::files_r)) {
            std::string file_name = match[1];
            FileHandler::FileHandler fh(directory, file_name, httpResponse);
            fh.handle_get(is_compressed);
        } else if (std::regex_match(target, match, rules::echo_r)) {
            std::string body = StringUtils::trim(match[1]);
            if (is_compressed) {
                httpResponse.write_compressed_text(body);
            } else {
                httpResponse.write_text_plain(body);
            }
        } else if (target == "/user-agent") {
            auto headers = req.headers;
            auto it = headers.find("user-agent");
            if (it == headers.end()) throw std::runtime_error("Does not contain User-Agent body");
            std::string body = StringUtils::trim(it->second);
            if (is_compressed) {
                httpResponse.write_compressed_text(body);
            } else {
                httpResponse.write_text_plain(body);
            }
            httpResponse.write_text_plain(body);
        } else {
            httpResponse.set_status(HttpStatus::NotFound);
        }
        if (isClose) httpResponse.close_connection();
        return httpResponse.to_string();
    }

    std::string handle_post(HttpRequest::HttpRequest& req, std::string& directory, bool isClose = false) {
        std::smatch match;
        HttpResponse::HttpResponse httpResponse;
        bool is_compressed = add_compression(req, httpResponse);

        std::string target = req.requestLine.target;
        if (std::regex_match(target, match, rules::files_r)) {
            std::string file_name = match[1];
            FileHandler::FileHandler fh(directory, file_name, httpResponse);
            fh.handle_post(req.body);
            if (isClose) httpResponse.close_connection();
            return httpResponse.to_string();
        } else {
            throw std::runtime_error("Unspecified paths for posting");
        }
    }

    bool add_compression(HttpRequest::HttpRequest& req, HttpResponse::HttpResponse& resp) {
        if (req.headers.find("accept-encoding") != req.headers.end()) {
            auto arr = StringUtils::split(req.headers["accept-encoding"], ",");
            for (auto s : arr) {
                if (StringUtils::trim(s) == "gzip") {
                    resp.set_header("content-encoding", "gzip");
                    return true;
                }
            }
        }
        return false;
    }
}