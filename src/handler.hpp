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
    std::string handle_get(HttpRequest::HttpRequest&, std::string&);
    std::string handle_post(HttpRequest::HttpRequest&, std::string&);
    bool add_compression(HttpRequest::HttpRequest&, HttpResponse::HttpResponse&);

    std::string handle(HttpRequest::HttpRequest req, std::string directory) {
        std::string method = req.requestLine.method;
        if (method == "GET") {
            return handle_get(req, directory);
        } else if (method == "POST") {
            return handle_post(req, directory);
        } else {
            throw std::runtime_error("Unknown methods");
        }
    }

    std::string handle_get(HttpRequest::HttpRequest& req, std::string& directory) {
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
            std::string body = match[1];
            if (is_compressed) body = Compression::gzip_compress_string(body);
            httpResponse.write_text_plain(StringUtils::trim(body));
        } else if (target == "/user-agent") {
            auto headers = req.headers;
            auto it = headers.find("user-agent");
            if (it == headers.end()) throw std::runtime_error("Does not contain User-Agent body");
            std::string body = it->second;
            if (is_compressed) body = Compression::gzip_compress_string(body);
            httpResponse.write_text_plain(StringUtils::trim(body));
        } else {
            httpResponse.set_status(HttpStatus::NotFound);
        }
        return httpResponse.to_string();
    }

    std::string handle_post(HttpRequest::HttpRequest& req, std::string& directory) {
        std::smatch match;
        HttpResponse::HttpResponse httpResponse;
        bool is_compressed = add_compression(req, httpResponse);

        std::string target = req.requestLine.target;
        if (std::regex_match(target, match, rules::files_r)) {
            std::string file_name = match[1];
            FileHandler::FileHandler fh(directory, file_name, httpResponse);
            fh.handle_post(req.body);
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