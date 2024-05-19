#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include "./AllHeaders.hpp"

class MimeTypes {
private:
    std::map<std::string, std::string> mimeMap;

public:
    MimeTypes();
    ~MimeTypes();

    MimeTypes(const MimeTypes &rhs);
    MimeTypes &operator=(const MimeTypes &rhs);

    std::string getType(const std::string& extension);
    std::map<std::string, std::string> getMap();
};

#endif
