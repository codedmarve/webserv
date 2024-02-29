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

// HttpRequestParser.cpp
#include "HttpRequestParser.hpp"
#include "HttpRequestParser.hpp"
#include <iostream>
#include <sstream>
#include <exception> // Added for std::terminate()

HttpRequestParser::HttpRequestParser() : content_length_(0) {}

HttpRequestParser::~HttpRequestParser() {}

int HttpRequestParser::parseRequest(const std::string &request) {
    size_t pos = request.find("\r\n\r\n");
    if (pos != std::string::npos) {
        std::string headerLines = request.substr(0, pos);
        body_ = request.substr(pos + 4); // Skip "\r\n\r\n"

        size_t endOfFirstLine = headerLines.find("\r\n");
        if (endOfFirstLine != std::string::npos) {
            std::string firstLine = headerLines.substr(0, endOfFirstLine);
            std::istringstream iss(firstLine);
            if (!(iss >> method_ >> target_ >> protocol_)) {
                throw std::invalid_argument("Invalid request line format");
            }

            parseHeaders(headerLines.substr(endOfFirstLine + 2)); // Skip "\r\n"
            parseContentLength();

            return 0; // Successfully parsed
        }
    }

    throw std::invalid_argument("Invalid request format");
}

void HttpRequestParser::parseHeaders(const std::string &headerLines) {
    std::istringstream iss(headerLines);
    std::string line;
    while (std::getline(iss, line, '\r')) {
        if (!line.empty()) {
            size_t pos = line.find(": ");
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 2);
                headers_[key] = value;
            }
        }
    }
}

void HttpRequestParser::parseContentLength() {
    std::map<std::string, std::string>::const_iterator it = headers_.find("Content-Length");
    if (it != headers_.end()) {
        std::istringstream iss(it->second);
        if (!(iss >> content_length_)) {
            throw std::invalid_argument("Invalid Content-Length value");
        }
    }
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

void HttpRequestParser::printRequest(const std::string& request) {
    HttpRequestParser parser;
    try {
        parser.parseRequest(request);
        std::cout << "Method: " << parser.getMethod() << std::endl;
        std::cout << "Target: " << parser.getTarget() << std::endl;
        std::cout << "Protocol: " << parser.getProtocol() << std::endl;
        std::cout << "Headers:" << std::endl;
        const std::map<std::string, std::string>& headers = parser.getHeaders();
        std::map<std::string, std::string>::const_iterator it;
        for (it = headers.begin(); it != headers.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        std::cout << "Body: " << parser.getBody() << std::endl;
        std::cout << "Content-Length: " << parser.getContentLength() << std::endl;
        std::cout << "-------------------------------\n";
    } catch (const std::exception& e) {
        std::cerr << "Error parsing request: " << e.what() << std::endl;
    }
}
