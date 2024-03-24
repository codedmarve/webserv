#ifndef HTTPSTATUSCODES_HPP
#define HTTPSTATUSCODES_HPP

#include "./AllHeaders.hpp"

class HttpStatusCodes {
private:
    std::map<int, std::string> codeMap;

public:
    HttpStatusCodes();
    ~HttpStatusCodes();
    std::string getStatusCode(int code);
};

#endif