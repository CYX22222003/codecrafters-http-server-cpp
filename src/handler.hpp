#pragma once

#include <stdexcept>
#include <regex>
#include <cstring>


#include "./HttpRequest.hpp"
#include "./HttpStatus.hpp"
#include "./HttpResponse.hpp"
#include "./FileHandler.hpp"

namespace rules {
    std::regex echo_r("^/echo/([^/]+)$");
    std::regex files_r("^/files/([^/]+)$");
}

namespace handler {
    std::string handle(HttpRequest::HttpRequest req, std::string directory) {
        HttpResponse::HttpResponse httpResponse;
        std::smatch match;
        std::string target = req.requestLine.target;
        if (target == "/") {
            httpResponse.set_status(HttpStatus::OK);
        } else if (std::regex_match(target, match, rules::files_r)) {
            std::string file_name = match[1];
            FileHandler::FileHandler fh(directory, file_name, httpResponse);
            fh.handle();
        } else if (std::regex_match(target, match, rules::echo_r)) {
            std::string body = match[1];
            httpResponse.write_text_plain(StringUtils::trim(body));
        } else if (target == "/user-agent") {
            auto headers = req.headers;
            auto it = headers.find("user-agent");
            if (it == headers.end()) throw std::runtime_error("Does not contain User-Agent body");
            std::string body = it->second;
            httpResponse.write_text_plain(StringUtils::trim(body));
        } else {
            httpResponse.set_status(HttpStatus::NotFound);
        }
        
        // debug
        std::cout << StringUtils::showCRLF(httpResponse.to_string()) << std::endl;
        return httpResponse.to_string();
    }
}