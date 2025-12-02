#pragma once
#include<string>
#include<vector>

namespace StringUtils {
    std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
        std::vector<std::string> result;
        size_t pos = 0, next;
        while ((next = s.find(delimiter, pos)) != std::string::npos) {
            result.push_back(s.substr(pos, next - pos));
            pos = next + delimiter.length();
        }
        result.push_back(s.substr(pos));
        return result;
    }
}