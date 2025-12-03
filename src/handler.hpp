#pragma one
#include "./HttpRequest.hpp"
#include "./HttpStatus.hpp"
#include "./HttpResponse.hpp"
#include <stdexcept>
#include <regex>
#include <cstring>

namespace rules {
    std::regex echo_r("^/echo/([^/]+)$");
}

namespace handler {
    std::string handle(HttpRequest::HttpRequest req) {
        HttpResponse::HttpResponse httpResponse;
        std::smatch match;
        std::string target = req.requestLine.target;
        if (target == "/") {
            httpResponse.set_status(HttpStatus::OK);
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