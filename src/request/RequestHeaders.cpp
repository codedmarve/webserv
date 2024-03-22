#include "../../inc/HttpRequestParser.hpp"

/// @todo enforce certain header constraints specified in the RFC (e.g., no whitespace around the colon, no empty header keys).

int HttpRequestParser::parseHeaders() {

    size_t headerEnd;
    size_t colonPos;
    std::string headerKey;
    std::string headerVal;

    while ((headerEnd = req_buffer_.find("\r\n")) != std::string::npos) {
        
        if (req_buffer_.find("\r\n") == 0) { // if /r/n is in index 0 it would mean end of headers
            req_buffer_.erase(0, headerEnd + 2);
            buffer_section_ = SPECIAL_HEADERS;
            break;
        }

        // find headerKey
        if ((colonPos = req_buffer_.find(':', 0)) != std::string::npos) {
            if (colonPos == 0 || req_buffer_[colonPos - 1] == ' ')
                return 400;

            headerKey = trimmer(req_buffer_.substr(0, colonPos));
            headerVal = trimmer(req_buffer_.substr(colonPos + 1, headerEnd - colonPos - 1));

            if (!isValidHeaderFormat(headerKey))
                return 400;

            headers_[headerKey] = headerVal;
        } else {
            return 400;
        }
        req_buffer_.erase(0, headerEnd + 2);
    }

    return 200;
}

bool HttpRequestParser::isValidHeaderFormat(const std::string& header) {
    if (header.empty())
        return false;

    for (std::string::const_iterator it = header.begin(); it != header.end(); ++it) {
        if (!isValidHeaderChar(*it)) {
            return false;
        }
    }
    return true;
}


bool HttpRequestParser::isValidHeaderChar(unsigned char c) {
    // Valid characters: Visible ASCII characters and obs-text(obsolete-text) (128-255) => Check ASCII table 
    // return (c >= 0x21 && c <= 0x7E) || (c >= 0x80 && c <= 0xFF);
    return (c >= 0x21 && c <= 0x7E) || ((c & 0x80) != 0 && c != 0x7F);
}

bool HttpRequestParser::checkSpecialHeaders() {
    if (headers_.count("Host")) {
        std::string value = headers_["Host"];
        if (value.empty() || value.find('@') != std::string::npos) {
            std::cerr << "Invalid 'Host' header value." << std::endl;
            return false;
        }
        /// @todo validate host using our validate uri method and make "@" valid
    }

    if (headers_.count("Transfer-Encoding")) {
        std::string value = headers_["Transfer-Encoding"];
        if (value == "chunked") {
            isChunked_ = true;
            chunk_status_ = CHUNK_SIZE;
            buffer_section_ = CHUNK;
        } else {
            return false;
        }
    } else if (headers_.count("Content-Length")) {
        std::string value = headers_["Content-Length"];
        if (value.find_first_not_of("0123456789") != std::string::npos) {
            std::cerr << "Invalid 'Content-Length' header value." << std::endl;
            return false;
        }
        try {
            // Convert the value to an integer
            std::istringstream iss(value); 
            iss >> length_;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing 'Content-Length' header: " << e.what() << std::endl;
            return false;
        }
        buffer_section_ = BODY;
    } else {
        return false; 
    }



    if (headers_.count("Method")) {
        /// @todo this part needs to be thought through
        std::string value = headers_["Method"];
        if (value != "POST" && value != "PUT") {
            std::cerr << "Unsupported HTTP method: " << value << std::endl;
            throw std::invalid_argument("Unsupported HTTP method");
        }
    }

    return true;
}

std::string HttpRequestParser::trimmer(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == std::string::npos || end == std::string::npos)
        ? ""
        : str.substr(start, end - start + 1);
}
