#include "../inc/HttpRequestParser.hpp"

HttpRequestParser::HttpRequestParser() {}

int HttpRequestParser::parseRequest(const std::string &request) {
    size_t pos = request.find("\r\n\r\n");
    if (pos != std::string::npos) {
        std::string headerLines = request.substr(0, pos);
        body_ = request.substr(pos + 4); // Skip "\r\n\r\n"

        size_t endOfFirstLine = headerLines.find("\r\n");
        if (endOfFirstLine != std::string::npos) {
            std::string requestLine = headerLines.substr(0, endOfFirstLine);
            std::istringstream iss(requestLine);
            if (!(iss >> method_ >> uri_ >> protocol_)) {
                return 400;
            }

            try {
                parseMethod(); // Call the parseMethod function
                parseHeaders(headerLines.substr(endOfFirstLine + 2)); // Skip "\r\n"
                return 200; // Successfully parsed
            } catch (const std::invalid_argument& e) {
                // Invalid method or character in method
                return 405; // Method Not Allowed
            }
        } else {
            return 400;
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

void HttpRequestParser::parseChunkedBody(const std::string &chunkedBody) {
    size_t pos = 0;
    while (pos < chunkedBody.size()) {
        size_t chunkSizePos = chunkedBody.find("\r\n", pos);
        if (chunkSizePos == std::string::npos)
            throw std::invalid_argument("Invalid chunked body");

        std::string chunkSizeStr = chunkedBody.substr(pos, chunkSizePos - pos);
        size_t chunkSize;
        std::istringstream iss(chunkSizeStr);
        if (!(iss >> std::hex >> chunkSize))
            throw std::invalid_argument("Invalid chunk size format");

        pos = chunkSizePos + 2; // Move past "\r\n"

        if (chunkSize == 0) {
            // Last chunk
            break;
        }

        size_t nextChunkPos = pos + chunkSize;
        if (nextChunkPos > chunkedBody.size()) {
            throw std::invalid_argument("Incomplete chunked body");
        }

        // Append this chunk to the body
        body_ += chunkedBody.substr(pos, chunkSize);

        pos = nextChunkPos + 2; // Move past chunk data and "\r\n"
    }
}

void HttpRequestParser::parseContentLength() {
    std::map<std::string, std::string>::iterator it = headers_.find("Content-Length");
    if (it != headers_.end()) {
        std::istringstream iss(it->second);
        size_t contentLength;
        if (!(iss >> contentLength))
            throw std::invalid_argument("Invalid Content-Length value");

        if (contentLength > body_.size())
            throw std::invalid_argument("Incomplete body");

        body_ = body_.substr(0, contentLength);
    }
}

std::string HttpRequestParser::getMethod() const {
    return method_;
}

std::string HttpRequestParser::getURI() const {
    return uri_;
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

void HttpRequestParser::printRequest(const std::string& request) {
    HttpRequestParser parser;
    try {
        parser.parseRequest(request);
        std::cout << "Method: " << parser.getMethod() << std::endl;
        std::cout << "Target: " << parser.getURI() << std::endl;
        std::cout << "Protocol: " << parser.getProtocol() << std::endl;
        std::cout << "Headers:" << std::endl;
        const std::map<std::string, std::string>& headers = parser.getHeaders();
        std::map<std::string, std::string>::const_iterator it;
        for (it = headers.begin(); it != headers.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        std::cout << "Body: " << parser.getBody() << std::endl;
        std::cout << "-------------------------------\n";
    } catch (const std::exception& e) {
        std::cerr << "Error parsing request: " << e.what() << std::endl;
    }
}


int HttpRequestParser::parseMethod() {
    // Set of valid HTTP methods
    static const std::string validMethodsArray[] = {
        "GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS", "HEAD", "TRACE", "CONNECT"
    };
    static const std::set<std::string> validMethods(validMethodsArray, validMethodsArray + sizeof(validMethodsArray) / sizeof(validMethodsArray[0]));

    // Check if the method is valid and in uppercase
    if (validMethods.find(method_) == validMethods.end()) {
        throw std::invalid_argument("Invalid HTTP method");
    }

    // Check each character for validity
    for (std::string::const_iterator it = method_.begin(); it != method_.end(); ++it) {
        if (!isMethodCharValid(*it)) {
            throw std::invalid_argument("Invalid character in HTTP method");
        }
    }
    return 200; // OK
}

bool HttpRequestParser::isMethodCharValid(char ch) const {
    // According to RFC 7230, valid characters for method are:
    // tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*"
    //        / "+" / "-" / "." / "^" / "_" / "`" / "|"
    //        / "~" / DIGIT / ALPHA
    return (ch == '!' || ch == '#' || ch == '$' || ch == '%' || ch == '&' || ch == '\'' ||
            ch == '*' || ch == '+' || ch == '-' || ch == '.' || ch == '^' || ch == '_' ||
            ch == '`' || ch == '|' || ch == '~' || std::isdigit(ch) || std::isalpha(ch));
}



