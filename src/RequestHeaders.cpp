#include "../inc/HttpRequestParser.hpp"

// int HttpRequestParser::parseHeaders(const std::string& headerLines) {
//     std::istringstream iss(headerLines);
//     std::string line;
//     std::string currentHeader;

//     // std::cout << "DEBUG\n";
//     while (std::getline(iss, line, '\n')) {
//         line = trim(line);

//         if (line.empty()) // Empty line indicates end of headers
//             break;

//         if (line[0] == ' ' || line[0] == '\t') {
//             if (!currentHeader.empty()) { // Multi-line header continuation
//                 headers_[currentHeader] += " " + line; // Append to the previous header value
//             }
//         } else {
//             // New header line
//             size_t pos = line.find(":");
//             if (pos != std::string::npos) {
//                 std::string key = trim(line.substr(0, pos));
//                 std::string value = trim(line.substr(pos + 1));
//                 headers_[key] = value;
//                 currentHeader = key; // Save current header for multi-line continuation

//                 // Check for "Host" header
//                 if (key == "Host" && value.empty() && authority_.empty()) {
//                     std::cerr << "Empty 'Host' header." << std::endl;
//                     // [important] validate Host header here
//                     return 400; // Bad Request: Empty Host header
//                 }

//                 // Check for "Transfer-Encoding" header
//                 if (key == "Transfer-Encoding" && value == "chunked") {
//                     isChunked_ = true;
//                 }

//                 // Check for "Content-Length" header
//                 if (key == "Content-Length") {
//                     // Check if the value is a valid integer
//                     if (value.find_first_not_of("0123456789") != std::string::npos) {
//                         std::cerr << "Invalid 'Content-Length' header value." << std::endl;
//                         return 400; // Bad Request: Invalid Content-Length header value
//                     }

//                     // Convert the value to an integer
//                     try {
//                         std::istringstream iss(value);
//                         iss >> length_;
//                         if (length_ < 0) {
//                             throw std::invalid_argument("Negative content-length");
//                         }
//                     } catch (const std::exception& e) {
//                         std::cerr << "Error parsing 'Content-Length' header: " << e.what() << std::endl;
//                         return 400; // Bad Request: Error parsing Content-Length header
//                     }
//                 } 
//             }
//         }
//     }
//     if ((headers_.find("Host") != headers_.end() && authority_.empty()))
//         return 400;

//     return 200; // OK: Headers parsed successfully

// }


















int HttpRequestParser::parseHeaders() {
    size_t headerEnd;
    size_t colonPos;
    std::string headerKey;
    std::string headerVal;

    while ((headerEnd = req_buffer_.find("\r\n")) != std::string::npos) {
        // if /r/n is in index 0 it would mean end of headers
        if (req_buffer_.find("\r\n") == 0) {
            req_buffer_.erase(0, headerEnd + 2);
            buffer_section_ = PREBODY;
            break;
        }

        // find header key
        if ((colonPos = req_buffer_.find(':', 0)) != std::string::npos) {
            if (colonPos == 0 || req_buffer_[colonPos - 1] == ' ')
                return 400; // Bad Request: Invalid header format

            headerKey = req_buffer_.substr(0, colonPos);
            headerVal = req_buffer_.substr(colonPos + 1, headerEnd - colonPos - 1);

            // Remove leading and trailing whitespaces from header and value
            headerKey = trimmer(headerKey);
            headerVal = trimmer(headerVal);

            // Check for valid header format
            if (!isValidHeaderFormat(headerKey, headerVal))
                return 400; // Bad Request: Invalid header format
            

             // Special handling for headers
            try {
                handleSpecialHeaders(headerKey, headerVal);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error processing header: " << e.what() << std::endl;
                return 400; // Bad Request: Invalid header value
            }

            // Store the header key-value pair
            headers_[headerKey] = headerVal;
        } else {
            return 400; // Bad Request: Invalid header format
        }
        req_buffer_.erase(0, headerEnd + 2);
    }

    return 200; // OK: Headers parsed successfully
}

bool HttpRequestParser::isValidHeaderFormat(const std::string& header, const std::string& value) {
    // Check for empty header or value
    if (header.empty() || value.empty())
        return false;

    // Check for valid header characters
    for (std::string::const_iterator it = header.begin(); it != header.end(); ++it) {
        if (!isValidHeaderChar(*it))
            return false;
    }

    // Check for valid value characters
    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it) {
        if (!isValidHeaderValueChar(*it))
            return false;
    }

    return true;
}


bool HttpRequestParser::isValidHeaderChar(char c) {
    // Valid characters for header: A-Z, a-z, 0-9, '-', '_', and '.'
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.';
}

bool HttpRequestParser::isValidHeaderValueChar(char c) {
    // Valid characters for header value: Any printable ASCII character except ':'
    return (c >= 32 && c <= 126) && c != ':';
}

void HttpRequestParser::handleSpecialHeaders(const std::string& header, const std::string& value) {
    // Check for "Transfer-Encoding" header
    if (header == "Transfer-Encoding" && value == "chunked") {
        isChunked_ = true;
    }

    // Check for "Content-Length" header
    if (header == "Content-Length") {
        // Check if the value is a valid integer
        if (value.find_first_not_of("0123456789") != std::string::npos) {
            std::cerr << "Invalid 'Content-Length' header value." << std::endl;
            throw std::invalid_argument("Invalid 'Content-Length' header value");
        }

        // Convert the value to an integer
        try {
             std::istringstream iss(value);
            iss >> length_;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing 'Content-Length' header: " << e.what() << std::endl;
            throw std::invalid_argument("Error parsing 'Content-Length' header");
        }
    }

    // Check for "Host" header
    if (header == "Host") {
        // If Host header is empty or contains '@', it's invalid
        if (value.empty() || value.find('@') != std::string::npos) {
            std::cerr << "Invalid 'Host' header value." << std::endl;
            throw std::invalid_argument("Invalid 'Host' header value");
        }
    }

    // Method Check
    if (header == "Method") {
        // If Method is not "POST" or "PUT", it's invalid
        if (value != "POST" && value != "PUT") {
            std::cerr << "Unsupported HTTP method: " << value << std::endl;
            throw std::invalid_argument("Unsupported HTTP method");
        }
    }

    // Add additional special headers handling as needed
}


std::string HttpRequestParser::trimmer(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start == std::string::npos || end == std::string::npos)
        return "";
    return str.substr(start, end - start + 1);
}
