#ifndef ERRORCODES_HPP
#define ERRORCODES_HPP

#include "AllHeaders.hpp"

class ErrorCodes {
private:
    std::map<int, std::string> errorMap;

public:
    ErrorCodes();
    ~ErrorCodes();

    std::string getErrorMessage(int errorNumber);
};

#endif 
