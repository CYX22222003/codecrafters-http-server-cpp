#pragma once
#include<string>
#include<vector>
#include <iostream>
#include <iomanip>
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

    std::string lower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
        return result;
    }

    void print_hex(const std::string& data) {
        for (unsigned char c : data) {
            // print each byte as 2-digit hex
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
        }
        std::cout << std::dec << std::endl; // reset formatting
    }

}