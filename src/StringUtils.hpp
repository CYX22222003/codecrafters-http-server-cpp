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

    std::string showCRLF(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '\r') result += "\\r";
            else if (c == '\n') result += "\\n";
            else result += c;
        }
        return result;
    }

    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end   = s.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        return s.substr(start, end - start + 1);
    }
}