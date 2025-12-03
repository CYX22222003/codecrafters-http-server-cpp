#pragma one
#include "./HttpRequest.hpp"
#include "./HttpStatus.hpp"
#include "./HttpResponse.hpp"
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
            httpResponse.set_status(HttpStatus::OK);
            httpResponse.set_header("Content-Type", "text/plain");
            httpResponse.set_header("Content-Length", std::to_string(body.size()));
            httpResponse.set_body(body);
        } else {
            httpResponse.set_status(HttpStatus::NotFound);
        }
        
        // debug
        std::cout << StringUtils::showCRLF(httpResponse.to_string()) << std::endl;
        return httpResponse.to_string();
    }
}