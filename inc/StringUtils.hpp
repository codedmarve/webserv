#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>

// Trim from start (in-place)
void ltrim(std::string &s);

// Trim from end (in-place)
void rtrim(std::string &s);

// Trim from both ends (in-place)
void trim(std::string &s);

// Check if a string starts with a prefix
bool starts_with(const std::string &s, const std::string &prefix);

// Convert string to integer
int stoi(const std::string &str);

#endif
