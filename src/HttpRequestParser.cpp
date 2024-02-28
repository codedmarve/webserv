#include "../inc/HttpRequestParser.hpp"

// HttpRequestParser::HttpRequestParser() : content_length_(0) {}

// HttpRequestParser::~HttpRequestParser() {}

// int HttpRequestParser::parseRequest(const std::string &request) {
//     size_t pos = request.find("\r\n\r\n");
//     if (pos != std::string::npos) {
//         std::string headerLines = request.substr(0, pos);
//         body_ = request.substr(pos + 4); // Skip "\r\n\r\n"

//         size_t endOfFirstLine = headerLines.find("\r\n");
//         if (endOfFirstLine != std::string::npos) {
//             std::string firstLine = headerLines.substr(0, endOfFirstLine);
//             std::istringstream iss(firstLine);
//             iss >> method_ >> target_ >> protocol_;

//             parseHeaders(headerLines.substr(endOfFirstLine + 2)); // Skip "\r\n"
//             return parseContentLength();
//         }
//     }
//     return -1; // Parsing failed
// }

// void HttpRequestParser::parseHeaders(const std::string &headerLines) {
//     std::istringstream iss(headerLines);
//     std::string line;
//     while (std::getline(iss, line, '\n')) {
//         size_t pos = line.find(": ");
//         if (pos != std::string::npos) {
//             std::string key = line.substr(0, pos);
//             std::string value = line.substr(pos + 2);
//             headers_[key] = value;
//         }
//     }
// }

// int HttpRequestParser::parseContentLength() {
//     std::map<std::string, std::string>::iterator it = headers_.find("Content-Length");
//     if (it != headers_.end()) {
//         content_length_ = stringToInt(it->second);
//         return 0; // Successfully parsed content length
//     }
//     return -1; // Content-Length header not found
// }

// int HttpRequestParser::stringToInt(const std::string &str) {
//     std::istringstream iss(str);
//     int result;
//     iss >> result;
//     return result;
// }

// int HttpRequestParser::getContentLength() {
//     return content_length_;
// }

// std::string HttpRequestParser::getMethod() const {
//     return method_;
// }

// std::string HttpRequestParser::getTarget() const {
//     return target_;
// }

// std::string HttpRequestParser::getProtocol() const {
//     return protocol_;
// }

// std::map<std::string, std::string> HttpRequestParser::getHeaders() const {
//     return headers_;
// }

// std::string HttpRequestParser::getBody() const {
//     return body_;
// }

#include "HttpRequestParser.hpp"
#include <iostream>
#include <sstream>

HttpRequestParser::HttpRequestParser() : content_length_(0) {}

HttpRequestParser::~HttpRequestParser() {}

int HttpRequestParser::parseRequest(const std::string &request) {
    std::cout << "Parsing Request:\n" << request << std::endl;

    size_t pos = request.find("\r\n\r\n");
    if (pos != std::string::npos) {
        std::string headerLines = request.substr(0, pos);
        body_ = request.substr(pos + 4); // Skip "\r\n\r\n"

        size_t endOfFirstLine = headerLines.find("\r\n");
        if (endOfFirstLine != std::string::npos) {
            std::string firstLine = headerLines.substr(0, endOfFirstLine);
            std::istringstream iss(firstLine);
            iss >> method_ >> target_ >> protocol_;

            std::cout << "Method: " << method_ << ", Target: " << target_ << ", Protocol: " << protocol_ << std::endl;

            parseHeaders(headerLines.substr(endOfFirstLine + 2)); // Skip "\r\n"
            int contentLengthResult = parseContentLength();
            if (contentLengthResult == 0) {
                std::cout << "Content-Length: " << content_length_ << std::endl;
            } else {
                std::cout << "Content-Length not found in headers." << std::endl;
            }

            std::cout << "Headers:" << std::endl;
            for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); ++it) {
                std::cout << it->first << ": " << it->second << std::endl;
            }

            std::cout << "Body:\n" << body_ << std::endl;
            return 0; // Successfully parsed
        }
    }

    std::cout << "Parsing failed." << std::endl;
    return -1; // Parsing failed
}

void HttpRequestParser::parseHeaders(const std::string &headerLines) {
    std::istringstream iss(headerLines);
    std::string line;
    while (std::getline(iss, line, '\n')) {
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            headers_[key] = value;
        }
    }
}

int HttpRequestParser::parseContentLength() {
    std::map<std::string, std::string>::iterator it = headers_.find("Content-Length");
    if (it != headers_.end()) {
        content_length_ = stringToInt(it->second);
        return 0; // Successfully parsed content length
    }
    return -1; // Content-Length header not found
}

int HttpRequestParser::stringToInt(const std::string &str) {
    std::istringstream iss(str);
    int result;
    iss >> result;
    return result;
}

std::string HttpRequestParser::getMethod() const {
    return method_;
}

std::string HttpRequestParser::getTarget() const {
    return target_;
}

std::string HttpRequestParser::getProtocol() const {
    return protocol_;
}

std::map<std::string, std::string> HttpRequestParser::getHeaders() const {
    return headers_;
}

std::string HttpRequestParser::getBody() const {
    return body_;
}

int HttpRequestParser::getContentLength() const {
    return content_length_;
}
