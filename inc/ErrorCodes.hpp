#ifndef ERRORCODES_HPP
#define ERRORCODES_HPP

#include "AllHeaders.hpp"

class ErrorCodes {
private:
    std::map<int, std::string> errorMap;

public:
    ErrorCodes();
    ~ErrorCodes();

    ErrorCodes (const ErrorCodes &rhs);
    ErrorCodes &operator=(const ErrorCodes &rhs);

    std::string getErrorMessage(int errorNumber);
};

#endif 
