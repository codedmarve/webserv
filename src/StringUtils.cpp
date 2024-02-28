#include "../inc/StringUtils.hpp"
#include <algorithm>
#include <sstream>

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

bool starts_with(const std::string &s, const std::string &prefix) {
    return s.compare(0, prefix.length(), prefix) == 0;
}

int stoi(const std::string &str) {
    std::istringstream ss(str);
    int result;
    ss >> result;
    return result;
}
