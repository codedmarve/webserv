#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include "./AllHeaders.hpp"

std::string to_lower(std::string s);
std::string to_upper(std::string s);
std::string trim_left(std::string str, char c);
std::string trim_right(std::string str, char c);
std::string unique_char(std::string str);
int stoi(const std::string &str, std::size_t *pos = 0, int base = 10);

template<typename T>
std::string to_string(T number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

struct s_compare {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return to_lower(lhs) < to_lower(rhs);
    };
};

#endif
