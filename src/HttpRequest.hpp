#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "./StringUtils.hpp"

namespace HttpRequest
{
    class RequestLine
    {
    public:
        std::string method;
        std::string target;
        std::string version;

        // Constructor
        RequestLine(const std::string &m = "", const std::string &t = "", const std::string &v = "")
            : method(m), target(t), version(v) {}

        RequestLine(const std::vector<std::string> &arr)
        {
            if (arr.size() != 3)
                throw std::runtime_error("Request Line must contain three elements");
            method = arr[0];
            target = arr[1];
            version = arr[2];
        }
    };

    // Full HTTP request class
    class HttpRequest
    {
    public:
        RequestLine requestLine;
        std::unordered_map<std::string, std::string> headers;
        std::string body;

        // Constructors
        HttpRequest() = default;

        HttpRequest(const RequestLine &rl,
                    const std::unordered_map<std::string, std::string> &h = {},
                    const std::string &b = "")
            : requestLine(rl), headers(h), body(b) {}

        std::string getHeader(const std::string &name) const
        {
            auto it = headers.find(name);
            return it != headers.end() ? it->second : "";
        }

        void setHeader(const std::string &name, const std::string &value)
        {
            headers[StringUtils::lower(name)] = value;
        }

        void setBody(const std::string &body)
        {
            this->body = body;
        }

        void setRequestLines(const std::string &request_line)
        {
            std::vector<std::string> req_line_arr = StringUtils::split(request_line, " ");
            requestLine = RequestLine(req_line_arr);
        }

        size_t getContentLength() {
            if (headers.find("content-length") == headers.end()) return 0;
            return (size_t)std::stoi(headers["content-length"]);
        }

        static HttpRequest parse_request(std::string &req)
        {
            std::vector<std::string> lines = StringUtils::split(req, "\r\n");
            int n = lines.size();
            if (n == 0)
                throw std::runtime_error("Empty request!");

            HttpRequest httpReq;
            httpReq.setRequestLines(lines[0]);

            int i = 1;
            while (i < n && !lines[i].empty())
            {
                std::string line = lines[i];
                size_t pos = line.find(':');
                if (pos == std::string::npos)
                {
                    throw std::runtime_error("Invalid header: missing ':'");
                }
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                httpReq.setHeader(StringUtils::lower(key), StringUtils::trim(value));
                i++;
            }

            if (i < n && lines[i].empty())
            {
                i++;
            }

            std::string body;
            while (i < n)
            {
                body += lines[i];
                i++;
            }
            httpReq.setBody(body);
            return httpReq;
        }

        static HttpRequest from_headers(const std::string& header_text)
        {
            HttpRequest req;

            size_t pos = 0;
            size_t line_end = header_text.find("\r\n", pos);

            std::vector<std::string> header_eles = StringUtils::split(header_text, "\r\n");

            // Set request line:
            req.setRequestLines(header_eles[0]);
            int n = header_eles.size();
            // Set header
            int i = 1;
            while (i < n && !header_eles[i].empty())
            {
                std::string line = header_eles[i];
                size_t pos = line.find(':');
                if (pos == std::string::npos)
                {
                    throw std::runtime_error("Invalid header: missing ':'");
                }
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                req.setHeader(StringUtils::lower(key), StringUtils::trim(value));
                i++;
            }
            return req;
        }
    };
}