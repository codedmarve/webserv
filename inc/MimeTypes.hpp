#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include "./AllHeaders.hpp"

class MimeTypes {
private:
    std::map<std::string, std::string> mimeMap;

public:
    MimeTypes();
    ~MimeTypes();

    std::string getType(const std::string& extension);
};

#endif
